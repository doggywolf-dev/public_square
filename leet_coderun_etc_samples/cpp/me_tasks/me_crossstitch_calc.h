#pragma once

#include <vector>

/*
    The little cat loves fish, birds, and enjoying walks in the garden, as well as embroidering. That’s why she is embroidering three pictures at once: one with fish, one with birds, and one with a garden. Every time she finishes embroidering one square of a picture, she gets tired and goes to sleep. When she wakes up, she randomly selects one of the three pictures and embroiders another square. It is known that she still needs to embroider n squares on the fish picture, m squares on the bird picture, and k squares on the garden picture. What is the probability for each of these pictures will be completed first?
*/
namespace crossstitch {

class SimpleCrossStitch {
public:
    explicit SimpleCrossStitch(const int n, const int m, const int k);

    std::vector<double> Calc();

protected:
    long CalculateOneSampleCombinations(const int n, const int other_n1, const int other_n2);
    long CalculateOneStepCombinations(const int workRange, const int n, const int other_n1, const int other_n2);

private:
    int nCount = 0;
    int mCount = 0;
    int kCount = 0;
    int maxCount = 0;
};

} // namespace crossstitch