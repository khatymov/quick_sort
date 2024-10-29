//
// Created by Renat_Khatymov on 10/3/2024.
//

#ifndef MY_PROJECT_SIMPLESORT_H
#define MY_PROJECT_SIMPLESORT_H

#include <vector>


template<typename T>
class SimpleQSort: public ISorter<T>{
    SimpleQSort(const SimpleQSort&) = delete;
    SimpleQSort(SimpleQSort&&) = delete;
    SimpleQSort& operator=(const SimpleQSort&) = delete;
    SimpleQSort& operator=(SimpleQSort&&) = delete;
public:
    SimpleQSort() = default;

    void sort(std::vector<T>& data) override {
        quickSort(data, 0, data.size() - 1);
    }

protected:
    void quickSort(std::vector<T>& data, int low, int high);
    int divideByPivotLomuto(vector<T>& data, int low, int high);
    int divideByPivotHoare(vector<T>& data, int low, int high);

    int partition(vector<T>& data, int low, int high);
    T medianOfThree(vector<T>& arr, int low, int high);
};

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
        int pi = divideByPivotHoare(data, low, high);
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


#endif // MY_PROJECT_SIMPLESORT_H
