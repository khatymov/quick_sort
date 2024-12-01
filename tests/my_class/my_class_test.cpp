#include "gtest/gtest.h"

#include "my_class_test.h"
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

TEST(test_quick_sort, test_basic_int)
{

    DataSet<int> dataSet;
    int id = 0;
//    dataSet.data[id++] = {};
//    dataSet.data[id++] = {1};
//    dataSet.data[id++] = {11, 2};
//    dataSet.data[id++] = {9, 1, 8, 2, 7, 3, 5}; //7 = 4
//    dataSet.data[id++] = {9, 1, 8, 2, 7, 3, 5, 11, 0, 13, -1, 14, -2}; //13 = 8
//    dataSet.data[id++] = {-848, -999, 815, -247, -106, -401, -946, 454, 684, 529};
//    dataSet.data[id++] = {-946, -999};
//    dataSet.data[id++] = {-229, -244, 139, 849, -479, 529, -798, 815, -247, -106, -401, -456, 454, 684, -263};
//    dataSet.data[id++] = {-456, -798, -263, -229, -244, 139, 849, 727, 897, -479, -848, -999, 815, -247, -106, -401, -946, 454, 684, 529};
//    dataSet.data[id++] = getRandomVec<int>(2, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<int>(5);
//    dataSet.data[id++] = getRandomVec<int>(13, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<int>(13);
//    dataSet.data[id++] = getRandomVec<int>(1000, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<int>(1000);
//    dataSet.data[id++] = getRandomVec<int>(100000, -1000, 1000);
    dataSet.data[id++] = getSortedVec<int>(100000);
    dataSet.data[id++] = getRandomVec<int>(10000000, -1000, 1000);

    Sorter<int> sorter;

    cout << "StandartQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<StandartQSort<int>>());
    cout << "SimpleQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<SimpleQSort<int>>());
    cout << "MultithreadQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<MultithreadQSort<int>>());
//
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
//    dataSet.data[id++] = getRandomVec<double>(2, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<double>(2);
//    dataSet.data[id++] = getRandomVec<double>(75000, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<double>(75000);
//    dataSet.data[id++] = getRandomVec<double>(100000, -1000, 1000);
//    dataSet.data[id++] = getSortedVec<double>(100000);
    dataSet.data[id++] = getRandomVec<double>(1000000, -1000, 1000);
    dataSet.data[id++] = getSortedVec<double>(1000000);
    dataSet.data[id++] = getRandomVec<double>(10000000, -1000, 1000);
    dataSet.data[id++] = getSortedVec<double>(10000000);

    Sorter<double> sorter;

    cout << "StandartQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<StandartQSort<double>>());
    cout << "MultithreadQSort:" << endl;
    sorter.sort(dataSet, std::make_unique<MultithreadQSort<double>>());
    cout << "MySOrt:" << endl;
//    sorter.sort(dataSet, std::make_unique<MySorter<double>>());


    EXPECT_TRUE(sorter.verifyEqulityOfData());
}