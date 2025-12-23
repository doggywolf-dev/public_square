"""
Solution for LeetCode Problem 239: Move Zeroes.

Given an integer array nums, move all 0's to the end of it while maintaining the relative order of the non-zero elements.

Note that you must do this in-place without making a copy of the array.
"""

import sys
from sample_decorators.timer import timer
from sample_decorators.tracer import tracer

class Solution_leet_283:
    @timer
    @tracer
    def moveZeroes(self, nums: list[int]) -> None:
        """
        Moves all zero elements to the end of the list while maintaining
        the relative order of non-zero elements.
        Modifies the list in-place.

        Time complexity: O(n)
        """
        N = len(nums)
        zeroes = 0
        for i in range(N):
            if nums[i] == 0:
                # Increment zero counter
                zeroes += 1
            else:
                # Shift non-zero element left by current zero count
                nums[i-zeroes] = nums[i]

            i += 1

        # Second pass: fill remaining positions at the end with zeros
        for j in range(N-zeroes, N):
            nums[j] = 0

def main():
    data = list(map(int, input().split()))
    solution = Solution_leet_283()
    solution.moveZeroes(data)
    print(data)

if __name__ == "__main__":
    main()