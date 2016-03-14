/*Created by Satvik Kishore
Some Flow*/
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
    std::ifstream myfile ("Flow.cl");
    if (myfile.is_open())
    {
        while ( getline (myfile,line) )
        {
            kersource=kersource+line;
            kersource=kersource+"\n";
        }
        myfile.close();
    }
    std::ofstream outfile;;
    outfile.open("FlowLog.txt");
    outfile<<kersource;
    outfile.close();
    const char* kernelSource  = kersource.c_str();

    unsigned int n = 1000;
     
    float *h_a;  
    
 
    cl::Buffer d_a;
    
 
    size_t bytes = n*n*sizeof (float);
 
    h_a = new float[n*n];
    for(int i = 0; i < n*n; i++ )
    {
        h_a[i] = 0;
    }
    h_a[0]=1;
    int count=0;
    cl_int err = CL_SUCCESS;
    std::string buildLog;

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
        std::cout<<deviceName<<"\n";
 
        cl::CommandQueue queue(context, devices[0], 0, &err);
 
        d_a = cl::Buffer(context, CL_MEM_READ_WRITE, bytes);
  
        queue.enqueueWriteBuffer(d_a, CL_TRUE, 0, bytes, h_a);
;
        
        cl::Program::Sources source(1,
            std::make_pair(kernelSource,strlen(kernelSource)));
        cl::Program program_ = cl::Program(context, source);
        program_.build(devices);
 
        buildLog =  
        program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0]); 
        cl::Kernel kernel(program_, "letItFlow", &err);
        kernel.setArg(0, d_a);
        kernel.setArg(1, n);
 
        cl::NDRange localSize(16,16);       
        cl::NDRange globalSize((int)(ceil(n/(float)16)*16),(int)(ceil(n/(float)16)*16));

        cl::Event event;
        for(count =0;h_a[n*n-1] != 1; count++)
        {
            queue.enqueueNDRangeKernel(
                kernel,
                cl::NullRange,
                globalSize,
                localSize,
                NULL,
                &event);        
            event.wait();
            queue.enqueueReadBuffer(d_a, CL_TRUE, 0, bytes, h_a);
        }
    }
    catch (cl::Error err) {
         std::cerr
            << "ERROR: "<<err.what()<<"("<<err.err()<<")"<<std::endl;
    }

    std::ofstream ansfile;;
    ansfile.open("FlowOut.txt");
    for(int i=0;i<n;i++)
    {
        for(int j=0; j<n;j++)
        {
            ansfile<<h_a[i*n+j]<<" ";
        }
        ansfile<<"\n";
    }   
    std::cout<<"The Count is :"<<count<<"\n";
    ansfile.close();
    
    
    delete(h_a);
    //delete(h_c); 
    return 0;
}