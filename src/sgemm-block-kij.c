#include <assert.h>

#ifndef BLOCK_SIZE
#define BLOCK_SIZE 64
#endif

#define min(x, y) (((x) < (y)) ? (x) : (y))

void sgemm(const float* __restrict pa, const float* __restrict pb, float* __restrict pc, int m, int n, int l) {
    for (int k = 0; k < l; k += BLOCK_SIZE) {
        int ll = min(BLOCK_SIZE, l - k);
        for (int i = 0; i < m; i += BLOCK_SIZE) {
            int mm = min(BLOCK_SIZE, m - i);
            for (int j = 0; j < n; j += BLOCK_SIZE) {
                int nn = min(BLOCK_SIZE, n - j);

                assert(mm == BLOCK_SIZE);
                assert(nn == BLOCK_SIZE);
                assert(ll == BLOCK_SIZE);

                for (int kk = 0; kk < ll; kk++) {
                    for (int ii = 0; ii < mm; ii++) {
                        double aik = pa[l * (i + ii) + k + kk];
                        for (int jj = 0; jj < nn; jj++) {
                            double bkj = pb[n * (k + kk) + j + jj];
                            pc[(i + ii) * n + (j + jj)] += aik * bkj;
                        }
                    }
                }
            }
        }
    }
}
