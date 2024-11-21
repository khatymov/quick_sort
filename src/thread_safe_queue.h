//
// Created by Renat_Khatymov on 11/21/2024.
//

#pragma once

#include "pch.h"

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
void ThreadSafeQueue<T>::push(std::span<T> data) {
    std::lock_guard<std::mutex> lockGuard(m_mutex);
    m_queue.push(data);
    //    m_condVar.notify_one();
}

template <typename T>
std::span<T> ThreadSafeQueue<T>::get()
{
    //    std::unique_lock<std::mutex> uniqueLock(m_mutex);
    //    m_condVar.wait(uniqueLock, [&]{return !m_queue.empty();});
    auto data = m_queue.front();
    m_queue.pop();
    return data;
}

