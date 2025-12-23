#include <cmath>
#include <cstdint>
#include <stdexcept>
#include "stat_utils.h"

namespace stat_utils {
/**
 * Calculates the binomial coefficient C(n, k) - the number
 * of combinations "n choose k" (C(n,k) = n! / (k! * (n-k)!))
 *
 * @param n The total number of elements in the set
 * @param k The number of elements to choose
 * @return The binomial coefficient C(n, k), or 0 if k > n
 *
 * @note Uses an optimized iterative approach that minimizes the risk of overflow
 *       by performing division as soon as possible after multiplication
 */
std::uint64_t CombinationsKN(const std::uint64_t n, const std::uint64_t k) {
    if (k > n) {
        return 0; // Invalid: can't choose more items than available
    }

    // Use symmetry property: C(n,k) = C(n, n-k) → compute with smaller k
    const std::uint64_t effective_k = (k > n - k) ? n - k : k;
    std::uint64_t res = 1;

    // Calculate the binomial coefficient using the formula:
    // C(n, k) = n! / (k! * (n-k)!)
    // Implemented as: (n * (n-1) * ... * (n-k+1)) / (k * (k-1) * ... * 1)
    for (std::uint64_t i = 1; i <= effective_k; i++) {
        res = res * (n - i + 1) / i;
    }

    return res;
};

/**
 * Calculates the total number of possible combinations when each of n elements
 * can be in one of 3 states (e.g., selected, not selected, or partially selected).
 *
 * @param n The number of elements
 * @return 3^n - the total number of possible combinations
 *
 * @throws std::overflow_error if n > 40, as the result would exceed 64-bit capacity
 *
 * @note The limit of 40 is because 3^40 is close to the maximum value for uint64_t
 */
std::uint64_t TotalCombinationsN(const std::uint64_t n) {
    if (n > 40) {
        throw std::overflow_error("n is too large for a 64-bit result.");
    }

    return std::pow(3, n);
};

} // namespace stat_utils
