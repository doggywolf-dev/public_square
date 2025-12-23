#pragma once

#include <vector>

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

/**
 * A class to calculate the probability of each picture being completed first.
 */
class SimpleCrossStitch {
public:
    /**
     * Constructor for SimpleCrossStitch.
     *
     * @param n Number of squares remaining to embroider on the fish picture.
     * @param m Number of squares remaining to embroider on the bird picture.
     * @param k Number of squares remaining to embroider on the garden picture.
     */
    explicit SimpleCrossStitch(const int n, const int m, const int k);

    /**
     * Calculate the probabilities of each picture being completed first.
     *
     * @return A vector containing the probabilities for the fish, bird, and garden pictures, respectively.
     */
    std::vector<double> Calc();

protected:
    /**
     * Calculate the total number of combinations for a given picture to be completed first.
     *
     * @param n The number of remaining squares for the picture being considered.
     * @param other_n1 The number of remaining squares for the first other picture.
     * @param other_n2 The number of remaining squares for the second other picture.
     * @return The total number of combinations for the picture to be completed first.
     */
    long CalculateOneSampleCombinations(const int n, const int other_n1, const int other_n2);

    /**
     * Calculate the number of combinations for one step of embroidering.
     *
     * @param workRange The current step range.
     * @param n The number of remaining squares for the picture being considered.
     * @param other_n1 The number of remaining squares for the first other picture.
     * @param other_n2 The number of remaining squares for the second other picture.
     * @return The number of combinations for one step.
     */
    long CalculateOneStepCombinations(const int workRange, const int n, const int other_n1, const int other_n2);

private:
    int nCount = 0; ///< Number of squares remaining for the fish picture.
    int mCount = 0; ///< Number of squares remaining for the bird picture.
    int kCount = 0; ///< Number of squares remaining for the garden picture.
    int maxCount = 0; ///< Maximum number of steps until one picture is complete.
};

} // namespace crossstitch