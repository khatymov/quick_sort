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
    void quickSort(std::vector<T>& data, int low, int high);
    int divideByPivotLomuto(vector<T>& data, int low, int high);
    int divideByPivotHoare(vector<T>& data, int low, int high);

    int partition(vector<T>& data, int low, int high);
    T medianOfThree(vector<T>& arr, int low, int high);
};

template <typename T>
void SimpleQSort<T>::sort(std::vector<T>& data)
{
    quickSort(data, 0, data.size() - 1);
}


template <typename T>
T SimpleQSort<T>::medianOfThree(vector<T>& arr, int low, int high)
{
    int mid = low + (high - low) / 2;
    if (arr[low] > arr[mid])
        swap(arr[low], arr[mid]);
    if (arr[low] > arr[high])
        swap(arr[low], arr[high]);
    if (arr[mid] > arr[high])
        swap(arr[mid], arr[high]);
    // Move the pivot to the end
    swap(arr[mid], arr[high - 1]);
    return arr[high - 1]; // return the pivot value
}

template <typename T>
int SimpleQSort<T>::divideByPivotHoare(vector<T>& data, int low, int high)
{
    T pivot = medianOfThree(data, low, high); // Choose the pivot (you could also choose the middle element or random)
    int i = low - 1; // Start before the first element
    int j = high + 1; // Start after the last element

    // 1 6 2 5 7 2 8
    // i 6     j 2
    // 1 2 2 5 7 6 8
    // 1 2 2 5 7 6 8
    //    ij 5
    while (true) {
        // Increment i until we find an element greater than or equal to the pivot
        do {
            i++;
        } while (data[i] < pivot);


        // Decrement j until we find an element less than or equal to the pivot
        do {
            j--;
        } while (data[j] > pivot);

        // If the two indices cross, we are done
        if (i >= j) {
            return j; // Return the partitioning index
        }

        // Swap the elements at indices i and j
        std::swap(data[i], data[j]);
    }
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

    for (int j = low; j < high; ++j) {
        if (data[j] <= pivot) {
            ++i;
            swap(data[i], data[j]);
        }
    }

    swap(data[i+1], data[high]);
    return i + 1;
}

template <typename T>
void SimpleQSort<T>::quickSort(vector<T>& data, int low, int high) {
    if (low < high) {
        const int pi = divideByPivotHoare(data, low, high);
        quickSort(data, low, pi);        // Recursively sort before partition
        quickSort(data, pi + 1, high);       // Recursively sort after partition

//        int pivotIndex = divideByPivotHoare(data, low, high);
//        // Hoare's Partition Scheme
//        quickSort(data, low, pivotIndex);
//        quickSort(data, pivotIndex + 1, high);

        // Lomuto partition scheme
//        int pivotIndex = divideByPivotLomuto(data, low, high);
//        quickSort(data, low, pivotIndex - 1);
//        quickSort(data, pivotIndex + 1, high);
    }
}

//==============================================================================================

struct Indexes {
    int pivot = 0;
    int low = 0;
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
    void quickSort(std::vector<T>& data, int low, int high, int depth);
    void nonRecursiveQuickSort(std::vector<T>& data);
//    int divideByPivot(vector<T>& data, int low, int high);
    int divideByPivotHoare(vector<T>& data, int low, int high);

    Indexes divideByPivot(vector<T>& data, int low, int high);

    T medianOfThree(vector<T>& arr, int low, int high);

    const int MAX_DEPTH = 2;
};


template <typename T>
void MultithreadQSort<T>::sort(std::vector<T>& data)
{
    quickSort(data, 0, data.size() - 1, 0);
}

template <typename T>
T MultithreadQSort<T>::medianOfThree(vector<T>& arr, int low, int high)
{
    int mid = low + (high - low) / 2;

    int k = arr[low];
    int z = arr[mid];

    const bool x = arr[low] > arr[mid];
    arr[low] = z * (x) + k * (!x);
    arr[mid] = z * (!x) + k * (x);
    // instead of
    // exclude branch prediction
//    if (arr[low] > arr[mid])
//        swap(arr[low], arr[mid]);

    if (arr[low] > arr[high])
        swap(arr[low], arr[high]);
    if (arr[mid] > arr[high])
        swap(arr[mid], arr[high]);
    // Move the pivot to the end
    swap(arr[mid], arr[high - 1]);
    return arr[high - 1]; // return the pivot value
}

template <typename T>
int MultithreadQSort<T>::divideByPivotHoare(vector<T>& data, int low, int high)
{
    T pivot = medianOfThree(data, low, high); // Choose the pivot (you could also choose the middle element or random)
    int i = low - 1; // Start before the first element
    int j = high + 1; // Start after the last element

    while (true) {
        // Increment i until we find an element greater than or equal to the pivot
        while (data[++i] < pivot);

        // Decrement j until we find an element less than or equal to the pivot
        while (data[--j] < pivot);

        // If the two indices cross, we are done
        if (i >= j) {
            return j; // Return the partitioning index
        }

        // Swap the elements at indices i and j
        std::swap(data[i], data[j]);
    }
}

//template <typename T>
//int MultithreadQSort<T>::divideByPivot(vector<T>& data, int low, int high)
//{
//    T pivot = data[high];
//    int i = low - 1;
//
//    for (int j = low; j < high; ++j) {
//        if (data[j] <= pivot) {
//            ++i;
//            swap(data[i], data[j]);
//        }
//    }
//
//    swap(data[i+1], data[high]);
//    return i + 1;
//}


template <typename T>
Indexes MultithreadQSort<T>::divideByPivot(vector<T>& data, int low, int high)
{
    T pivot = data[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        if (data[j] <= pivot) {
            ++i;
            swap(data[i], data[j]);
        }
    }

    swap(data[i+1], data[high]);
    return Indexes(i + 1, low, high);
}

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
    //      std::future<[i, low, high]> future_indexes1 = pool.submit_task(divideByPivot(arr, low, i);
    //      futures.push_back(future_indexes1);
    //      std::future<[i, low, high]> future_indexes2 = pool.submit_task(divideByPivot(arr, i+1, high);
    //      futures.push_back(future_indexes2);
    //
    //  for (auto it:futures)
    //      queue.push_back(it.get());
    // !need more generic
}

template <typename T>
void MultithreadQSort<T>::quickSort(std::vector<T>& data, int low, int high, int depth) {
    while (low < high) {
        int pi = divideByPivot(data, low, high);
//        threadPool.push();
    }
//    //    0 3 7
//    // -1 0
//    if (low < high) {
//        int pi = divideByPivotHoare(data, low, high);
//
//        if (depth < MAX_DEPTH) {
//            std::thread leftThread(&MultithreadQSort<T>::quickSort, this, std::ref(data), low, pi, depth + 1);
//            leftThread.join();
//
//            quickSort(data, pi + 1, high, depth + 1);       // Recursively sort after partition
//        } else {
//            quickSort(data, low, pi, depth + 1);        // Recursively sort before partition
//            quickSort(data, pi + 1, high, depth + 1);       // Recursively sort after partition
//        }
//    }
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
//    dataSet.data[id++] = getRandomVec<int>(2, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<int>(2);
    dataSet.data[id++] = getRandomVec<int>(10, -1000, 1000);
    dataSet.data[id++] = getSortedVec<int>(10);
//    dataSet.data[id++] = getRandomVec<int>(1000, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<int>(1000);
    dataSet.data[id++] = getRandomVec<int>(100000, -1000, 1000);
    dataSet.data[id++] = getSortedVec<int>(100000);
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

TEST(test_quick_sort, test_basic_double)
{
    DataSet<double> dataSet;
    int id = 0;
    //    dataSet.data[id++] = {};
    //    dataSet.data[id++] = {1};
    //    dataSet.data[id++] = {11, 2};
    //    dataSet.data[id++] = {9, 1, 8, 2, 7, 3, 5};
    dataSet.data[id++] = getRandomVec<double>(2, -1000, 1000);
    dataSet.data[id++] = getSortedVec<double>(2);
    dataSet.data[id++] = getRandomVec<double>(10, -1000, 1000);
    dataSet.data[id++] = getSortedVec<double>(10);
    dataSet.data[id++] = getRandomVec<double>(1000, -1000, 1000);
    dataSet.data[id++] = getSortedVec<double>(1000);
    dataSet.data[id++] = getRandomVec<double>(1000000, -1000, 1000);
    dataSet.data[id++] = getSortedVec<double>(1000000);
    dataSet.data[id++] = getRandomVec<double>(10000000, -1000, 1000);
    dataSet.data[id++] = getSortedVec<double>(10000000);

    Sorter<double> sorter;

    cout << "StandartQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<StandartQSort<double>>());
    cout << "SimpleQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<SimpleQSort<double>>());
    cout << "MultithreadQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<MultithreadQSort<double>>());

    EXPECT_TRUE(sorter.verifyEqulityOfData());
}