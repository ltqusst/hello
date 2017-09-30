//a simpler one
__kernel void v1(const int M, const int N,
                      			const __global uchar* A,
                      			__global uchar* Buf)
{
    // Thread identifiers
    const int x = get_global_id(0); // Row ID of C (0..M)
    const int y = get_global_id(1); // Col ID of C (0..N)
	int R,G,B;
    int tmp1 = A[y*M + x];
    int tmp2 = A[M*N + (y>>1)*M + (x & (~1))];
    int tmp3 = A[M*N + (y>>1)*M + (x | 1)];
 	
 	tmp1 -= 16;
 	tmp2 -= 128;
 	tmp3 -= 128;
 	
 	R = ( 298 * tmp1 + 516 * tmp2 + 128) >> 8;
 	G = ( 298 * tmp1 - 100 * tmp2 - 208 * tmp3 + 128) >> 8;
 	B = ( 298 * tmp1  + 409 * tmp3 + 128) >> 8;
 
    Buf[(y*M + x)*3 + 0] = clamp(R,0,255);
    Buf[(y*M + x)*3 + 1] = clamp(G,0,255);
    Buf[(y*M + x)*3 + 2] = clamp(B,0,255);
}


//a simpler one
__kernel void v2(const int M, const int N,
                      			const __global uchar* A,
                      			__global uchar* Buf)
{
    // Thread identifiers
    const int x = get_global_id(0); // Row ID of C (0..M)
    const int y = get_global_id(1); // Col ID of C (0..N)
    
	const __global uchar* uv = &(A[M*N + (y>>1)*M + (x | 1)]);
	
    int yValue = A[y*M + x];
    int uValue = uv[-1]-128;
    int vValue = uv[0]-128;
    
    float rTmp = yValue + (1.370705f * (vValue));
    float gTmp = yValue - (0.698001f * (vValue)) - (0.337633f * (uValue));
    float bTmp = yValue + (1.732446f * (uValue));
 
    Buf[(y*M + x)*3 + 0] = clamp(rTmp,0.0f,255.0f);
    Buf[(y*M + x)*3 + 1] = clamp(gTmp,0.0f,255.0f);
    Buf[(y*M + x)*3 + 2] = clamp(bTmp,0.0f,255.0f);
}

