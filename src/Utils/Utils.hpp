#ifndef UTILS_HPP
#define UTILS_HPP

#include <iostream>
#include <string>
#include <vector>

#include <chrono>
#include <iomanip>
#include <filesystem>
#include <cstdlib>

#include "Logger.hpp"

#define logMessage(...) logMessageInternal(__VA_ARGS__, __FILE__, __LINE__)

void logMessageInternal(int level, const std::string &message, const std::vector<std::string> &tags = {}, const char* file = "", int line = -1);
// writes to console with color and saves to html file
// higher numbers are more granlular
// prints up to and including the set level
// -1 - all logging
// 0 - no logging
// 1 - fatal errors only
// 2 - warnings
// 3 - step titles
// 4 - detailed info

void disposeLogger();



#endif
