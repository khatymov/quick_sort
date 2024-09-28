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
    const auto duration = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now() - startTime);
    // Extract seconds and milliseconds
    const auto microsec  = std::chrono::duration_cast<std::chrono::microseconds>(duration);
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    const auto milliseconds = duration - seconds;
    // Print the results
    if (dataSetId_ != -1 and dataSize_ != 0) {
        std::cout << "id: " << dataSetId_ << "\nsize: " << dataSize_ << std::endl;
    }
    std::cout << "Process time: " << microsec.count() << " microseconds\n";
//    std::cout << "Process time: " << seconds.count() << "seconds\n";
}