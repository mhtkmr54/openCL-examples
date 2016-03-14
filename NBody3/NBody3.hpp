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


unsigned int n = 1024;
    cl_float2 *pi;
    cl_float2 *pf;
    cl_float2 *v;
   size_t bytes = n*sizeof(cl_float2);
