__constant sampler_t imageSampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_CLAMP | CLK_FILTER_LINEAR; 
//#pragma OPENCL EXTENSION cl_khr_fp64 : enable
__kernel void nBody(  __global float4 *pi,
                       __global float4 *pf,           
                       __global float4 *v,
                       const unsigned int n,
                       __local float4 *ploc)
                      // __write_only image2d_t img)
{
  int id = get_global_id(0);
  int lid = get_local_id(0);
  int ng = get_global_size(0);
  int nl = get_local_size(0);
  int nb = ng/nl;
  if(id<n)
  {

    float dt =0.1;
    float4 d;
    float r;
    float4 p = pi[id];
    float4 a = (float4)(0,0,0,0);
    for(int k=0; k <nb; k++)
    {
      ploc[lid] = pi[k*nl + lid];
      barrier(CLK_LOCAL_MEM_FENCE);
      for(int i =0; i<nl  ; i++)
      {
       d = ploc[i] -p;
        if(i != id)
        {
          r = d.x*d.x + d.y*d.y + d.z*d.z + 0.001;
          r = rsqrt(r);
        }
        else 
        r=0;
        a += d*r*r*r;

      }
    }
    pf[id] = p + v[id]*dt + 0.5*a*dt*dt;
    v[id] += a*dt;
    int2 coord = (int2)(pf[id].x*60, pf[id].y*60  );
    uint4 color = (0,0,100,0);
    //write_imageui(img, coord, color );
  }
}