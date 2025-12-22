import sys
from sample_decorators.timer import timer
from sample_decorators.tracer import tracer

class Solution_leet_283:
    @timer
    @tracer
    def moveZeroes(self, nums: list[int]) -> None:
        """
        Do not return anything, modify nums in-place instead.
        """
        N = len(nums)
        zeroes = 0
        for i in range(N):
            if nums[i] == 0:
                zeroes += 1
            else:
                nums[i-zeroes] = nums[i]

            i += 1

        for j in range(N-zeroes, N):
            nums[j] = 0

def main():
    data = list(map(int, input().split()))
    solution = Solution_leet_283()
    solution.moveZeroes(data)
    print(data)

if __name__ == "__main__":
    main()