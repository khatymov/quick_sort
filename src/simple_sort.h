//
// Created by Renat_Khatymov on 11/21/2024.
//

#pragma once

#include "sorter_interface.h"

template<typename T>
class SimpleQSort: public ISorter<T>{
    SimpleQSort(const SimpleQSort&) = delete;
    SimpleQSort(SimpleQSort&&) = delete;
    SimpleQSort& operator=(const SimpleQSort&) = delete;
    SimpleQSort& operator=(SimpleQSort&&) = delete;
public:
    SimpleQSort() = default;

    void sort(std::vector<T>& data) override;

protected:
    void nonRecursiveSort(std::vector<T>& data);
    std::pair<std::span<T>,std::span<T>> splitByPivot(span<T> data);
};

template <typename T>
void SimpleQSort<T>::sort(std::vector<T>& data)
{
    nonRecursiveSort(data);
}

template <typename T>
void SimpleQSort<T>::nonRecursiveSort(std::vector<T>& data)
{
    // span = data;
    // stack<std::span<T>> stackIn;
    // while(not stackIn.empty())
    //  curInterval = stackIn.top();
    //  stackIn.pop();
    //  if (interval.begin() == interval.end()--)
    //      continue;
    //  auto pivotLomuto = interval.end()--;
    //  auto it = std::partition(curInterval.begin(), curInterval.end(), [&pivotLomuto](int num){return std::less<T>(*pivotLomuto)});
    //  stackOut.push({curInterval.begin(), it});
    //  stackOut.push({it, curInterval.end()});

    // >> 'it' has its fixed place, and won't change

    stack<std::span<T>> spanStack;
    spanStack.push(data);
    while(not spanStack.empty()) {
        auto curInterval = spanStack.top();
        spanStack.pop();

        if (curInterval.empty() or curInterval.size() == 1) {
            continue;
        }

        const auto splitSpans = splitByPivot(curInterval);

        spanStack.push(splitSpans.first);
        spanStack.push(splitSpans.second);
    }
}


template <typename T>
std::pair<std::span<T>,std::span<T>>  SimpleQSort<T>::splitByPivot(span<T> data)
{
    // Select pivot as the last element
    T pivotLomuto = data[data.size() - 1];

    // Partition around the pivot
    typename std::span<T>::iterator pivot = std::partition(data.begin(), data.end() - 1,
                                                           [&](const T& value) { return value < pivotLomuto; });

    // Place the pivot in its correct sorted position
    std::iter_swap(pivot, data.end() - 1);

    return {{data.begin(), pivot}, {pivot + 1, data.end()}};
}



