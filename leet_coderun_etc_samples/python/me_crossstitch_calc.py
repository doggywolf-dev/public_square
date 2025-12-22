import sys

'''
    The little cat loves fish, birds, and enjoying walks in the garden, as well as embroidering. That’s why she is embroidering three pictures at once: one with fish, one with birds, and one with a garden. Every time she finishes embroidering one square of a picture, she gets tired and goes to sleep. When she wakes up, she randomly selects one of the three pictures and embroiders another square. It is known that she still needs to embroider n squares on the fish picture, m squares on the bird picture, and k squares on the garden picture. What is the probability for each of these pictures will be completed first?
'''

def combinations(n: int, k: int) -> int:
    if k > n:
        return 0

    k = n - k if k > (n - k) else k
    res = 1
    for i in range (k):
        res = res * (n-i) / (i+1)

    return res


class OneSchemeProbability:
    def __init__(self, self_N: float, other1_N: float, other2_N: float):
        self.N = self_N
        self.other1_N = other1_N
        self.other2_N = other2_N
        self.max_N = self_N + other1_N + other2_N - 2

    def total_combinations(self) -> float:
        return pow(3, self.max_N)

    def calculate_one_step_combinations(self, work_range: int) -> float:
        additional = work_range - self.N + 1
        result = 0.0
        for k in range(0, min(additional, self.other1_N-1) + 1):
            for l in range(additional-k, min(additional-k, self.other2_N-1) + 1):
                result += combinations(work_range, k) * combinations(work_range-k, l)

        return result

    def сalc(self) -> float:
        result_sum = 0
        for i in range(self.N - 1, self.max_N):
            mult = pow(3.0, self.max_N - i - 1)
            result_sum += self.calculate_one_step_combinations(i) * mult

        return result_sum / self.total_combinations()

def main():
    print("Input n, m and k (the remaining squares for fish picture, bird picture and garden picture):")
    N_fishes, N_birds, N_garden = list(map(int, input().split()))
    max_N = N_fishes + N_garden + N_birds - 2

    #fishes
    fishes = OneSchemeProbability(N_fishes, N_birds, N_garden)
    print(f"Fishes will be finished first with probability = {fishes.сalc()}.")

    #road
    birds = OneSchemeProbability(N_birds, N_fishes, N_garden)
    print(f"Birds will be finished first with probability = {birds.сalc()}.")

    #planet
    garden = OneSchemeProbability(N_garden, N_birds, N_fishes)
    print(f"Garden will be finished first with probability = {garden.сalc()}.")


if __name__ == "__main__":
    main()