void sgemm(float* pc, float* pa, float* pb, int mm, int ll, int kk) {
    for (int m0 = 0; m0 < mm; m0++) {
        for (int k0 = 0; k0 < kk; k0++) {
            // float cmk = 0.0;
            for (int l0 = 0; l0 < ll; l0++) {
                float aml = pa[m0 * ll + l0];
                float blk = pb[l0 * kk + k0];
                pc[m0 * kk + k0] += aml * blk;
            }
        }
    }
}
