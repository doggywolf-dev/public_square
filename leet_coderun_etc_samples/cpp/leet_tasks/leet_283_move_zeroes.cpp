#include "leet_283_move_zeroes.h"

void Solution_leet_283::moveZeroes (std::vector<int>& nums) {
    int N = nums.size();
    std::cout << N << std::endl;
    int zeros = 0;
    for (int i = 0; i < N; i++) {
        if (nums[i] == 0) {
            zeros ++;
        } else {
            nums[i-zeros] = nums[i];
        }
    }

    for (int j = N-zeros; j < N; j++) {
        nums[j] = 0;
    }
};
