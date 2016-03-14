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

int main(int argc, char *argv[]){
 
    std::string line;
    std::string kersource="";
    std::ifstream myfile ("Mohit.cl");
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
 
    // Host input vectors
    int *h_a;
    
 
    // Device input buffers
    cl::Buffer d_a;

 
    // Size, in bytes, of each vector
    size_t bytes = n*sizeof (int);
 
    // Allocate memory for each vector on host
    h_a = new int[n];


    cl::STRING_CLASS buildlog;
    cl::Program program_;
    std::vector<cl::Device> devices;

    cl_int err = CL_SUCCESS;
    try {
 
        // Query platforms
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);
        if (platforms.size() == 0) {
            std::cout << "Platform size 0\n";
            return -1;
        }
        

 
        // Get list of devices on default platform and create context
        cl_context_properties properties[] =
           { CL_CONTEXT_PLATFORM, (cl_context_properties)(platforms[0])(), 0};
        cl::Context context(CL_DEVICE_TYPE_CPU, properties);
         devices = context.getInfo<CL_CONTEXT_DEVICES>();


 
        // Create command queue for first device
        cl::CommandQueue queue(context, devices[0], 0, &err);
 
        // Create device memory buffers
        d_a = cl::Buffer(context, CL_MEM_WRITE_ONLY, bytes);
 
        // Bind memory buffers
 
 
        //Build kernel from source string
        cl::Program::Sources source(1,
            std::make_pair(kernelSource,strlen(kernelSource)));
        program_ = cl::Program(context, source);
        program_.build(devices);

        
        

        std::cout<<"BuildLog: \n"<<buildlog;
        

        // Create kernel object
        cl::Kernel kernel(program_, "multiMat", &err);
 
        // Bind kernel arguments to kernel
        kernel.setArg(0, d_a);
        kernel.setArg(1,n);
  
 
        // Number of work items in each local work group
        cl::NDRange localSize(64);
        // Number of total work items - localSize must be devisor
        cl::NDRange globalSize(1024);
 
        // Enqueue kernel
        cl::Event event;
        queue.enqueueNDRangeKernel(
            kernel,
            cl::NullRange,
            globalSize,
            localSize,
            NULL,
            &event);
 
        // Block until kernel completion
        event.wait();
 
        // Read back d_c
        queue.enqueueReadBuffer(d_a, CL_TRUE, 0, bytes, h_a);
        }
    catch (cl::Error err) 
    {
        std::cerr
        << "ERROR: "<<err.what()<<"("<<err.err()<<")"<<std::endl;

        buildlog = program_.getBuildInfo<CL_PROGRAM_BUILD_LOG>(devices[0], NULL);
        std::ofstream logfile ("MohitLog.txt");
        logfile<<buildlog;
        logfile.close();
        
    }
    
 
    for(int i=0;i<n;i++)
    {
        std::cout<<h_a[i]<<" ";
    }

    delete(h_a);

    return 0;
}