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


unsigned int n = 256;
    cl_float ttau = 0.9f;
    cl_float4 *h_if1234;
    cl_float4 *h_if5678;
    cl_float  *h_if0;
    cl_float4 *h_of1234;
    cl_float4 *h_of5678;
    cl_float  *h_of0;
    cl_float  *h_rho;
    const cl_float weight[9] = {4.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 9.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0, 1.0 / 36.0};

    cl::Buffer d_if1234;
    cl::Buffer d_if5678;
    cl::Buffer   d_if0;
    cl::Buffer d_of1234;
    cl::Buffer d_of5678;
    cl::Buffer   d_of0;
    cl::Buffer   d_rho;
    cl::Buffer d_weight;

    std::vector<cl::Platform> platforms;
    
    cl::Context context;
    size_t bytes;
    std::vector<cl::Device> devices;
    cl::Kernel kernel;
    
    cl::Program program_;
    cl::Event event;
