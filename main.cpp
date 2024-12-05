/*! \file main.cpp
 * \brief Entry point.
 */

#include <iostream>

using namespace std;

#include "multithread_qsort.h"
#include "timer.h"

int main()
{
    Timer timer;
    std::vector <int> data = {1, 3, 5, 7, -7, -5, -3, -1};
    MultithreadQSort<int> multithreadQSort;
    multithreadQSort.sort(data);
    return 0;
}