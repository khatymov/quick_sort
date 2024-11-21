//
// Created by Renat_Khatymov on 11/21/2024.
//

#pragma once

#include "pch.h"
/*
 * MyThreadPool class accepts jobs aka functors, push it the jobs queue
 * if there is available thread it starts executing a job from a queue
 */
template<typename T>
class MyThreadPool
{
    MyThreadPool(const MyThreadPool&) = delete;
    MyThreadPool(MyThreadPool&&) = delete;
    MyThreadPool& operator=(const MyThreadPool&) = delete;
    MyThreadPool& operator=(MyThreadPool&&) = delete;

public:

    explicit MyThreadPool(const size_t numThreads = 8);

    ~MyThreadPool();

    void start();

    void stop();

    void push(std::function<void()>&& task);

private:
    std::vector<std::thread> m_threads;
    std::queue<std::function<void()>> m_jobQueue;
    // at the beginning thread pool sleeps
    std::atomic_bool m_stop = true;
    size_t m_numThreads = 6;

    std::condition_variable m_jobCondVar;
    std::mutex m_jobsMutex;
};


template <typename T>
MyThreadPool<T>::MyThreadPool(const size_t numThreads) {
    if (numThreads < 1) {
        throw std::invalid_argument("Wrong threads number");
    }
    m_numThreads = numThreads;
}

template <typename T>
MyThreadPool<T>::~MyThreadPool() {
    stop();
}

template <typename T>
void MyThreadPool<T>::start() {

    // thread pool is started already
    if (m_stop == false) {
        return;
    }

    auto func = [this]() {
        while (true) {
            std::unique_lock<std::mutex> uniqueLock(m_jobsMutex);
            m_jobCondVar.wait(uniqueLock,[&] { return !m_jobQueue.empty() or m_stop;});
            if (m_stop) {
                break;
            }
            auto executeJob = std::move(m_jobQueue.front());
            m_jobQueue.pop();
            uniqueLock.unlock();

            executeJob();
        }
    };

    m_stop = false;
    for (size_t i = 0; i < m_numThreads; ++i) {
        m_threads.emplace_back(std::thread(func));
    }

    m_jobCondVar.notify_all();
}

template <typename T>
void MyThreadPool<T>::push(std::function<void()>&& task) {
    std::unique_lock<std::mutex> uniqueLock(m_jobsMutex);
    m_jobQueue.push(std::move(task));
    uniqueLock.unlock();
    m_jobCondVar.notify_all();
}

template <typename T>
void MyThreadPool<T>::stop() {
    if (not m_stop) {
        m_stop = true;
        m_jobCondVar.notify_all();
        for (auto& curThread:m_threads) {
            curThread.join();
        }
    }
}

