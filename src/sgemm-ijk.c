// void sgemm(float* pa, float* pb, float* pc, int m, int n, int l) {
//     for (int i = 0; i < m; i++) {
//         for (int j = 0; j < n; j++) {
//             for (int k = 0; k < l; k++) {
//                 pc[i * n + j] += pa[i * l + k] * pb[k * n + j];
//             }
//         }
//     }
// }

void sgemm(float* pc, float* pa, float* pb, int m, int k, int n) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            float cij = 0;
            for (int k = 0; k < l; k++) {
                cij += pa[i * l + k] * pb[k * n + j];
            }
            pc[i * n + j] += cij;
        }
    }
}