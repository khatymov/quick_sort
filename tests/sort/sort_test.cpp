#include "gtest/gtest.h"

#include "sort_test.h"
#include "timer.h"

using namespace std;
using namespace testing;

#include "pch.h"

#include "sorter_interface.h"
#include "simple_sort.h"
#include "standart_qsort.h"
#include "multithread_qsort.h"

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

TEST(test_sort, test_sort_random_double)
{
    DataSet<double> dataSet;
    int id = 0;
    dataSet.data[id++] = getRandomVec<double>(2, -1000, 1000);
    dataSet.data[id++] = getRandomVec<double>(10, -1000, 1000);
    dataSet.data[id++] = getRandomVec<double>(1000, -1000, 1000);
    dataSet.data[id++] = getRandomVec<double>(1000000, -1000, 1000);

    Sorter<double> sorter;

    cout << "StandartQSort algorithm" << endl;
    sorter.sort(dataSet, std::make_unique<StandartQSort<double>>());
    cout << "MultithreadQSort algorithm" << endl;
    sorter.sort(dataSet, std::make_unique<MultithreadQSort<double>>());
    cout << "SimpleQSort algorithm" << endl;
    sorter.sort(dataSet, std::make_unique<SimpleQSort<double>>());


    EXPECT_TRUE(sorter.verifyEqulityOfData());
}

TEST(test_sort, test_sort_sorted_double)
{
    DataSet<double> dataSet;
    int id = 0;
    dataSet.data[id++] = getSortedVec<double>(2);
    dataSet.data[id++] = getSortedVec<double>(10);
    dataSet.data[id++] = getSortedVec<double>(1000);
    dataSet.data[id++] = getSortedVec<double>(1000000);

    Sorter<double> sorter;

    cout << "StandartQSort algorithm" << endl;
    sorter.sort(dataSet, std::make_unique<StandartQSort<double>>());
    cout << "MultithreadQSort algorithm" << endl;
    sorter.sort(dataSet, std::make_unique<MultithreadQSort<double>>());
    cout << "SimpleQSort algorithm" << endl;
    sorter.sort(dataSet, std::make_unique<SimpleQSort<double>>());


    EXPECT_TRUE(sorter.verifyEqulityOfData());
}
