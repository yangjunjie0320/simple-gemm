// #include <assert.h>

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 64
#endif
#define min(x, y) (((x) < (y)) ? (x) : (y))

// A.shape = (M, L), B.shape = (L, N), C.shape = (M, N)
// thus for the block: (with numpy notation)
// C[i:i+mm, j:j+nn] += A[i:i+mm, k:k+ll] @ B[k:k+ll, j:j+nn]
// which is equivalent to:
// for ii in range(mm):
//     for jj in range(nn):
//         for kk in range(ll):
//             C[i+ii, j+jj] += A[i+ii, k+kk] * B[k+kk, j+jj]

// Note:
// A[i+ii, k+kk] = pa + (i + ii) * L + (k + kk)
// B[k+kk, j+jj] = pb + (k + kk) * N + (j + jj)
// C[i+ii, j+jj] = pc + (i + ii) * N + (j + jj)

// void sgemm_(float* paa, float* pbb, float* pcc, int m, int n, int l, int mm, int nn, int ll) {
//     for (int ii = 0; ii < mm; ii++) {
//         for (int kk = 0; kk < ll; kk++) {
//             float aik = paa[l * ii + kk];
//             for (int jj = 0; jj < nn; jj++) {
//                 float bkj = pbb[n * kk + jj];
//                 pcc[ii * n + jj] += aik * bkj;
//             }
//         }
//     }
// }

static inline void sgemm_unroll(const float* paa, const float* pbb, float* pcc, int m, int n, int l) {
    for (int ii = 0; ii < BLOCK_SIZE; ii++) {
        for (int kk = 0; kk < BLOCK_SIZE; kk++) {
            float aik = paa[ii * l + kk];
            for (int jj = 0; jj < BLOCK_SIZE; jj++) {
                float bkj = pbb[kk * n + jj];
                pcc[ii * n + jj] += aik * bkj;
            }
        }
    }
}

void sgemm(const float* __restrict pa, const float* __restrict pb, float* __restrict pc, int m, int n, int l) {
    for (int i = 0; i < m; i += BLOCK_SIZE) {
        int mm = min(BLOCK_SIZE, m - i);
        for (int k = 0; k < l; k += BLOCK_SIZE) {
            int ll = min(BLOCK_SIZE, l - k);
            for (int j = 0; j < n; j += BLOCK_SIZE) {
                int nn = min(BLOCK_SIZE, n - j);

                // for (int ii = 0; ii < mm; ii++) {
                //     for (int kk = 0; kk < ll; kk++) {
                //         float aik = pa[l * (i + ii) + k + kk];
                //         for (int jj = 0; jj < nn; jj++) {
                //             float bkj = pb[n * (k + kk) + j + jj];
                //             pc[(i + ii) * n + (j + jj)] += aik * bkj;
                //         }
                //     }
                // }

                // assert(mm == BLOCK_SIZE);
                // assert(nn == BLOCK_SIZE);
                // assert(ll == BLOCK_SIZE);

                sgemm_unroll(
                    pa + i * l + k, 
                    pb + k * n + j, 
                    pc + i * n + j, 
                    m, n, l // , mm, nn, ll
                );
            }
        }
    }
}