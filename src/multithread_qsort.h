//
// Created by Renat_Khatymov on 11/21/2024.
//

#pragma once

#include "sorter_interface.h"
#include "multithread_partitioner.h"

template<typename T>
class MultithreadQSort: public ISorter<T> {

    MultithreadQSort(const MultithreadQSort&) = delete;
    MultithreadQSort(MultithreadQSort&&) = delete;
    MultithreadQSort& operator=(const MultithreadQSort&) = delete;
    MultithreadQSort& operator=(MultithreadQSort&&) = delete;

public:
    MultithreadQSort() = default;

    void sort(std::vector<T>& data) override;
protected:
    void nonRecursiveQuickSort(std::vector<T>& data);
};


template <typename T>
void MultithreadQSort<T>::sort(std::vector<T>& data)
{
    nonRecursiveQuickSort(data);
}


/*
 * This function sort input data using quick sort (partitioning) algorithm in multiple threads.
 * The initial idea that after partitioning we get 2 subarrays(I use spans) divided by pivot element.
 * One span should be partitioning further in a thread, the other should go to the queue and wait until available thread will sort it.
 */

// MulthithreadSorter sort span
// get span
template <typename T>
void MultithreadQSort<T>::nonRecursiveQuickSort(std::vector<T>& data) {

    if (data.empty() or data.size() == 1) {
        return;
    }

    // need to call partition for non intersection span in multiple threads
    // initialize MultithreadPartitioner
    // loop until size partitioned elements of instance MultithreadPartitioner is equal
    // to size of inital array
    // push span to partitioner
    // get processed span (right part) from partitioner
    std::span<T> currentSpan = data;

    MultithreadPartitioner<T> partitioner;
    while (partitioner.getNumSortedElements() <= (data.size() - 1)) {
        if (not currentSpan.empty())
        {
            partitioner.pushDataToPartition(currentSpan);
        }
        // get new span (right side)
        currentSpan = partitioner.getPartitionedSpan();
    }
}
