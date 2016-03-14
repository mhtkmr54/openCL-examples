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

 
// OpenCL kernel. Each work item takes care of one element of c
const char *kernelSource =                                      "\n" \
"//#pragma OPENCL EXTENSION cl_khr_fp64 : enable                    \n" \
"__kernel void vecAdd(  __global float4 *a,                       \n" \
"                       __global float4 *b,                       \n" \
"                       __global float4 *c,                       \n" \
"                       const unsigned int n)                    \n" \
"{                                                               \n" \
"    //Get our global thread ID                                  \n" \
"    int id = get_global_id(0);                                  \n" \
"                                                                \n" \
"    //Make sure we do not go out of bounds                      \n" \
"    if (id < n)                                                 \n" \
"        c[id][0] = a[id][0] + b[id][0];                         \n" \
"}                                                               \n" \
                                                                "\n" ;
 
 
int main(int argc, char *argv[]){
 
    // Length of vectors
    unsigned int n = 1000;
 
    // Host input vectors
    
    cl_float4 *f4a;
    f4a= new cl_float4[n];
    cl_float4 *f4b;
    f4b= new cl_float4[n];
    cl_float4 *f4c;
    f4c= new cl_float4[n];
    
    
 
    // Device input buffers
    cl::Buffer d_a;
    cl::Buffer d_b;
    // Device output buffer
    cl::Buffer d_c;
 
    // Size, in bytes, of each vector
    size_t bytes = n*sizeof (cl_float4);
 
    // Initialize vectors on host
    for(int i = 0; i < n; i++ )
    {
        f4a[i][0] = sinf(i)*sinf(i);
        f4b[i][0] = cosf(i)*cosf(i);
        f4a[i][1] = 1;
        f4b[i][1] = 2;
        f4a[i][2] = 3;
        f4b[i][2] = 4;
        f4a[i][3] = 5;
        f4b[i][3] = 6;
    }
 
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
        cl::Context context(CL_DEVICE_TYPE_GPU, properties);
        std::vector<cl::Device> devices = context.getInfo<CL_CONTEXT_DEVICES>();
 
        // Create command queue for first device
        cl::CommandQueue queue(context, devices[0], 0, &err);
 
        // Create device memory buffers
        d_a = cl::Buffer(context, CL_MEM_READ_ONLY, bytes);
        d_b = cl::Buffer(context, CL_MEM_READ_ONLY, bytes);
        d_c = cl::Buffer(context, CL_MEM_WRITE_ONLY, bytes);
 
        // Bind memory buffers
        queue.enqueueWriteBuffer(d_a, CL_TRUE, 0, bytes, h_a);
        queue.enqueueWriteBuffer(d_b, CL_TRUE, 0, bytes, h_b);
 
        //Build kernel from source string
        cl::Program::Sources source(1,
            std::make_pair(kernelSource,strlen(kernelSource)));
        cl::Program program_ = cl::Program(context, source);
        program_.build(devices);
 
        // Create kernel object
        cl::Kernel kernel(program_, "vecAdd", &err);
 
        // Bind kernel arguments to kernel
        kernel.setArg(0, d_a);
        kernel.setArg(1, d_b);
        kernel.setArg(2, d_c);
        kernel.setArg(3, n);
 
        // Number of work items in each local work group
        cl::NDRange localSize(64);
        // Number of total work items - localSize must be devisor
        cl::NDRange globalSize((int)(ceil(n/(float)64)*64));
 
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
        queue.enqueueReadBuffer(d_c, CL_TRUE, 0, bytes, h_c);
        }
    catch (cl::Error err) {
         std::cerr
            << "ERROR: "<<err.what()<<"("<<err.err()<<")"<<std::endl;
    }
 
    // Sum up vector c and print result divided by n, this should equal 1 within error
    float sum = 0;
    for(int i=0; i<n; i++)
        sum += h_c[i];
    std::cout<<"final result: "<<sum<<std::endl;
 
    // Release host memory
    delete(h_a);
    delete(h_b);
    delete(h_c);
 
    return 0;
}