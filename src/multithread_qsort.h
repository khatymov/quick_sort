//
// Created by Renat_Khatymov on 11/21/2024.
//

#pragma once

#include "sorter_interface.h"
#include "thread_pool.h"
#include "thread_safe_queue.h"

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
 * This routine sorts data using quick sort algorithm.
 * Using Thread Pool and Thread Safe Queue and partitioning algorithm we sort data in a fast way.
 */


// Task: need to sort it without copying
// Need to answer to a question - how to store results
// I think that the whole idea is that we can't sort more than number of threads.
// So we need to create a buffer with let's say 6 data in it.
// if we send 1 span we get 2 spans
// 3 spans -> 6 AND WE REACHED OUR LIMIT
// Buffer should be thread safe
// Initially allocate memory for 6x2 = 12 spans
// 2 queues. First in, second - out Max size = 6 per each
// At the first iteration inQueue set data span
// Output queue will get 2 spans
// Need to change queues places. Now first will be out, second will be in
// Second iteration.
// IN: 2
// OUT: 4
// switch
// Third iteration
// Since 4*2 = 8 > 6 we can't send all, just first 6. Remaning 2



/*
 * This function sort input data using quick sort (partitioning) algorithm in multiple threads.
 * The initial idea that after partitioning we get 2 subarrays(I use spans) divided by pivot element.
 * Based on that fact we can send 2 spans(subarray) to another threads, so they can be sorted independently.
 */
template <typename T>
void MultithreadQSort<T>::nonRecursiveQuickSort(std::vector<T>& data) {

    if (data.empty() or data.size() == 1) {
        return;
    }

    // Create thread safe queue that can
    //  - push data into it;
    //  - get size of the queue. we need it will be used in condition that thread pool is finished its work; should not block
    //  - get data from queue;

    // create shared ptr of thread safe queue(TSQ) where we can store results of executed functor
    std::shared_ptr<ThreadSafeQueue<T>> threadSafeQueue = std::make_shared<ThreadSafeQueue<T>>();

    std::weak_ptr<ThreadSafeQueue<T>> weakThreadSafeQueue(threadSafeQueue);

    // create a functor that accepts span and a shared pointer of TSQ
    //  functor should:
    //      - split a span into two spans
    //      - push spans into TSQ
    MyThreadPool<T> myThreadPool(7);
    myThreadPool.start();
    // create a stack for spans and push initial data
    std::stack<std::span<T>> spans;
    spans.push(data);
    // create a while loop where finished condition will be counter of spans aka data size.
    // When counter lower than elements in data - keep going :)
    while (not spans.empty()) {
        //  initially expectedSpansNumber is equal to zero. Increment this value only when span goes to ThreadPool
        int expectedSpansNumber = 0;
        //  To have concurrency feature we need to put all available spans into thread pool to be sorted
        //  While stack in not empty push a functor with a span from stack and pointer to TSQ
        while (not spans.empty()) {
            auto curSpan = spans.top();
            spans.pop();
            //if current span is empty or equal to 1 - means everything is sorted.
            if (curSpan.empty() or curSpan.size() == 1) {
                continue;
            }
            // increment expectedSpansNumber -> further we expected double of those elements
            ++expectedSpansNumber;
            // push functor to thread pool
            myThreadPool.push([curSpan = std::move(curSpan), weakThreadSafeQueue]() mutable {
                // Select pivot as the last element
                T pivotLomuto = curSpan[curSpan.size() - 1];

                // Partition around the pivot
                auto pivot = std::partition(curSpan.begin(), curSpan.end() - 1,
                                            [&](const T& value) { return value < pivotLomuto; });

                // Place the pivot in its correct sorted position
                std::iter_swap(pivot, curSpan.end() - 1);

                auto queuePtr = weakThreadSafeQueue.lock();
                if (queuePtr) {
                    queuePtr->push({curSpan.begin(), pivot});
                    queuePtr->push({pivot + 1, curSpan.end()});
                }
            });
        }

        if (expectedSpansNumber == 0) {
            continue;
        }
        // multiply expectedSpansNumber to two - because every span will be split in functor into 2
        expectedSpansNumber = expectedSpansNumber * 2;
        // The condition that thread pool finished its work is when queue has a size of expectedSpansNumber
        // if size is less we need to give control to other threads back so just call yield and continue
        while (threadSafeQueue->getSize() != expectedSpansNumber) {
            this_thread::yield();
            continue;
        }

        // if size of queue is met our expected size then pop element from queue and push it to stack
        for (int i = 0; i < expectedSpansNumber; ++i) {
            spans.push(threadSafeQueue->get());
        }
    }

    // stop thread pool, everything is sorted
    myThreadPool.stop();
}
