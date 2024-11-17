#include "gtest/gtest.h"

#include "my_class_test.h"
#include "timer.h"

using namespace std;
using namespace testing;

#include <algorithm>
#include <cstdlib>
#include <future>
#include <map>
#include <queue>
#include <random>
#include <span>
#include <stack>
#include <thread>
#include <vector>

template<typename T>
class MyThreadPool;

//==============================================================================================

template<typename T>
class ISorter {
public:
    virtual void sort(std::vector<T>& data) = 0;
    ~ISorter() = default;
};

//==============================================================================================

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
    void quickSort(std::vector<T>& data, int low, int high);

    std::pair<std::span<T>,std::span<T>> splitByPivot(span<T> data);

    int divideByPivotLomuto(vector<T>& data, int low, int high);
};
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
void SimpleQSort<T>::sort(std::vector<T>& data)
{
//    nonRecursiveSort(data);
    quickSort(data, 0, data.size() - 1);
}

template <typename T>
int SimpleQSort<T>::divideByPivotLomuto(vector<T>& data, int low, int high) {
    // 9, 1, 8, 2, 7, 3, 5
    // 1, 9, 8, 2, 7, 3, 5
    // 1, 2, 8, 9, 7, 3, 5
    // 1, 2, 3, 9, 7, 8, 5
    // 1, 2, 3, 5, 7, 8, 9
    // 5 - pivot
    // 1, 2, 3, 5, 9, 8, 7
    // return index of 5 - 3
    T pivot = data[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; ++j) {
        if (data[j] < pivot) {
            ++i;
            swap(data[i], data[j]);
        }
    }

    swap(data[i+1], data[high]);
    return i + 1;
}

template <typename T>
void SimpleQSort<T>::quickSort(vector<T>& data, int low, int high) {
    nonRecursiveSort(data);
//    if (low < high) {
//        // Lomuto partition scheme
//        int pivotIndex = divideByPivotLomuto(data, low, high);
//        quickSort(data, low, pivotIndex - 1);
//        quickSort(data, pivotIndex + 1, high);
//    }
}

//==============================================================================================

template<typename T>
class MultithreadQSort: public ISorter<T>{
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


template <typename T>
class ThreadSafeQueue {
public:
    void push(std::span<T> data);

    std::span<T> get();

    size_t getSize();
private:
    std::mutex m_mutex;
    std::condition_variable m_condVar;
    std::queue<std::span<T>> m_queue;
};

template <typename T>
size_t ThreadSafeQueue<T>::getSize()
{
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    return m_queue.size();
}

template <typename T>
void ThreadSafeQueue<T>::push(span<T> data) {
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    m_queue.push(data);
//    m_condVar.notify_one();
}

template <typename T>
span<T> ThreadSafeQueue<T>::get()
{
//    std::unique_lock<std::mutex> uniqueLock(m_mutex);
//    m_condVar.wait(uniqueLock, [&]{return !m_queue.empty();});
    auto data = m_queue.front();
    m_queue.pop();
    return data;
}



/*
 * MyThreadPool class accepts jobs aka functors, push it the jobs queue
 * if there is available thread it starts executing a job from a queue
 */
template<typename T>
class MyThreadPool
{
    MyThreadPool(const MyThreadPool&) = delete;
    MyThreadPool(MyThreadPool&&) = delete;
    MyThreadPool& operator=(const MyThreadPool&) = delete;
    MyThreadPool& operator=(MyThreadPool&&) = delete;

public:

    explicit MyThreadPool(const size_t numThreads = 8);

    ~MyThreadPool();

    void start();

    void stop();

    void push(std::function<void()> task);

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_jobQueue;
    std::queue<std::span<T>> m_spanResQueue;
    // at the beginning thread pool sleeps
    std::atomic_bool m_stop = true;
    size_t m_numThreads = 6;

    std::condition_variable m_jobCondVar;
    std::mutex m_jobsMutex;
};


template <typename T>
MyThreadPool<T>::MyThreadPool(const size_t numThreads) {
    if (numThreads < 1) {
        throw std::invalid_argument("Wrong threads number");
    }
    m_numThreads = numThreads;
}

template <typename T>
MyThreadPool<T>::~MyThreadPool() {
    stop();
}

template <typename T>
void MyThreadPool<T>::start() {

    // thread pool is started already
    if (m_stop == false) {
        return;
    }

    auto func = [this]() {
        while (true) {
            std::unique_lock<std::mutex> uniqueLock(m_jobsMutex);
            m_jobCondVar.wait(uniqueLock,[&] { return !m_jobQueue.empty() or m_stop;});
            if (m_stop) {
                break;
            }
            auto executeJob = m_jobQueue.front();
            m_jobQueue.pop();
            uniqueLock.unlock();

            executeJob();
        }
    };

    m_stop = false;
    for (size_t i = 0; i < m_numThreads; ++i) {
        m_threads.emplace_back(std::thread(func));
    }

    m_jobCondVar.notify_all();
}

template <typename T>
void MyThreadPool<T>::push(std::function<void()> task) {
    std::unique_lock<std::mutex> uniqueLock(m_jobsMutex);
    m_jobQueue.push(task);
    uniqueLock.unlock();
    m_jobCondVar.notify_all();
}

template <typename T>
void MyThreadPool<T>::stop() {
    if (not m_stop) {
        m_stop = true;
        m_jobCondVar.notify_all();
        for (auto& curThread:m_threads) {
            curThread.join();
        }
    }
}

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

    // create a functor that accepts span and a shared pointer of TSQ
    //  functor should:
    //      - split a span into two spans
    //      - push spans into TSQ

    // create a threadPool and start it
    MyThreadPool<T> myThreadPool(7);
    myThreadPool.start();
    // create a stack for spans and push initial data
    std::stack<std::span<T>> spanStack;
    spanStack.push(data);
    // used as an indicator that an array is sorted when elementsCounter = data.size()
//    int elementsCounter = 0;
    // create a while loop where finished condition will be counter of spans aka data size.
    // When counter lower than elements in data - keep going :)
    while (not spanStack.empty()) {
        //  initially expectedSpansNumber is equal to zero. Increment this value only when span goes to ThreadPool
        int expectedSpansNumber = 0;
        //  To have concurrency feature we need to put all available spans into thread pool to be sorted
        //  While stack in not empty push a functor with a span from stack and pointer to TSQ
        while (not spanStack.empty()) {
            auto curSpan = spanStack.top();
            spanStack.pop();
            //if current span is empty or equal to 1 - means everything is sorted.
            if (curSpan.empty() or curSpan.size() == 1) {
//                ++elementsCounter;
                continue;
            }
            // increment expectedSpansNumber -> further we expected double of those elements
            ++expectedSpansNumber;
            // push functor to thread pool
            myThreadPool.push([&curSpan, &threadSafeQueue](){
                // Select pivot as the last element
                T pivotLomuto = curSpan[curSpan.size() - 1];

                // Partition around the pivot
                auto pivot = std::partition(curSpan.begin(), curSpan.end() - 1,
                                            [&](const T& value) { return value < pivotLomuto; });

                // Place the pivot in its correct sorted position
                std::iter_swap(pivot, curSpan.end() - 1);

                threadSafeQueue->push({curSpan.begin(), pivot});
                threadSafeQueue->push({pivot + 1, curSpan.end()});
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
            // count pivots
//            ++elementsCounter;
            spanStack.push(threadSafeQueue->get());
        }
    }


    // stop thread pool, everything is sorted
    myThreadPool.stop();
}

//==============================================================================================

template<typename T>
class StandartQSort: public ISorter<T>{
    StandartQSort(const StandartQSort&) = delete;
    StandartQSort(StandartQSort&&) = delete;
    StandartQSort& operator=(const StandartQSort&) = delete;
    StandartQSort& operator=(StandartQSort&&) = delete;
public:
    StandartQSort() = default;

    void sort(std::vector<T>& data) override;
};

template<typename T>
void StandartQSort<T>::sort(std::vector<T>& data) {

    std::qsort(data.data(), data.size(), sizeof(T), [](const void* x, const void* y)
    {
       const auto arg1 = *static_cast<const T*>(x);
       const auto arg2 = *static_cast<const T*>(y);
       const auto cmp = arg1 <=> arg2;
       if (cmp < 0)
           return -1;
       if (cmp > 0)
           return 1;
       return 0;
    });
}

//==============================================================================================

template <typename T>
struct DataSet {
    map<int, vector<T>> data;

    bool operator == (const DataSet<T> other){
        if (data.size() != other.data.size()) {
            return false;
        }
        for (const auto [id, vec]: data) {
            auto otherVec = other.data.at(id);
            if (otherVec != vec) {
                return false;
            }
            int i = 1;
            i++;
        }

        return true;
    }
};

template<typename T>
struct Sorter {
    void sort(DataSet<T> dataSet, std::unique_ptr<ISorter<T>> sorter) {
        for (auto& [id, data]: dataSet.data) {
            Timer timer(id, data.size());
            sorter->sort(data);
        }
        sortedData.push_back(dataSet);
        cout << "\n" << endl;
    }


    bool verifyEqulityOfData() {
        if (sortedData.empty() or sortedData.size() == 1) {
            return true;
        }

        auto primaryData = sortedData[0];
        for (int i = 1; i < sortedData.size(); i++) {
            if (primaryData == sortedData[i]) {
                // Too lazy to rework such logic
            } else {
                return false;
            }
        }

        return true;
    }

    vector<DataSet<T>> sortedData;
};

template<typename T>
vector<T> getRandomVec(size_t size, T minValue, T maxValue) {
    std::random_device rd;
    std::mt19937 gen(rd());  // Mersenne Twister random number engine
    std::uniform_int_distribution<> dist(minValue, maxValue);  // Range [minValue, maxValue]

    std::vector<T> randomNumbers(size);
    for (T& num : randomNumbers) {
        num = dist(gen);  // Generate random number
    }

    return randomNumbers;
}


template<typename T>
vector<T> getSortedVec(size_t size) {

    std::vector<T> numbers(size);
    int startVal = 0;
    for (T& num : numbers) {
        num = ++startVal;  // Generate random number
    }

    return numbers;
}

TEST(test_quick_sort, test_basic_int)
{
    DataSet<int> dataSet;
    int id = 0;
//    dataSet.data[id++] = {};
//    dataSet.data[id++] = {1};
//    dataSet.data[id++] = {11, 2};
//    dataSet.data[id++] = {9, 1, 8, 2, 7, 3, 5};
//    dataSet.data[id++] = {9, 1, 8, 2, 7, 3, 5, 11, 0, 13, -1, 14, -2};
//    dataSet.data[id++] = getRandomVec<int>(2, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<int>(2);
//    dataSet.data[id++] = getRandomVec<int>(10, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<int>(10);
    dataSet.data[id++] = getRandomVec<int>(1000, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<int>(1000);
//    dataSet.data[id++] = getRandomVec<int>(100000, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<int>(100000);
//    dataSet.data[id++] = getRandomVec<int>(100000000, -1000, 1000);

    Sorter<int> sorter;

    cout << "StandartQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<StandartQSort<int>>());
    cout << "SimpleQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<SimpleQSort<int>>());
    cout << "MultithreadQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<MultithreadQSort<int>>());

    EXPECT_TRUE(sorter.verifyEqulityOfData());
}


// Function that performs work and sets the result via promise
int calculateSquare(int value) {
    return value * value;
//    resultPromise.set_value(result);  // Set the result in the promise
}

TEST(test_quick_sort, test_basic_double)
{
    DataSet<double> dataSet;
    int id = 0;
//    dataSet.data[id++] = {};
    dataSet.data[id++] = {1};
    dataSet.data[id++] = {11, 2};
//    dataSet.data[id++] = {9, 1, 8, 2, 7, 3, 5};
//    dataSet.data[id++] = getRandomVec<double>(2, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<double>(2);
//    dataSet.data[id++] = getRandomVec<double>(10, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<double>(10);
//    dataSet.data[id++] = getRandomVec<double>(1000, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<double>(1000);
    dataSet.data[id++] = getRandomVec<double>(100000, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<double>(50000);
//    dataSet.data[id++] = getRandomVec<double>(10000000, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<double>(10000000);

    Sorter<double> sorter;

    cout << "StandartQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<StandartQSort<double>>());
    cout << "SimpleQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<SimpleQSort<double>>());
    cout << "MultithreadQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<MultithreadQSort<double>>());



    EXPECT_TRUE(sorter.verifyEqulityOfData());
}