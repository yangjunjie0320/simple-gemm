void sgemm(float* pa, float* pb, float* pc, int m, int n, int l) {
    for (int j = 0; j < n; j++) {
        for (int k = 0; k < l; k++) {
            for (int i = 0; i < m; i++) {
                pc[i * n + j] += pa[i * l + k] * pb[k * n + j];
            }
        }
    }
} 