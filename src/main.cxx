#include <stdio.h>
#include <chrono>
#include <cstdlib>
#include <vector>
#include <numeric>

#ifndef CHECK
#define CHECK 0
#endif

#include <Eigen/Dense>
typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> FloatMatrix;

typedef std::chrono::high_resolution_clock::time_point TimePoint;
double duration(TimePoint t1, TimePoint t2) {
    // convert to seconds
    auto s = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();
    return static_cast<double>(s);
}

extern "C" {
    void sgemm(const float* pa, const float* pb, float* pc, int m, int n, int k);
}

FloatMatrix mm_sol(const FloatMatrix& ma, const FloatMatrix& mb, std::vector<double> &tt) {
    float* pa = (float*) ma.data();
    float* pb = (float*) mb.data();

    int mm = ma.rows();
    int ll = ma.cols();
    int kk = mb.cols();

    auto mc = FloatMatrix(mm, kk);
    mc.setZero();
    float* pc = mc.data();

    assert(ma.rows() == mm && ma.cols() == ll); // A.shape == (mm, ll)
    assert(mb.rows() == ll && mb.cols() == kk); // B.shape == (ll, kk)
    assert(mc.rows() == mm && mc.cols() == kk); // C.shape == (mm, kk)

    auto t1 = std::chrono::high_resolution_clock::now();
    sgemm(pc, pa, pb, mm, ll, kk);
    auto t2 = std::chrono::high_resolution_clock::now();

    double dt = duration(t1, t2);
    tt.push_back(dt);

    return mc;
}

FloatMatrix mm_ref(const FloatMatrix &ma, const FloatMatrix &mb, std::vector<double> &tt)
{
    FloatMatrix mc(ma.rows(), mb.cols());
    mc.setZero(); // optional, since we specify beta=0 anyway
    
    auto t1 = std::chrono::high_resolution_clock::now();
    mc += ma * mb;
    auto t2 = std::chrono::high_resolution_clock::now();

    double dt = duration(t1, t2);
    tt.push_back(dt);

    return mc;
}

std::vector<double> gflops(const std::vector<double> &tt, const int s) {
    std::vector<double> gg;
    for (auto t : tt) {
        gg.push_back(2.0 * s * s * s / t / 1e9);
    }
    return gg;
}

double average(std::vector<double> &xx) {
    double x0 = 0.0;
    for (auto x : xx) {
        x0 += x;
    }
    return x0 / xx.size();
}

double deviation(std::vector<double> &xx) {
    assert(xx.size() > 1);
    int d = xx.size() - 1;

    double x0 = 0.0;
    for (auto x : xx) {
        x0 += x;
    }
    x0 = x0 / xx.size();

    double n = 0.0;
    for (auto x : xx) {
        n += (x - x0) * (x - x0);
    }
    return std::sqrt(n / d);
}

int main(int argc, char* argv[]) {
    std::vector<double> tt0;
    std::vector<double> tt1;

    int L = std::stoi(argv[1]);
    int N = std::stoi(argv[2]);
    printf("L = %d, N = %d\n", L, N);

    for (int i = 0; i < N; i++) {
        FloatMatrix A;
        A.resize(L, L);
        A.setRandom();

        FloatMatrix B;
        B.resize(L, L);
        B.setRandom();

        auto C1 = mm_sol(A, B, tt1);

        if (CHECK) {
            auto C0 = mm_ref(A, B, tt0);
            auto err = (C0 - C1).array().abs().maxCoeff();
            assert(err < 1e-3);
        }
    }

    if (CHECK) {
        std::sort(tt0.begin(), tt0.end());
        tt0.erase(tt0.begin());
        tt0.erase(tt0.end() - 1);

        auto gg0 = gflops(tt0, L);
        printf("MM_REF t = %6.2e +/- %6.2e, GFLOPS = %6.2f +/- %6.2f\n", average(tt0), deviation(tt0), average(gg0), deviation(gg0));
    }

    std::sort(tt1.begin(), tt1.end());
    tt1.erase(tt1.begin());
    tt1.erase(tt1.end() - 1);

    auto gg1 = gflops(tt1, L);
    printf("MM_SOL t = %6.2e +/- %6.2e, GFLOPS = %6.2f +/- %6.2f\n", average(tt1), deviation(tt1), average(gg1), deviation(gg1));

    return 0;
}
