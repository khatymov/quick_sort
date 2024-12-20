/*! \file timer.cpp
 * \brief Timer class implementation.
 */
#include "timer.h"

Timer::Timer(int dataSetId /*= -1*/, size_t dataSize /*= 0*/) :
      dataSetId_(dataSetId),
      dataSize_(dataSize),
      startTime(std::chrono::system_clock::now())
{}

Timer::~Timer() {
    // Calculate duration
    auto duration = (std::chrono::system_clock::now() - startTime);

    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    duration -= seconds;

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    duration -= milliseconds;

    auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    duration -= microseconds;

    auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);

    if (dataSetId_ != -1 and dataSize_ != 0) {
        std::cout << "size: " << dataSize_ << std::endl;
    }

    std::cout << seconds.count() << "'"
              << std::setw(3) << std::setfill('0') << milliseconds.count() << "'"
              << std::setw(3) << std::setfill('0') << microseconds.count() << "'"
              << std::setw(3) << std::setfill('0') << nanoseconds.count()
              << std::endl;
}