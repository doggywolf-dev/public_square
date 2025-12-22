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
        num = deque()
        deind = deque()
        for i in range(N):
            while len(deind) > 0 and deind[0] <= i - k:
                num.popleft()
                deind.popleft()
            while len(num) > 0 and num[-1] < nums[i]:
                num.pop()
                deind.pop()

            num.append(nums[i])
            deind.append(i)
            if i >= k-1:
                result.append(num[0])

        return result

def main():
    K = int(input())
    data = list(map(int, input().split()))
    solution = Solution_leet_239()
    max_list = solution.maxSlidingWindow(data, k)
    print(data)

if __name__ == "__main__":
    main()