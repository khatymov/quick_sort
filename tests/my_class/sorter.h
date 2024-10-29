//
// Created by Renat_Khatymov on 10/3/2024.
//

#ifndef MY_PROJECT_ISORTER_H
#define MY_PROJECT_ISORTER_H


#include <vector>

template<typename T>
class ISorter {
public:
    virtual void sort(std::vector<T>& data) = 0;
    ~ISorter() = default;
};

#endif // MY_PROJECT_ISORTER_H
