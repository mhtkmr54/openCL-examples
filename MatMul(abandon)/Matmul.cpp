/*Created by Satvik Kishore
Basic Matrix Multiplication*/
#define __CL_ENABLE_EXCEPTIONS
 
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cstdio>
#include <CL/opencl.h>
#include <CL/cl.hpp>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <math.h>

int main(int argc, char *argv[])
{    
    std::string line;
    std::string kersource="";
    std::ifstream myfile ("Matmul.cl");
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

    unsigned int n = 1024;
     
    float *h_a;
    float *h_b;   
    float *h_c;
 
    cl::Buffer d_a;
    cl::Buffer d_b;
    cl::Buffer d_c;
 
    size_t bytes = n*n*sizeof (float);
 
    h_a = new float[n*n];
    h_b = new float[n*n];
    h_c = new float[n*n];
    for(int i = 0; i < n*n; i++ )
    {
        h_a[i] = 1;
        h_b[i] = 2;
    }
 
    cl_int err = CL_SUCCESS;
    

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
        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
        
        std::string deviceName = devices[0].getInfo<CL_DEVICE_NAME>();
        size_t deviceWorkGroupSize = devices[0].getInfo<CL_DEVICE_MAX_WORK_GROUP_SIZE>();
        std::cout<<deviceName<<"\n";
        std::cout<<"WorkGroupSize: "<<deviceWorkGroupSize<<"\n";
 
        cl::CommandQueue queue(context, devices[0], 0, &err);
 
        d_a = cl::Buffer(context, CL_MEM_READ_ONLY , bytes);
        d_b = cl::Buffer(context, CL_MEM_READ_ONLY , bytes);
        d_c = cl::Buffer(context, CL_MEM_WRITE_ONLY , bytes);
  
        queue.enqueueWriteBuffer(d_a, CL_TRUE, 0, bytes, h_a);
        queue.enqueueWriteBuffer(d_b, CL_TRUE, 0, bytes, h_b);
        
        cl::Program::Sources source(1,
            std::make_pair(kernelSource,strlen(kernelSource)));
        cl::Program program_ = cl::Program(context, source);

         
        cl::Kernel kernel(program_, "multiMat", &err);
        kernel.setArg(0, d_a);
        kernel.setArg(1, d_b);
        kernel.setArg(2, d_c);
        kernel.setArg(3, n);
 
        cl::NDRange localSize(16,16);       
        cl::NDRange globalSize(1024,1024);

        cl::Event event;
        queue.enqueueNDRangeKernel(
            kernel,
            cl::NullRange,
            globalSize,
            localSize,
            NULL,
            &event);        
        event.wait();
        queue.enqueueReadBuffer(d_c, CL_TRUE, 0, bytes, h_c);
        /*std::string buildLog= "dsscdscsdcsdhbjhbjbjhbjhbhjbjhbhjbjhbhjbbhjbhbjbjhbjbhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhhh";
        program_.getBuildInfo( devices[0], (cl_program_build_info)CL_PROGRAM_BUILD_LOG, &buildLog );
        std::ofstream logfile;
        logfile.open("Matmullog.txt");
        
        logfile<<buildLog;
        logfile.close();
        */
        


    }
    catch (cl::Error err) {
         std::cerr
            << "ERROR: "<<err.what()<<"("<<err.err()<<")"<<std::endl;

        
    }
    
    
    float sum = 0;
    for(int i=0; i<n*n; i++)
        sum += h_c[i];
    std::cout<<"final result: "<<sum<<std::endl;

    std::ofstream ansfile;;
    ansfile.open("MatmulAns.txt");
    for(int i=0;i<n;i++)
    {
        for(int j=0; j<n;j++)
        {
            ansfile<<h_c[i*n+j]<<" ";
        }
        ansfile<<"\n";
    }   
    ansfile.close();
    
        


    delete(h_a);
    delete(h_b);
    delete(h_c); 
    return 0;
}