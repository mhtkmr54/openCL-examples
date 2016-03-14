//#pragma OPENCL EXTENSION cl_khr_fp64 : enable
__kernel void multiMat(  __global float *a,
                       __global float *b,           
                       __global float *c,
                       const unsigned int n,
                       __local float *bwrk)
{
                                      
  int id1 = get_global_id(0);
  int iloc = get_local_id(0);
  int nloc =get_local_size(0);
    
  if(id1<n )
  {
      
    //c[id1*n+id2] = a[id1*n+id2] + b[id1*n+id2];
    float temp=0.0f;
    int j=0;
    int k=0;
    const int size = (int)(ceil(n/(float)64)*64);
    float awrk[1024];
    for(k=0;k<n;k++)
    awrk[k] = a[id1*n+k];
    for(j=0;j<n;j++)
    {
      
      for(k=iloc;k<n;k+=nloc)
      {
        bwrk[k] = b[k*n + j];
      }
      barrier(CLK_LOCAL_MEM_FENCE);
      temp = 0.0f;
      for(k=0;k<n;k++)
      {
        temp += awrk[k] * bwrk[k];
      }
      c[id1*n+j] = temp;
      barrier(CLK_LOCAL_MEM_FENCE);
      
    }
  }
}