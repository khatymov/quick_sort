#include "gtest/gtest.h"

#include "my_class_test.h"
#include "timer.h"

using namespace std;
using namespace testing;

#include <vector>
#include <algorithm>
#include <thread>
#include <map>
#include <random>
#include <cstdlib>
#include <future>
#include <stack>
#include <span>


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
    int divideByPivotLomuto(vector<T>& data, int low, int high);
};

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

        auto pivotLomuto = curInterval[curInterval.size() -1];
        auto it = std::partition(curInterval.begin(), curInterval.end(), [&pivotLomuto](int num){return num < pivotLomuto;});
        std::nth_element(curInterval.begin(), it, curInterval.end());

        // all sorted, continue
        if (it == curInterval.end()) {
            continue;
        }

        spanStack.push({curInterval.begin(), it});
        spanStack.push({++it, curInterval.end()});
    }
}

template <typename T>
void SimpleQSort<T>::sort(std::vector<T>& data)
{
    nonRecursiveSort(data);
//    quickSort(data, 0, data.size() - 1);
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
//    nonRecursiveSort(data);
    if (low < high) {
        // Lomuto partition scheme
        int pivotIndex = divideByPivotLomuto(data, low, high);
        quickSort(data, low, pivotIndex - 1);
        quickSort(data, pivotIndex + 1, high);
    }
}

//==============================================================================================

struct Indexes {
    int low = 0;
    int pivot = 0;
    int high = 0;
};

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

    int divideByPivotHoare(vector<T>& data, int low, int high);

    static Indexes divideByPivot(vector<T>& data, int low, int high);

//    static pair<span<T>,span<T>> divideByPivot(span<T> data);
};


template <typename T>
void MultithreadQSort<T>::sort(std::vector<T>& data)
{
    nonRecursiveQuickSort(data);
//    for (const auto& it: data) {
//        cout << it << "\t";
//    }
//    cout << endl;
}

template <typename T>
Indexes MultithreadQSort<T>::divideByPivot(vector<T>& data, int low, int high)
{
    T pivot = data[high];
    int i = low - 1;

    for (int j = low; j <= high - 1; ++j) {
        if (data[j] < pivot) {
            ++i;
            swap(data[i], data[j]);
        }
    }

    swap(data[i+1], data[high]);
    return Indexes(low, i + 1, high);
}

#include <queue>
#include "BS_thread_pool.hpp"
#include "BS_thread_pool_utils.hpp"

#include <algorithm>

#include <span>

// задача тред пула - получать задачу, которая будет выполняться в отдельном потоке
// специфика данного тред пула, что он должен брать спан, сортировать его и отдавать два отсортированных спана
//

template<typename T>
class ThreadPool{
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

public:

    span<T> getSpan() const;

private:
    std::vector<std::thread> m_threads;
};

template <typename T>
void MultithreadQSort<T>::nonRecursiveQuickSort(std::vector<T>& data) {
    // до каких пор будем вызывать?
    // thread_pool(8)
    // [i, low, high] = divideByPivot(arr, low, high);
    // queue.push_back([i, low, high])
    // vector<future> futures; reserve(8)
    // while (not queue.empty)
    //  [i, low, high] = queue.front();
    //  queue.pop();
    //  if low < high
    //      std::future<[i, low, high]> future_indexes1 = pool.submit_task(divideByPivot(arr, low, i-1);
    //      futures.push_back(future_indexes1);
    //      std::future<[i, low, high]> future_indexes2 = pool.submit_task(divideByPivot(arr, i+1, high);
    //      futures.push_back(future_indexes2);
    //
    //  for (auto it:futures)
    //      queue.push_back(it.get());
    // !need more generic
    //
    // interfaces
    // components: thread_pool where jobs are executed + functor
    //
    // loop
    //   thread_pool(partition(data))

//    auto it = std::partition(data.begin(), data.end(), std::less<>);

    if (data.empty() or data.size() == 1) {
        return;
    }

    // написать АПИ в headers
    // все span -> span0/1
    // [s0, s1] = divide[s];
    // [s2, s3] = divide[s0];
    // [s4, s5] = divide[s1];

    // threadPool(8);
    // threadPool.push_back(std::span data);
    // std::span span = threadPool.getSpan();
    // 1 -> 8
    // 8 -> 1
    // when its done? what is the condition
    // video, how many algorithm exist

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
    dataSet.data[id++] = {};
    dataSet.data[id++] = {1};
    dataSet.data[id++] = {11, 2};
    dataSet.data[id++] = {9, 1, 8, 2, 7, 3, 5};
//    dataSet.data[id++] = getRandomVec<int>(2, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<int>(2);
//    dataSet.data[id++] = getRandomVec<int>(10, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<int>(10);
//    dataSet.data[id++] = getRandomVec<int>(1000, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<int>(1000);
//    dataSet.data[id++] = getRandomVec<int>(100000, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<int>(100000);
//    dataSet.data[id++] = getRandomVec<int>(100000000, -1000, 1000);

    Sorter<int> sorter;

    cout << "StandartQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<StandartQSort<int>>());
    cout << "SimpleQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<SimpleQSort<int>>());
//    cout << "MultithreadQSort:" << endl;
//    sorter.sort(dataSet, std::make_unique<MultithreadQSort<int>>());

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
//        dataSet.data[id++] = {};
//        dataSet.data[id++] = {1};
//        dataSet.data[id++] = {11, 2};
//        dataSet.data[id++] = {9, 1, 8, 2, 7, 3, 5};
//    dataSet.data[id++] = getRandomVec<double>(2, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<double>(2);
//    dataSet.data[id++] = getRandomVec<double>(10, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<double>(10);
    dataSet.data[id++] = getRandomVec<double>(1000, -1000, 1000);
    dataSet.data[id++] = getSortedVec<double>(1000);
    dataSet.data[id++] = getRandomVec<double>(100000, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<double>(100000);
//    dataSet.data[id++] = getRandomVec<double>(10000000, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<double>(10000000);

    Sorter<double> sorter;

    cout << "StandartQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<StandartQSort<double>>());
    cout << "SimpleQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<SimpleQSort<double>>());
//    cout << "MultithreadQSort:" << endl;
//    sorter.sort(dataSet, std::make_unique<MultithreadQSort<double>>());



    EXPECT_TRUE(sorter.verifyEqulityOfData());
}