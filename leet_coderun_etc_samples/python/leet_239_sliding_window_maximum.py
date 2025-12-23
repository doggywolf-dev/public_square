'''
Solution for LeetCode Problem 239: Sliding Window Maximum.

You are given an array of integers nums, there is a sliding window of size k which is moving from the very left of the array to the very right. You can only see the k numbers in the window. Each time the sliding window moves right by one position.
Return the max sliding window.

The solution uses a monotonic decreasing deque to track indices of potential maximums.
Time complexity: O(n).
'''

import sys
from collections import deque
from sample_decorators.timer import timer
from sample_decorators.tracer import tracer

class Solution_leet_239:
    @timer
    @tracer
    def maxSlidingWindow(self, nums: list[int], k: int) -> list[int]:
        N = len(nums)
        result = []
        num = deque() # Stores the actual values
        deind = deque() # Stores the corresponding indices
        for i in range(N):
            # Remove elements that are outside the current window
            # The leftmost valid index for current window is (i - k + 1)
            while len(deind) > 0 and deind[0] <= i - k:
                num.popleft()
                deind.popleft()

            # Maintain monotonic decreasing property
            # Remove all elements smaller than the current element from the back
            # They can never be the maximum while the current element is in the window
            while len(num) > 0 and num[-1] < nums[i]:
                num.pop()
                deind.pop()

            # Add the current element to the deque
            num.append(nums[i])
            deind.append(i)

            # Record the maximum for the current window
            if i >= k-1:
                result.append(num[0])

        return result

def main():
    '''
    Main function to read input and execute the sliding window maximum algorithm.

    Input format:
        - First line: integer k (window size)
        - Second line: space-separated integers (the array)
    '''
    K = int(input())
    data = list(map(int, input().split()))
    solution = Solution_leet_239()
    max_list = solution.maxSlidingWindow(data, k)
    print(data)

if __name__ == "__main__":
    main()