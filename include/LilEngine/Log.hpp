#pragma once

#include <iostream>
#define LIL_LOG_TRACE(msg)    std::cout << "[TRACE] " << msg << std::endl
//#define LIL_LOG_TRACE(msg)    {}

#define LIL_LOG_INFO(msg)    std::cout << "[INFO] " << msg << std::endl
#define LIL_LOG_WARN(msg)    std::cout << "[WARN] " << msg << std::endl
#define LIL_LOG_ERROR(msg)   std::cerr << "[ERROR] " << msg << std::endl