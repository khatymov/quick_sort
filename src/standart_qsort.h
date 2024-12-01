//
// Created by Renat_Khatymov on 11/21/2024.
//

#pragma once

#include "sorter_interface.h"

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
    std::sort(data.begin(), data.end());
}

