#include "NBody2.hpp"

int main(int argc, char *argv[])
{
 
    std::string line;
    std::string kersource="";
    std::ifstream myfile ("NBody2.cl");
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            kersource=kersource+line;
            kersource=kersource+"\n";
        }
        myfile.close();
    }

    cl::Image2D img;
  
    const char* kernelSource  = kersource.c_str();
    
    unsigned int n = 4096;
    cl_float4 *pi;
    cl_float4 *pf;
    cl_float4 *v;

    cl::Buffer d_pi;
    cl::Buffer d_pf;
    cl::Buffer d_v;
    cl::LocalSpaceArg d_ploc;
    

    size_t bytes = n*sizeof(cl_float4);

    pi = new cl_float4[n];
    pf = new cl_float4[n];
    v  = new cl_float4[n];


    for(int i=0;i<n;i++)
    {
        pi[i] = { (i%16),
                  ((i/16)%16),
                  i/256,
                  1       };       
        v[i] = {0,0,0,0};
    }

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
        
        /*img = cl::Image2D(context, CL_MEM_WRITE_ONLY,
                          cl::ImageFormat(CL_RGBA, CL_UNSIGNED_INT8),
                          n,
                          n,
                          0,
                          &cvImg,
                          &err);  */

        d_pi = cl::Buffer(context, CL_MEM_READ_ONLY,  bytes);
        d_pf = cl::Buffer(context, CL_MEM_WRITE_ONLY, bytes);
        d_v  = cl::Buffer(context, CL_MEM_READ_WRITE, bytes);
        d_ploc = cl::Local(localsize*sizeof(cl_float4));

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
        kernel.setArg(4, d_ploc);
        //kernel.setArg(5, img);

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
    std::cout<<"Duration: "<<duration<<"\n";

    buildlog = program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0], NULL);
        std::ofstream logfile ("NBody2log.txt");
        logfile<<buildlog;
        logfile.close();

    std::ofstream outfile ("NBody2Ans.txt");
    for(int i=0;i<n;i++)
    {
        outfile<<pf[i].x<<" "<<pf[i].y<<" "<<pf[i].z<<"      ";
        outfile<<pi[i].x<<" "<<pi[i].y<<" "<<pi[i].z<<"      ";
        outfile<<v[i].x<<" "<<v[i].y<<" "<<v[i].z;

        outfile<<"\n";
    }
    outfile.close();
    duration = (clock() - start)/(double)CLOCKS_PER_SEC;
    std::cout<<"Final Duration: "<<duration<<"\n";

    


    //CopyMemory(cvImg , img, n*n*sizeof(CL_UNSIGNED_INT8));
    

    delete(pi);
    delete(pf);
    delete(v);
 
}