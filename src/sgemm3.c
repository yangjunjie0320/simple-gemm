#define UNROLL 4

// this is a simple implementation of sgemm, which is a matrix multiplication function
// it is used to multiply two matrices A and B, and store the result in matrix C
// C += A dot B
// A is M x K
// B is K x N
// C is M x N
void simple_sgemm(float* pa, float* pb, float* pc, int m, int n, int k) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            float cij = 0.0;

            for (int l = 0; l < k; l += UNROLL) {
                for (int u = 0; u < UNROLL; u++) {
                    float aik = pa[i * k + l + u];
                    float bkj = pb[(l + u) * n + j];
                    cij += aik * bkj;
                }
            }
            pc[i * n + j] += cij;
        }
    }
}
