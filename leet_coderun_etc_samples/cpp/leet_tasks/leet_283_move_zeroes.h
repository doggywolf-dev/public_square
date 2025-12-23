#include <iostream>
#include <vector>

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
class Solution_leet_283 {
public:
    void moveZeroes(std::vector<int>& nums);
};