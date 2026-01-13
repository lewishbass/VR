#include "Utils.hpp"


void disposeLogger()
{
    Logger::getInstance()->dispose();
}

void logMessageInternal(int level, const std::string &message, const std::vector<std::string> &tags, const char* file, int line)
{
    if (CLI_LOGGING == 0)
        return;
    
    Logger::getInstance()->logMessageInternal(level, message, tags, file, line);
}