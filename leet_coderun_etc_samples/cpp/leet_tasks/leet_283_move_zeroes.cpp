#include "leet_283_move_zeroes.h"

/**
 * LeetCode 283: Move Zeroes
 *
 * Given an integer array nums, move all 0's to the end of it
 * while maintaining the relative order of the non-zero elements.
 *
 * Approach: Two-pointer technique (in-place)
 * - Count zeros and shift non-zero elements to the left
 * - Fill the remaining positions with zeros
 *
 * Time Complexity: O(n) - single pass through the array
 * Space Complexity: O(1) - in-place modification
 */
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
