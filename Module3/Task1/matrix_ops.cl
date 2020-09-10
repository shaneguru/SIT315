
__kernel void multiply_matrices(const int M, const int N, const int K,
                      const __global int* A,
                      const __global int* B,
                      __global int* C)
{
  const int globalRow = get_global_id(0); // Row ID of C (0..M)
  const int globalCol = get_global_id(1); // Col ID of C (0..N)
  if (globalRow < M && globalCol < M) {
    int i, sum = 0;
    for (i = 0 ; i < M ; ++i) {
      sum += A[globalCol*M+i] * B[globalRow+i*M];
    }
    C[globalRow+globalCol*M] = sum;
  }
}




/*
    //printf("(%d,%d)\n ", globalRow, globalCol);
    // Compute a single element (loop over K)
    int acc = 0.0f;
    for (int k=0; k<K; k++) {
        acc += B[k*M + globalRow] * A[globalCol*K + k];
     //   printf("(%d,%d), values = (%d, %d)\n ", k*M + globalRow, globalCol*K + k, A[k*M + globalRow] , B[globalCol*K + k]);
    }
*/