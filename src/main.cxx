#include <stdio.h>
#include <chrono>
#include <cstdlib>
#include <vector>
#include <numeric>
#include <iostream>

#include <Eigen/Dense>
typedef Eigen::Matrix<float, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> FloatMatrix;

#define NN 32

typedef std::chrono::high_resolution_clock::time_point TimePoint;
double duration(TimePoint t1, TimePoint t2) {
    // convert to seconds
    auto s = std::chrono::duration_cast<std::chrono::duration<double>>(t2 - t1).count();
    return static_cast<double>(s);
}

extern "C" {
    void sgemm(const float* pa, const float* pb, float* pc, int m, int n, int k);
}

FloatMatrix mm_sol(const FloatMatrix& A, const FloatMatrix& B, std::vector<double> &tt) {
    auto C = FloatMatrix(A.rows(), B.cols());
    C.setZero();

    auto t1 = std::chrono::high_resolution_clock::now();
    sgemm(A.data(), B.data(), C.data(), A.rows(), B.cols(), A.cols());
    auto t2 = std::chrono::high_resolution_clock::now();
    double dt = duration(t1, t2);
    tt.push_back(dt);
    // printf("MM_SOL t = %6.2e, GFLOPS = %6.2f\n", dt, 2.0 * A.rows() * A.cols() * B.cols() / dt / 1e9);

    return C;
}

#include <mkl_cblas.h>
FloatMatrix mm_ref(const FloatMatrix &A, const FloatMatrix &B, std::vector<double> &tt)
{
    FloatMatrix C(A.rows(), B.cols());
    C.setZero(); // optional, since we specify beta=0 anyway

    int M = static_cast<int>(A.rows());
    int N = static_cast<int>(B.cols());
    int L = static_cast<int>(A.cols());

    float alpha = 1.0f;
    float beta  = 0.0f;
    
    auto t1 = std::chrono::high_resolution_clock::now();
    cblas_sgemm(
        CblasRowMajor, CblasNoTrans, CblasNoTrans, 
        M, N, L, alpha, A.data(), L, B.data(), N, beta, C.data(), N
        );
    auto t2 = std::chrono::high_resolution_clock::now();
    double dt = duration(t1, t2);
    tt.push_back(dt);
    // printf("MM_REF t = %6.2e, GFLOPS = %6.2f\n", dt, 2.0 * A.rows() * A.cols() * B.cols() / dt / 1e9);

    return C;
}

int main(int argc, char* argv[]) {
    // auto t1 = std::chrono::high_resolution_clock::now();
    // auto t2 = std::chrono::high_resolution_clock::now();
    std::vector<double> tt1;
    std::vector<double> tt2;

    int M = std::stoi(argv[1]);
    int N = std::stoi(argv[2]);
    int L = std::stoi(argv[3]);
    printf("M = %d, N = %d, L = %d\n", M, N, L);

    for (int i = 0; i < NN; i++) {
        FloatMatrix A;
        A.resize(M, L);
        A.setRandom();
        FloatMatrix B;
        B.resize(L, N);
        B.setRandom();
        
        auto C_ref = mm_ref(A, B, tt1);
        auto C_sol = mm_sol(A, B, tt2);

        auto err = (C_ref - C_sol).array().abs().maxCoeff();
        assert(err < 1e-3);
    }

    // remove the largest and smallest values
    std::sort(tt1.begin(), tt1.end());
    tt1.erase(tt1.begin());
    tt1.erase(tt1.end() - 1);

    std::sort(tt2.begin(), tt2.end());
    tt2.erase(tt2.begin());
    tt2.erase(tt2.end() - 1);

    // std::cout << "tt1: ";
    // for (auto const &val : tt1) {
    //     std::cout << val << " ";
    // }
    // std::cout << std::endl;

    // std::cout << "tt2: ";
    // for (auto const &val : tt2) {
    //     std::cout << val << " ";
    // }
    // std::cout << std::endl;

    // compute mean and std deviation
    double m1 = std::accumulate(tt1.begin(), tt1.end(), 0.0) / tt1.size();
    double m2 = std::accumulate(tt2.begin(), tt2.end(), 0.0) / tt2.size();
    double s1 = std::sqrt(std::accumulate(tt1.begin(), tt1.end(), 0.0, [m1](double a, double b) { return a + (b - m1) * (b - m1); }) / (tt1.size() - 1));
    double s2 = std::sqrt(std::accumulate(tt2.begin(), tt2.end(), 0.0, [m2](double a, double b) { return a + (b - m2) * (b - m2); }) / (tt2.size() - 1));

    printf("MM_REF t = %6.2e +/- %6.2e, GFLOPS = %6.2f\n", m1, s1, 2.0 * M * L * N / m1 / 1e9);
    printf("MM_SOL t = %6.2e +/- %6.2e, GFLOPS = %6.2f\n", m2, s2, 2.0 * M * L * N / m2 / 1e9);

    return 0;
}
