#include <stdio.h>
#include <chrono>
#include <cstdlib>
#include <vector>
#include <numeric>

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
    float* pa = ma.data();
    float* pb = mb.data();
    float* pc = mc.data();

    int m = A.rows();
    int k = B.cols();
    int l = A.cols();

    auto mc = FloatMatrix(m, n);
    mc.setZero();

    assert(ma.rows() == m && ma.cols() == l);
    assert(mb.rows() == l && mb.cols() == k);
    assert(mc.rows() == m && mc.cols() == k);

    auto t1 = std::chrono::high_resolution_clock::now();
    sgemm(pc, pa, pb, m, k, l);
    auto t2 = std::chrono::high_resolution_clock::now();

    double dt = duration(t1, t2);
    tt.push_back(dt);

    return C;
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

    double x0 = 0.0;
    for (auto x : xx) {
        x0 += x;
    }
    x0 = x0 / xx.size();

    double d = 0.0;
    for (auto x : xx) {
        d += (x - x0) * (x - x0);
    }
    return std::sqrt(d / (xx.size() - 1));
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

        #ifdef CHECK
            auto C0 = mm_ref(A, B, tt0);
            auto err = (C0 - C1).array().abs().maxCoeff();
            assert(err < 1e-3);
        #endif
        
        auto C1 = mm_sol(A, B, tt1);
    }

    #ifdef CHECK
        std::sort(tt0.begin(), tt0.end());
        tt0.erase(tt0.begin());
        tt0.erase(tt0.end() - 1);

        auto gg0 = gflops(tt0, L);
        printf("MM_REF t = %6.2e +/- %6.2e, GFLOPS = %6.2f +/- %6.2f\n", average(tt0), deviation(tt0), average(gg0), deviation(gg0));
    #endif

    std::sort(tt1.begin(), tt1.end());
    tt1.erase(tt1.begin());
    tt1.erase(tt1.end() - 1);

    auto gg1 = gflops(tt1, L);
    printf("MM_SOL t = %6.2e +/- %6.2e, GFLOPS = %6.2f +/- %6.2f\n", average(tt1), deviation(tt1), average(gg1), deviation(gg1));

    return 0;
}
