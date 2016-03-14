//#pragma OPENCL EXTENSION cl_khr_fp64 : enable                    
__kernel void letItFlow(  __global float *a,                                                                     
                       const unsigned int n)                    
{                                                               
    //Get our global thread ID                                  
    int id1 = get_global_id(0);        
    int id2 = get_global_id(1);                          
                                                                
    //Make sure we do not go out of bounds                      
    if (a[id1*n+id2-1]==1 || a[id1*n+id2+1]==1 || a[id1*(n-1)+id2]==1 || a[id1*(n+1)+id2]==1 )      
    {                      
        a[id1*n+id2] = 1 ;        
    }              
    else
    a[id1*n+id2] = 0;          
}                                                               
                                                                