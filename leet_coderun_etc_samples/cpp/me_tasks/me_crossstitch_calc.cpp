#include <cmath>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include "me_crossstitch_calc.h"
#include "../utils/stat_utils.h"

/**
 * The little cat loves fish, birds, and enjoying walks in the garden,
 * as well as embroidering. That’s why she is embroidering three pictures
 * at once: one with fish, one with birds, and one with a garden.
 * Every time she finishes embroidering one square of a picture,
 * she gets tired and goes to sleep. When she wakes up, she randomly
 * selects one of the three pictures and embroiders another square.
 * It is known that she still needs to embroider n squares on the fish picture,
 * m squares on the bird picture, and k squares on the garden picture.
 * What is the probability for each of these pictures will be completed first?
 */

namespace crossstitch {

using namespace stat_utils;

/**
 * Constructor for SimpleCrossStitch.
 *
 * Initializes the problem with the number of remaining stitches for each of the three
 * embroidery pictures: fish (n), birds (m), and garden (k).
 *
 * The maximum number of steps before any picture can possibly finish is (n + m + k - 2),
 * because the earliest a picture can complete is when the other two still have at least
 * one stitch left.
 */
SimpleCrossStitch::SimpleCrossStitch(const int n, const int m, const int k)
    : nCount(n)
    , mCount(m)
    , kCount(k)
    , maxCount(n + m + k - 2)
    {};

/**
 * Calculate the number of combinations for one step of embroidering.
 */
long SimpleCrossStitch::CalculateOneStepCombinations(const int work_range, const int n, const int other_n1, const int other_n2) {
    int additional = work_range - n + 1;
    long result = 0;
    for (int k = 0; k < std::min(additional, other_n1-1) + 1; k++) {
        for (int m = additional-k; m < std::min(additional-k, other_n2-1) + 1; m++) {
            result += CombinationsKN(work_range, k) * CombinationsKN(work_range-k, m);
        }
    }

    return result;
}

/**
 * Calculate the total number of combinations for a given picture to be completed first.
 */
long SimpleCrossStitch::CalculateOneSampleCombinations(const int n, const int other_n1, const int other_n2) {
    long resultSum = 0;
    for (int i = n-1; i < maxCount; i++) {
        long mult = pow(3, maxCount - i - 1);
        resultSum += CalculateOneStepCombinations(i, n, other_n1, other_n2) * mult;
    }

    return resultSum;
};

/**
 * Calculate the probabilities of each picture being completed first.
 *
 * Returns a vector containing the probabilities for the fish, bird, and garden pictures, respectively.
 */
std::vector<double> SimpleCrossStitch::Calc() {
    long totalCombinations = TotalCombinationsN(maxCount);
    double nResultSum = (double)CalculateOneSampleCombinations(nCount, mCount, kCount) / (double)totalCombinations;
    double mResultSum = (double)CalculateOneSampleCombinations(mCount, nCount, kCount) / (double)totalCombinations;
    double kResultSum = (double)CalculateOneSampleCombinations(kCount, nCount, mCount) / (double)totalCombinations;

    std::vector<double> result = {nResultSum, mResultSum, kResultSum};
    return result;
};
} // namespace crossstitch
