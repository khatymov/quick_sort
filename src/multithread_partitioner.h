//
// Created by Renat_Khatymov on 11/24/2024.
//

#pragma once

#include "pch.h"
/*
 * MultithreadPartitioner is a class that partition data(span) using std::partition algorithm
 * in multiple threads and has counter of partitioned elements.
 * It has a few methods: 1. partitionSpan - divide a span into two by pivot element
 *                          + put second span to a queue
 *                       2. get a second span from a queue
 *                       3. get pivot counter to identify how many elements are sorted
 *
 */
template <typename T>
class MultithreadPartitioner {
    MultithreadPartitioner(const MultithreadPartitioner&) = delete;
    MultithreadPartitioner(MultithreadPartitioner&&) = delete;
    MultithreadPartitioner& operator=(const MultithreadPartitioner&) = delete;
    MultithreadPartitioner& operator=(MultithreadPartitioner&&) = delete;
public:
    // ctr. Create threads and put partion function
    explicit MultithreadPartitioner(int threadsNum = (std::thread::hardware_concurrency() - 1));
    // dctr. Join threads
    ~MultithreadPartitioner();

    // partition in available thread
    // the idea is to partition left part of a span until size will be 0/1
    // meanwhile right part will be pushed to a queue to be handled in other threads
    void pushDataToPartition(std::span<T> data);

    // get a span from queue - a span that was divided in pushDataToPartition method
    std::span<T> getPartitionedSpan();

    // get number of elements that are sorted. Used as a sign that an element has found its place
    // aka how many times std::partition was called.
    size_t getNumSortedElements() const;
protected:
    void worker();
    // partition span
    std::span<T>::iterator getPivotIterator(std::span<T> data);
private:
    atomic_bool m_stop = true;

    std::vector<std::thread> m_threads;
    // mutex for spans in a queue m_inSpansForProcessing
    std::mutex m_mtxSpanIn;
    // mutex for spans in a queue m_OutSpansProcessed
    std::mutex m_mtxSpanOut;
    // condition_variable for notification that data arrived to a queue
    // and we are good to process them
    std::condition_variable m_CVSpanArrived;

    std::queue<std::span<T>> m_inSpansForProcessing;
    std::queue<std::span<T>> m_OutSpansProcessed;

    struct alignas(hardware_constructive_interference_size) OneCacheLiner // occupies one cache line
    {
        std::atomic_uint64_t counter = 0;
    } m_pivot;
};

template <typename T>
MultithreadPartitioner<T>::MultithreadPartitioner(int threadsNum) {
    if (threadsNum < 2) {
        throw std::invalid_argument("Wrong value of threads number");
    }

    cout << "Number of threads: " << threadsNum << endl;

    m_stop = false;
    for (int i = 0; i < threadsNum; ++i) {
        m_threads.emplace_back(std::thread(&MultithreadPartitioner::worker, this));
    }

    m_CVSpanArrived.notify_all();
}

template <typename T>
MultithreadPartitioner<T>::~MultithreadPartitioner() {
    m_stop = true;
    m_CVSpanArrived.notify_all();
    for (auto &thread: m_threads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

template <typename T>
void MultithreadPartitioner<T>::pushDataToPartition(std::span<T> data) {
    std::unique_lock<std::mutex> uniqueLock(m_mtxSpanIn);
    m_inSpansForProcessing.push(std::move(data));
    uniqueLock.unlock();
    m_CVSpanArrived.notify_one();
}


template <typename T>
span<T> MultithreadPartitioner<T>::getPartitionedSpan()
{
    std::lock_guard<std::mutex> lockGuard(m_mtxSpanOut);
    if (m_OutSpansProcessed.empty()) {
        return span<T>();
    }
    auto res = m_OutSpansProcessed.front();
    m_OutSpansProcessed.pop();
    return res;
}

template <typename T>
size_t MultithreadPartitioner<T>::getNumSortedElements() const
{
    return m_pivot.counter;
}

template <typename T>
std::span<T>::iterator MultithreadPartitioner<T>::getPivotIterator(std::span<T> data)
{
    auto middle = data.begin() + ((data.end() - data.begin()) / 2);
    std::iter_swap(middle, data.end() - 1);
    // Partition around the pivot
    middle = std::partition(data.begin(), data.end() - 1, [last = data.end()](const T& value) { return value < *(last - 1); });
    // Place the pivot in its correct sorted position
    std::iter_swap(middle, data.end() - 1);

    return middle;
}

/*
 * The main purpose of this routine is to sort a span.
 * For sorting we use quick sort technique (partitioning).
 * If a span after partitioning is a quite big - give a smaller part to the another thread for sorting.
 * The remaining part sorts in the current thread.
 */

// size of span that's worth to send for sorting to another thread
//32 kb
constexpr size_t THRESHOLD_SIZE_TO_SORT = 32768*1; //8388608

#include "simple_sort.h"
#include "my_sort.h"

template <typename T>
void MultithreadPartitioner<T>::worker() {
    while (true) {
        std::unique_lock<std::mutex> locker(m_mtxSpanIn);
        m_CVSpanArrived.wait(locker, [&] { return !m_inSpansForProcessing.empty() or m_stop;});

        if (m_stop) {
            return;
        }

        auto data = m_inSpansForProcessing.front();
        m_inSpansForProcessing.pop();
        locker.unlock();

        int partitionCounter = 0;
        // got a span.
        while (data.size() > THRESHOLD_SIZE_TO_SORT) {
            const auto pivot = getPivotIterator(data);
            ++partitionCounter;
//            std::span<T> leftSpan = {data.begin(), pivot};
//            std::span<T> rightSpan = {pivot + 1, data.end()};

            const auto leftSize = pivot - data.begin();
            const auto rightSize = data.end() - (pivot + 1);
            if (leftSize > rightSize)
            {
                {
                    std::lock_guard<std::mutex> lockGuard(m_mtxSpanOut);
                    m_OutSpansProcessed.push(std::span<T>({data.begin(), pivot}));
                }
                data = std::span<T>({pivot + 1, data.end()});
            } else {
                {
                    std::lock_guard<std::mutex> lockGuard(m_mtxSpanOut);
                    m_OutSpansProcessed.push(std::span<T>({pivot + 1, data.end()}));
                }
                data = std::span<T>({data.begin(), pivot});
            }
        }

        partitionCounter += data.size();

        SimpleQSort<T>::sort(data);

        m_pivot.counter.fetch_add(partitionCounter);
    }
}

