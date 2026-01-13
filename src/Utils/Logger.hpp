#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <iostream>
#include <string>
#include <fstream>
#include <mutex>
#include <vector>
#include <chrono>
#include <iomanip>
#include <filesystem>
#include <cstdlib>
#include <sstream>

// forward declaration of the C-style API exported from Utils
void disposeLogger();

class Logger {
private:
    static Logger* instance_;
    static std::mutex mutex_;
    int logLevel_;
    std::string logFileName_;
    std::ofstream logFile_;
    const std::vector<std::string> levelTags_ = {"ERR ", "WARN", "STEP", "INFO"};
    const std::vector<std::string> levelColors_ = {"#880000", "#888800", "#008800", "#000088"};
    const std::vector<std::string> levelConsoleColors_ = {"\033[1;31m", "\033[1;33m", "\033[1;32m", "\033[1;34m"};
    const std::vector<std::string> levelConsoleColorsDim_ = {"\033[2:31m", "\033[2:33m", "\033[2:32m", "\033[2:34m"};

    void genFileName();

    Logger();
    ~Logger();

public:
    static Logger* getInstance();
    void logMessageInternal(int level, const std::string &message, const std::vector<std::string> &tags = {}, const char* file = "", int line = -1);
    void dispose();
};

#endif
