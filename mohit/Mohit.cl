//#pragma OPENCL EXTENSION cl_khr_fp64 : enable
__kernel void multiMat(  __global float *a,
                       const unsigned int n)
{
                                      
  int id1 = get_global_id(0);


    
  if(id1<n)
  {
      a[id1]= id1;
    
      
  }
  
}