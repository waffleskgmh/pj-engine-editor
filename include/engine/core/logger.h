#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <chrono>

namespace PJEngine {

enum class LogLevel {
    INFO,
    WARNING,
    LOG_ERROR
};

struct LogMessage {
    LogLevel level = LogLevel::INFO;
    std::string message;
    std::string timestamp;

    LogMessage() = default;
    LogMessage(LogLevel lvl, const std::string& msg, const std::string& time)
        : level(lvl), message(msg), timestamp(time) {}
};

class Logger {
public:
    static Logger& get();

    static void info(const std::string& msg);
    static void warn(const std::string& msg);
    static void error(const std::string& msg);
    static void log(LogLevel level, const std::string& msg);

    void clear();
    std::vector<LogMessage> getMessages();
    size_t getMessageCount();

private:
    Logger() = default;
    ~Logger() = default;

    std::mutex logMutex;
    std::vector<LogMessage> messageBuffer;
    static constexpr size_t MAX_LOG_MESSAGES = 1000;

    static std::string getCurrentTimeString();
};

} // namespace PJEngine