#include "Logger.hpp"
#include "Utils.hpp"

Logger *Logger::instance_ = nullptr;
std::mutex Logger::mutex_;

Logger::Logger() : logLevel_(CLI_LOGGING), logFileName_("")
{
    if (logFileName_.empty())
    {
        genFileName();
    }
    logFile_.open(logFileName_, std::ios::out | std::ios::app);
    if (!logFile_.is_open())
    {
        std::cerr << "Failed to open log file: " << logFileName_ << std::endl;
    }
    else
    {
        logFile_ << "<html><head><title>Log File</title><link rel=\"stylesheet\" href=\"logs.css\"><script src=\"logs.js\"></script></head><body>" << std::endl;
        logFile_ << "<h1>Log " << logFileName_ << "</h1>" << std::endl;
    }
    std::cout << "Logger initialized with log file: " << logFileName_ << std::endl;
    atexit(disposeLogger);
}

Logger::~Logger()
{
    if (logFile_.is_open())
    {
        logFile_ << "</body></html>" << std::endl;
        logFile_.flush();
        logFile_.close();
    }
}

Logger *Logger::getInstance()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ == nullptr)
    {
        instance_ = new Logger();
    }
    return instance_;
}

void Logger::logMessageInternal(int level, const std::string &message, const std::vector<std::string> &tags, const char *file, int line)
{
    level = std::max(1, std::min(level, static_cast<int>(levelTags_.size()))); // clamp level between 1 and max
    // Get current time
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream timeStream;
    timeStream << std::put_time(std::localtime(&in_time_t), "%X");

    std::string filename = std::filesystem::path(file).stem().string();

    if (CLI_LOGGING >= level)
    {
        // Format message
        std::stringstream formattedMessage;
        formattedMessage << "\033[90m" << "[" << timeStream.str() << "] ";
        formattedMessage << "\033[95m" << std::left << std::setw(15) << filename;
        if (line != -1)
        {
            formattedMessage << " : " << std::right << std::setw(4) << line;
        }
        formattedMessage << " ";
        formattedMessage << levelConsoleColors_[level - 1] << "[" << levelTags_[level - 1] << "] ";
        formattedMessage << levelConsoleColorsDim_[level - 1];

        // Log to console
        std::cout << formattedMessage.str();

        std::string tmpMsg = message;
        bool first = true;
        while (tmpMsg.length() > 0)
        {
            size_t pos = tmpMsg.find('\n');
            if (pos != std::string::npos)
            {
                std::string line = tmpMsg.substr(0, pos);
                if (!first)
                    line = "                                     " + line;
                std::cout << line << std::endl;
                tmpMsg = tmpMsg.substr(pos + 1);
                first = false;
            }
            else
            {
                std::cout << (first ? "" : "                                     ") << tmpMsg << std::endl;
                break;
            }
        }

        // reset CLI color
        std::cout << "\033[0m";
    }

    // Log to file
    if (logFile_.is_open())
    {
        std::string htmlMessage = message;
        size_t pos = 0;
        while ((pos = htmlMessage.find('\n', pos)) != std::string::npos)
        {
            htmlMessage.replace(pos, 1, "<br>");
            pos += 4; // length of <br>
        }

        logFile_ << "<div class=\"log-entry level-" << level << "\" data-tags=\"";
        for (const auto &tag : tags)
        {
            logFile_ << tag << " ";
        }
        logFile_ << "\"";
        logFile_ << " data-file=\"" << file << "\"";
        if (line != -1)
        {
            logFile_ << " data-line=\"" << line << "\"";
        }
        logFile_ << ">";
        logFile_ << "<div class=\"time\">[" << timeStream.str() << "]</div> ";
        logFile_ << "<div class=\"level-tag\">[" << levelTags_[level - 1] << "]</div> ";
        logFile_ << "<div class=\"message\">" << htmlMessage << "</div>";
        logFile_ << "</div>" << std::endl;
    }
}

void Logger::genFileName()
{
    try
    {
        std::string logDir = "logs";
        std::filesystem::create_directories(logDir);
        if (!std::filesystem::exists(logDir))
            throw std::runtime_error("Failed to create logs directory");

        if (!std::filesystem::is_directory(logDir))
            throw std::runtime_error("logs exists but is not a directory");

        int logIndex = 0;
        for (const auto &entry : std::filesystem::directory_iterator(logDir))
        {
            if (!entry.is_regular_file())
                continue;

            try
            {
                logIndex = std::max(logIndex, std::stoi(entry.path().filename().stem().string()));
            }
            catch (const std::exception &e)
            {
                continue;
            }
        }
        std::stringstream datestream;
        auto now = std::chrono::system_clock::now();
        auto in_time_t = std::chrono::system_clock::to_time_t(now);
        datestream << std::put_time(std::localtime(&in_time_t), "%m-%d");

        std::stringstream ss;
        ss << std::setw(4) << std::setfill('0') << (logIndex + 1);
        this->logFileName_ = logDir + "/" + ss.str() + "_log_" + datestream.str() + ".html";
    }
    catch (const std::exception &e)
    {
        std::cerr << "Exception while creating logs directory: " << e.what() << std::endl;
        this->logFileName_ = "log.html";
        return;
    }
}

void Logger::dispose()
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (instance_ != nullptr)
    {
        delete instance_;
        instance_ = nullptr;
    }
}
