//
// Created by Renat_Khatymov on 11/21/2024.
//

#pragma once

#include "pch.h"

template<typename T>
class ISorter {
public:
    virtual void sort(std::vector<T>& data) = 0;
    ~ISorter() = default;
};