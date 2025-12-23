"""
The little cat loves fish, birds, and enjoying walks in the garden, as well as embroidering.
That’s why she is embroidering three pictures at once: one with fish, one with birds, and one with a garden.
Every time she finishes embroidering one square of a picture, she gets tired and goes to sleep.
When she wakes up, she randomly selects one of the three pictures and embroiders another square.

It is known that she still needs to embroider:
- n squares on the fish picture,
- m squares on the bird picture,
- k squares on the garden picture.

The task is to compute the probability that each of these pictures will be completed first.
"""

import sys

def combinations(n: int, k: int) -> int:
    """
    Calculates the number of ways to choose k items from n items as the binomial coefficient C(n, k) = n! / (k! * (n-k)!).
    Uses an optimized iterative approach to avoid factorial overflow.
    """
    if k > n:
        return 0

    k = n - k if k > (n - k) else k
    res = 1
    for i in range (k):
        res = res * (n-i) / (i+1)

    return res


class OneSchemeProbability:
    """
    A class to compute the probability that a specific embroidery pattern (e.g., fish)
    will be completed before the other two (birds and garden).

    The model assumes the cat randomly picks one of the three projects each time she works,
    and we want the probability that one project reaches zero remaining squares first.
    """
    def __init__(self, self_N: float, other1_N: float, other2_N: float):
        """
        Initialize the probability calculator for one target project.

        Args:
            self_N (float): Remaining squares for the target project (to be completed first).
            other1_N (float): Remaining squares for the first competing project.
            other2_N (float): Remaining squares for the second competing project.
        """
        self.N = self_N
        self.other1_N = other1_N
        self.other2_N = other2_N
        self.max_N = self_N + other1_N + other2_N - 2

    def total_combinations(self) -> float:
        """
        Calculates the total number of possible embroidery sequences of length max_N.
        Each step has 3 choices (which picture to work on), so total = 3^max_N.
        """
        return pow(3, self.max_N)

    def calculate_one_step_combinations(self, work_range: int) -> float:
        """
        Calculates the number of valid sequences of length 'work_range' where:
        1. Our target picture is completed exactly at step work_range+1
        2. Competitor pictures are not completed before our target picture
        """
        additional = work_range - self.N + 1
        result = 0.0
        for k in range(0, min(additional, self.other1_N-1) + 1):
            for m in range(additional-k, min(additional-k, self.other2_N-1) + 1):
                # Count sequences: choose positions for k steps of competitor 1,
                # then from remaining positions choose m steps for competitor 2
                result += combinations(work_range, k) * combinations(work_range-k, m)

        return result

    def сalc(self) -> float:
        """
        Compute the probability that the target project will be finished first.

        This sums over all possible completion points (from N-1 to max_N),
        weighting each by the probability of that path.

        Returns:
            float: Probability that this project finishes first.
        """
        result_sum = 0
        # Consider all possible lengths of work sequences before completion
        for i in range(self.N - 1, self.max_N):
            # For sequences of length i, we have (max_N - i - 1) remaining steps
            # Each remaining step has 3 choices, so multiplier = 3^(remaining_steps)
            mult = pow(3.0, self.max_N - i - 1)
            # Add probability contribution for sequences of this length
            result_sum += self.calculate_one_step_combinations(i) * mult

        # Normalize by total number of possible sequences
        return result_sum / self.total_combinations()

def main():
    """
    Main function to read input, calculate probabilities for all three pictures,
    and display the results.
    """
    print("Input n, m and k (the remaining squares for fish picture, bird picture and garden picture):")
    N_fishes, N_birds, N_garden = list(map(int, input().split()))
    max_N = N_fishes + N_garden + N_birds - 2

    #fishes
    fishes = OneSchemeProbability(N_fishes, N_birds, N_garden)
    print(f"Fishes will be finished first with probability = {fishes.сalc()}.")

    #birds
    birds = OneSchemeProbability(N_birds, N_fishes, N_garden)
    print(f"Birds will be finished first with probability = {birds.сalc()}.")

    #garden
    garden = OneSchemeProbability(N_garden, N_birds, N_fishes)
    print(f"Garden will be finished first with probability = {garden.сalc()}.")


if __name__ == "__main__":
    main()