#include "NBody3.hpp"
#include <GL/glut.h> 

void Timer(int value)
{
   glutPostRedisplay();                 
   glutTimerFunc(35, Timer, 0);
}
void initGL() 
{

   glClearColor(0.0f, 0.0f, 0.0f, 0.2f); 
}
int process()
{
 
    std::string line;
    std::string kersource="";
    std::ifstream myfile ("NBody3.cl");
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            kersource=kersource+line;
            kersource=kersource+"\n";
        }
        myfile.close();
    }
 
    const char* kernelSource  = kersource.c_str();
    
    cl::Buffer d_pi;
    cl::Buffer d_pf;
    cl::Buffer d_v;
    
    cl_int err = CL_SUCCESS;
    cl::STRING_CLASS buildlog;
    cl::Program program_;
    std::vector<cl::Device> devices;

    clock_t start;
    double duration;
    start = clock(); 
    try 
    {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.size() == 0) {
            std::cout << "Platform size 0\n";
            return -1;
        }
           cl_context_properties properties[] =
           { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
        cl::Context context(CL_DEVICE_TYPE_GPU, properties);
        devices = context.getInfo<CL_CONTEXT_DEVICES>();
 
        cl::CommandQueue queue(context, devices[0], 0, &err);

        const int localsize = 64;
        
        d_pi = cl::Buffer(context, CL_MEM_READ_ONLY,  bytes);
        d_pf = cl::Buffer(context, CL_MEM_WRITE_ONLY, bytes);
        d_v  = cl::Buffer(context, CL_MEM_READ_WRITE, bytes);

        queue.enqueueWriteBuffer(d_pi, CL_TRUE, 0, bytes, pi);
        queue.enqueueWriteBuffer(d_v,  CL_TRUE, 0, bytes, v );

        cl::Program::Sources source(1,
            std::make_pair(kernelSource,strlen(kernelSource)));
        program_ = cl::Program(context, source);
        program_.build(devices);

        cl::Kernel kernel(program_, "nBody", &err);

        kernel.setArg(0, d_pi);
        kernel.setArg(1, d_pf);
        kernel.setArg(2, d_v);
        kernel.setArg(3, n);

        cl::NDRange localSize(localsize);
        cl::NDRange globalSize((int)(ceil(n/(float)64)*64));

        cl::Event event;
        queue.enqueueNDRangeKernel(
            kernel,
            cl::NullRange,
            globalSize,
            localSize,
            NULL,
            &event);

        event.wait();
 
        queue.enqueueReadBuffer(d_pf, CL_TRUE, 0, bytes, pf);
        queue.enqueueReadBuffer(d_v, CL_TRUE, 0, bytes, v);
    }
    catch (cl::Error err) {
         std::cerr
            << "ERROR: "<<err.what()<<"("<<err.err()<<")"<<std::endl;
    }

    duration = (clock() - start)/(double)CLOCKS_PER_SEC;
    buildlog = program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0], NULL);
        std::ofstream logfile ("NBody3log.txt");
        logfile<<buildlog;
        logfile.close();

    std::ofstream outfile ("NBody3Ans.txt");
    for(int i=0;i<n;i++)
    {
        outfile<<pf[i].x<<" "<<pf[i].y<<"      ";
        outfile<<pi[i].x<<" "<<pi[i].y<<"      ";
        outfile<<v[i].x<<" "<<v[i].y;

        outfile<<"\n";
    }
    outfile.close();
    duration = (clock() - start)/(double)CLOCKS_PER_SEC; 
}

void keyboard(unsigned char key , int x, int y)
{
    if(key == 'r')
    {

        for(int i=0;i<n;i++)
        {
            pi[i] = { 32*(i%32) ,32*(int)i/32 };       
            v[i] = { 0,0};
        }
    }
    if(key == 27)
        exit(0);

}
void idle()
{
    process();
    memcpy(pi,pf,bytes);
}
 
void display() 
{
   glClear(GL_COLOR_BUFFER_BIT);   
    glBegin(GL_POINTS);
      
    for(int i = 0 ;i< n; i++)
    { 
        int pos[2] ={ pf[i].x - 512 , 512 - pf[i].y};
        glColor3f(1,abs(v[i].x),abs(v[i].y));
        glVertex2f((float)pos[0]/1024 , (float)pos[1]/1024);
    } 
    glEnd();
    
   glFlush();  
}
 
int main(int argc, char** argv) 
{
    pi = new cl_float2[n];
    pf = new cl_float2[n];
    v  = new cl_float2[n];


    for(int i=0;i<n;i++)
    {
        pi[i] = { 32*(i%32) ,32*(int)i/32 };       
        v[i] = { 0,0};
    }
    glutInit(&argc, argv);
   glutCreateWindow("Vertex, Primitive & Color");  
   glutInitWindowSize(2048,2048);   
   glutInitWindowPosition(100,100); // Position the window's initial top-left corner
   glutDisplayFunc(display ); 
   glutTimerFunc(0 , Timer , 0);
   glutKeyboardFunc(keyboard); 
   glutIdleFunc(idle);     // Register callback handler for window re-paint event
   initGL();                       // Our own OpenGL initialization
   glutMainLoop();   

    delete(pi);
    delete(pf);
    delete(v); 
    std::cout<<"bye\n";             // Enter the event-processing loop
   return 0;
}