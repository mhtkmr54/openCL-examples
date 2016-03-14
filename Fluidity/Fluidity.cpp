#include "Fluidity.hpp"
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

int setupCL()
{
    std::string line;
    std::string kersource="";
    std::ifstream myfile ("Fluidity.cl");
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
    bytes = n*n*sizeof(cl_float4);
    cl_int err = CL_SUCCESS;
    cl::STRING_CLASS buildlog;
    
    try 
    {
        ;
        cl::Platform::get(&platforms);
        if (platforms.size() == 0) {
            std::cout << "Platform size 0\n";
            return -1;
        }
           cl_context_properties properties[] =
           { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
        
        context = cl::Context(CL_DEVICE_TYPE_GPU, properties);
        devices = context.getInfo<CL_CONTEXT_DEVICES>();

        d_if1234 = cl::Buffer(context, CL_MEM_READ_ONLY, bytes);
        d_if5678 = cl::Buffer(context, CL_MEM_READ_ONLY, bytes);
        d_if0    = cl::Buffer(context, CL_MEM_READ_ONLY, bytes/4);
        d_of1234 = cl::Buffer(context, CL_MEM_WRITE_ONLY, bytes);
        d_of5678 = cl::Buffer(context, CL_MEM_WRITE_ONLY, bytes);
        d_of0    = cl::Buffer(context, CL_MEM_WRITE_ONLY, bytes/4);
        d_rho    = cl::Buffer(context, CL_MEM_WRITE_ONLY, bytes/4);
        d_weight = cl::Buffer(context, CL_MEM_READ_ONLY, 9*sizeof(cl_float));

        cl::Program::Sources source(1,
        std::make_pair(kernelSource,strlen(kernelSource)));
        program_ = cl::Program(context, source);

        program_.build(devices);

        kernel = cl::Kernel(program_, "fluidify", &err);
    }
    catch (cl::Error err) {
         std::cerr
            << "ERROR: "<<err.what()<<"("<<err.err()<<")"<<std::endl;
    }

    buildlog = program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0], NULL);
        std::ofstream logfile ("Fluiditylog.txt");
        logfile<<buildlog;
        logfile.close();    
}
int process()
{
    cl_int err = CL_SUCCESS;
    try{
    cl::CommandQueue queue(context, devices[0], 0, &err);
    queue.enqueueWriteBuffer(d_if1234, CL_TRUE, 0, bytes, h_if1234);
    queue.enqueueWriteBuffer(d_if5678, CL_TRUE, 0, bytes, h_if5678);
    queue.enqueueWriteBuffer(d_if0   , CL_TRUE, 0, bytes/4, h_if0);    
  
    kernel.setArg(0, d_if1234);
    kernel.setArg(1, d_if5678);
    kernel.setArg(2, d_if0);
    kernel.setArg(3, d_of1234);
    kernel.setArg(4, d_of5678);
    kernel.setArg(5, d_of0);
    kernel.setArg(6, d_rho);
    kernel.setArg(7, ttau);
    kernel.setArg(8, d_weight);
    kernel.setArg(9, n);
        
    cl::NDRange localSize(16,16);
    cl::NDRange globalSize(n*n);      
            
    queue.enqueueNDRangeKernel(
        kernel,
        cl::NullRange,
        globalSize,
        localSize,
        NULL,
        &event);

    event.wait();

    queue.enqueueReadBuffer(d_of1234, CL_TRUE, 0, bytes, h_of1234);
    queue.enqueueReadBuffer(d_of1234, CL_TRUE, 0, bytes, h_of1234);
    queue.enqueueReadBuffer(d_of0   , CL_TRUE, 0, bytes/4, h_of0);
    queue.enqueueReadBuffer(d_rho   , CL_TRUE, 0, bytes/4, h_rho); 
    }
    catch (cl::Error err) {
         std::cerr
            << "ERROR: "<<err.what()<<"("<<err.err()<<")"<<std::endl;
    }     
    std::cout<<h_rho[5050]<<"\n";
    std::cout<<h_if1234[4020].x;
}
void idle()
{
    process();
    memcpy(h_if0    , h_of0    , bytes/4);
    memcpy(h_if1234 , h_of1234 , bytes)  ;
    memcpy(h_if5678 , h_of5678 , bytes)  ;
}
void keyboard(unsigned char key , int x, int y)
{
    /*if(key == 'r')
    {

        for(int i=0;i<n;i++)
        {
            pi[i] = { 32*(i%32) ,32*(int)i/32 };       
            v[i] = { 0,0};
        }
    }*/
    if(key == 27)
        exit(0);

}
void display() 
{
   glClear(GL_COLOR_BUFFER_BIT);   
    glBegin(GL_POINTS);
      
    for(int i = 0 ;i< n; i++)
    { 
        for(int j =0; j<n; j++)
        {
            glColor3f(1,0,10*h_rho[i*n+j]);
            glVertex2f((float)i/128-1 , (float)j/128-1);
        }
    } 
    glEnd();
    
    
   glFlush();  
}
 
int main(int argc, char** argv) 
{
    h_if1234 = new cl_float4[n*n];
    h_if5678 = new cl_float4[n*n];
    h_if0    = new cl_float[n*n];
    h_of1234 = new cl_float4[n*n];
    h_of5678 = new cl_float4[n*n];
    h_of0    = new cl_float[n*n];
    h_rho    = new cl_float[n*n];

    for(int i=0;i<n*n;i++)
    {
        h_if1234[i] = {i%32768,i%32768,i%32768,i%32768};
        h_if5678[i] = {i%32768,i%32768,i%32768,i%32768};
        h_if0[i] = i%80;
    }
    setupCL();
    glutInit(&argc, argv);
   glutCreateWindow("Vertex, Primitive & Color");  
   glutInitWindowSize(n,n);   
   glutInitWindowPosition(100,100); // Position the window's initial top-left corner
   glutDisplayFunc(display ); 
   glutTimerFunc(0 , Timer , 0);
   glutKeyboardFunc(keyboard); 
   glutIdleFunc(idle);     // Register callback handler for window re-paint event
   initGL();                       // Our own OpenGL initialization
   glutMainLoop();   

    delete(h_of1234);
    delete(h_if5678);
    delete(h_if0);
    std::cout<<"bye\n";             // Enter the event-processing loop
   return 0;
}