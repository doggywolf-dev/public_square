#include <leet_283_move_zeroes.h>
#include "me_tasks/me_crossstitch_calc.h"
#include <iostream>
#include <vector>

int main(int, char**) {
    // Check leet_283 solution
    std::cout << "Lets run leet_283 solution.\n";

    std::cout << "Input the array size:\n";
    int N;
    std::cin >> N;

    std::cout << "and the array with zeroes:\n";
    std::vector<int> data;
    int value;
    for (int i = 0; (i < N) && (std::cin >> value); ++i) {
        data.push_back(value);
    }

    Solution_leet_283 solution;
    solution.moveZeroes(data);
    std::cout << "Resulting array is:\n";
    for (int x: data) {
        std::cout << x << " ";
    }
    std::cout << std::endl;

    // Check kitten stitch solution
    std::cout << "Lets run the kitten stitch task. Input three squares counts:\n";
    int nCount;
    int mCount;
    int kCount;
    std::cin >> nCount >> mCount >> kCount;

    crossstitch::SimpleCrossStitch* cross = new crossstitch::SimpleCrossStitch(nCount, mCount, kCount);
    std::vector<double> firstStitch = cross->Calc();
    std::cout << "Result probabilities are: " << firstStitch[0] << ", " << firstStitch[1] << ", " << firstStitch[2] << std::endl;
    delete cross;
}
