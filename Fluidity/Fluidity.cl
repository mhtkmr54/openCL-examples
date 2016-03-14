
//#pragma OPENCL EXTENSION cl_khr_fp64 : enable


float getfeq(float2 u,float ceX, float ceY, float rhoK,float weight)
{
  float eiu = u.x*ceX + u.y*ceY;
  return weight*rhoK*(1 - 1.5*(u.x*u.x + u.y*u.y) + 3*eiu + 4.5*eiu*eiu);
}
__kernel void fluidify(  __global float4 *if1234,
                       __global float4 *if5678,
                       __global float *if0,           
                       __global float4 *of1234,
                       __global float4 *of5678,
                       __global float *of0,
                       __global float *rho,
                       const float ttau,
                       __global float *weight,
                       const unsigned int n)
{
  int id1 = get_global_id(0);
  int id2 = get_global_id(1);
  float4 f1234 = if1234[id1*n+id2];
  float4 f5678 = if5678[id1*n+id2];
  float f0    = if0[id1*n+id2];
  if(id1<n && id2<n)
  {
    float rhoK = f1234.x + f1234.y + f1234.z + f1234.w + f5678.x + f5678.y + f5678.z + f5678.w + f0;
    float2 u;
    u.x = f1234.x - f1234.z + f5678.x + f5678.w - f5678.y - f5678.z;
    u.y = f1234.y - f1234.w + f5678.x - f5678.w + f5678.y - f5678.z;
    u.x /= rhoK;
    u.y /= rhoK;

    float4 feq1234;
    float4 feq5678;
    float feq0 = getfeq(u,0, 0, rhoK, weight[0]);
    feq1234.x = getfeq(u, 1, 0, rhoK, weight[1]);
    feq1234.y = getfeq(u, 0, 1, rhoK, weight[2]);
    feq1234.z = getfeq(u,-1, 0, rhoK, weight[3]);
    feq1234.w = getfeq(u, 0,-1, rhoK, weight[4]);
    feq5678.x = getfeq(u, 1, 1, rhoK, weight[5]);
    feq5678.y = getfeq(u,-1, 1, rhoK, weight[6]);
    feq5678.z = getfeq(u,-1,-1, rhoK, weight[7]);
    feq5678.w = getfeq(u, 1, -1, rhoK, weight[8]);

    f1234 = (1-ttau)*f1234 + ttau*feq1234;
    f5678 = (1-ttau)*f5678 + ttau*feq5678;
    f0    = (1-ttau)*f0    + ttau*feq0;   
   
    of1234[id1*n+id2].z = f1234.x;  
    of1234[id1*n+id2].w = f1234.y; 
    of1234[id1*n+id2].x = f1234.z;    
    of1234[id1*n+id2].y = f1234.w;   
    of5678[id1*n + id2 + 1 - n].x  =f5678.x;
    of5678[id1*n + id2 - 1 - n].y  =f5678.y;
    of5678[id1*n + id2 - 1 + n].z  =f5678.z;
    of5678[id1*n + id2 + 1 + n].w  =f5678.w;
    of0[id1*n + id2] = f0;
    rho[id1*n + id2] = rhoK;

  }
}