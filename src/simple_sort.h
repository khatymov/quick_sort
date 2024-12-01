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

    static void sort(std::span<T> data);
protected:

    // partition span
    static inline std::span<T>::iterator getPivotIterator(std::span<T> data);
};

template <typename T>
span<T>::iterator SimpleQSort<T>::getPivotIterator(span<T> data)
{
    auto middle = data.begin() + ((data.end() - data.begin()) / 2);
    std::iter_swap(middle, data.end() - 1);
    // Partition around the pivot
    middle = std::partition(data.begin(), data.end() - 1, [last = data.end()](const T& value) { return value < *(last - 1); });
    // Place the pivot in its correct sorted position
    std::iter_swap(middle, data.end() - 1);

    return middle;
}


template <typename T>
void SimpleQSort<T>::sort(span<T> data) {
    stack<std::span<T>> spanStack;
    spanStack.push(data);
    while(not spanStack.empty()) {
        auto curInterval = spanStack.top();
        spanStack.pop();

        if (curInterval.empty() or curInterval.size() == 1) {
            continue;
        }

        if (curInterval.size() == 2) {
            if (curInterval[0] > curInterval[1]) {
                std::swap(curInterval[0], curInterval[1]);
            }
            continue;
        }

        const auto pivot = getPivotIterator(curInterval);

        std::span<T> left({curInterval.begin(), pivot});
        std::span<T> right({pivot + 1, curInterval.end()});

        if (left.size() > 1) {
            spanStack.push(left);
        }

        if (right.size() > 1) {
            spanStack.push(right);
        }
    }
}

#include "my_sort.h"

template <typename T>
void SimpleQSort<T>::sort(std::vector<T>& data)
{
    SimpleQSort<T>::sort(span<T>(data));
}
