//
// Created by rene on 02/12/24.
//

#pragma once

#include <algorithm>
#include <iterator>
#include <cmath> // for std::log2

#include "sorter_interface.h"

template <typename RandomIt>
void insertionSort(RandomIt first, RandomIt last) {
    for (auto i = first + 1; i < last; ++i) {
        auto key = *i;
        auto j = i;
        while (j > first && *(j - 1) > key) {
            *j = *(j - 1);
            --j;
        }
        *j = key;
    }
}

template <typename RandomIt>
RandomIt partition(RandomIt first, RandomIt last) {
    auto pivot = *(last - 1); // Use the last element as the pivot
    auto i = first - 1;

    for (auto j = first; j < last - 1; ++j) {
        if (*j <= pivot) {
            ++i;
            std::iter_swap(i, j);
        }
    }

    std::iter_swap(i + 1, last - 1); // Place pivot in the correct position
    return i + 1;
}

template <typename RandomIt>
void quickSort(RandomIt first, RandomIt last, int depthLimit) {
    while (std::distance(first, last) > 16) {
        if (depthLimit == 0) {
            std::make_heap(first, last); // Switch to heapsort
            std::sort_heap(first, last);
            return;
        }

        auto pivot = partition(first, last);

        // Tail call elimination: Sort the smaller partition first
        if (std::distance(first, pivot) < std::distance(pivot + 1, last)) {
            quickSort(first, pivot, depthLimit - 1);
            first = pivot + 1; // Continue sorting the larger partition
        } else {
            quickSort(pivot + 1, last, depthLimit - 1);
            last = pivot;
        }
    }

    insertionSort(first, last); // Use insertion sort for small partitions
}


template <typename RandomIt>
void mySort(RandomIt first, RandomIt last) {
    if (first == last) return;

    int depthLimit = 2 * std::log2(std::distance(first, last));
    quickSort(first, last, depthLimit);
}

template<typename T>
class MySorter: public ISorter<T> {
public:
    MySorter() = default;
    void sort(vector<T>& data) override;
};
template <typename T>
void MySorter<T>::sort(vector<T>& data) {
    mySort(data.begin(), data.end());
}

//template<typename T>
//class StandartQSort: public ISorter<T>{
//    StandartQSort(const StandartQSort&) = delete;
//    StandartQSort(StandartQSort&&) = delete;
//    StandartQSort& operator=(const StandartQSort&) = delete;
//    StandartQSort& operator=(StandartQSort&&) = delete;
//public:
//    StandartQSort() = default;
//
//    void sort(std::vector<T>& data) override;
//};
//
//template<typename T>
//void StandartQSort<T>::sort(std::vector<T>& data) {
//    std::sort(data.begin(), data.end());
//}

