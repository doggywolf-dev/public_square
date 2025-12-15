import sys
from time import time

def SpentTime(func):
    def FuncWrapper(*args, **kwargs):
        start_time = time()
        result = func(*args, **kwargs)
        end_time = time()
        print(f"The function has spent {end_time - start_time:.8f} sec.")
        return result
    
    return FuncWrapper
   
def PrintArguments(func):
    def FuncWrapper(*args, **kwargs):
        print(f"Function {func.__name__!r} was called with the following arguments:\n{args[1:]}\n{kwargs}.")
        result = func(*args, **kwargs)
        return result
    
    return FuncWrapper
   
class Solution:
    @PrintArguments
    @SpentTime
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
    solution = Solution()
    solution.moveZeroes(data)
    print(data)

if __name__ == "__main__":
    main()