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

