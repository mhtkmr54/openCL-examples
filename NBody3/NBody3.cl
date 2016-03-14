//#pragma OPENCL EXTENSION cl_khr_fp64 : enable
__kernel void nBody(  __global float2 *pi,
                       __global float2 *pf,           
                       __global float2 *v,
                       const unsigned int n)
{
  int id = get_global_id(0);
  if(id<n)
  {

    float dt =1;
    float2 d;
    float r;
    float2 p = pi[id];
    float2 a = (float2)(0,0);
    for(int i=0; i <n; i++)
    {
        d = pi[i] - p;
        if(i != id)
        {
          r = d.x*d.x + d.y*d.y + 1;
          r = rsqrt(r);
        }
        else 
        r=0;
        a -= (((id+i)%2)-1)*(d*r*r*r);
    }
    pf[id] = p + v[id]*dt + 0.5*a*dt*dt;
    v[id] += a*dt;
  }
}