#include <cstdio>
#include <cuda_runtime_api.h>

__host__ __device__ void NextHello() {
    printf("Universal\n");
}

__global__ void Hello() {
    printf("Hello, GPU! tid=%d%d bid=%d%d!\n", threadIdx.x, threadIdx.y, blockIdx.x, blockIdx.y);
    if (threadIdx.x == 3) {
        NextHello();
    }
}

int main() {
    //cudaSetDevice(0); /* or CUDA_VISIBLE_DEVICES=0 environment variables
	dim3 gridDim{2, 2};
	dim3 threadDim{2, 2};
    Hello<<<gridDim, threadDim>>>();

    cudaDeviceSynchronize();
    printf("Hello, CPU!\n");
    NextHello();

    return 0;
}
