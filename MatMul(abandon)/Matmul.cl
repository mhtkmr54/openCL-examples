//#pragma OPENCL EXTENSION cl_khr_fp64 : enable                    
__kernel void multiMat(  __global float *a,                       
                       __global float *b,                       
                       __global float *c,                       
                       const unsigned int n)                    
{                                                               
    //Get our global thread ID                                  
    int id1 = get_global_id(0);        
         if(id1<n*n)
    c[id1]=a[id1]+b[id1];
}                                                               
                                                                