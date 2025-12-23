#include <cstdio>
#include <cuda_runtime_api.h>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include <string>
#include <stdexcept>

void CheckCudaError(cudaError_t cudaError) {
    if (cudaError != cudaSuccess) {
        throw std::runtime_error(
            std::string("Some cuda code failed: ") +
            cudaGetErrorString(cudaError)
        );
    }
}

int main() {
    static constexpr size_t nRank = 1 << 4;

    int* matrixAHost = reinterpret_cast<int*>(malloc(nRank * nRank * sizeof(int)));
    int* matrixBHost = reinterpret_cast<int*>(malloc(nRank * nRank * sizeof(int)));

    int* aDevice = nullptr;
    CheckCudaError(cudaMalloc(&aDevice, nRank * nRank * sizeof(int)));

    // init
    std::iota(matrixAHost, matrixAHost + nRank*nRank, 0);
    std::iota(matrixBHost, matrixAHost + nRank*nRank, 0);

    CheckCudaError(cudaMemcpy(aDevice, matrixAHost, nRank * nRank * sizeof(int), cudaMemcpyHostToDevice));
    CheckCudaError(cudaMemcpy(matrixBHost, aDevice, nRank * nRank * sizeof(int), cudaMemcpyDeviceToHost));

    cudaDeviceSynchronize();

    for (size_t i = 0; i < nRank * nRank; ++i) {
        int expected = matrixAHost[i];
        int actual = matrixBHost[i];
        if (expected != actual) {
            throw std::runtime_error(
                std::string("Problem at position #") + std::to_string(i) +
                ": expected " + std::to_string(matrixAHost[i]) +
                ", got " + std::to_string(matrixBHost[i])
            );
        }
    }

    cudaFree(aDevice);
    free(matrixBHost);
    free(matrixAHost);
    return 0;
}
