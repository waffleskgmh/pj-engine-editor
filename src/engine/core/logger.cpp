#include "engine/core/logger.h"
#include <iostream>
#include <iomanip>
#include <sstream>

namespace PJEngine {

Logger& Logger::get() {
    static Logger instance;
    return instance;
}

std::string Logger::getCurrentTimeString() {
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    struct tm timeinfo;
#if defined(_WIN32)
    localtime_s(&timeinfo, &in_time_t);
#else
    localtime_r(&in_time_t, &timeinfo);
#endif
    ss << std::put_time(&timeinfo, "%H:%M:%S");
    return ss.str();
}

void Logger::log(LogLevel level, const std::string& msg) {
    Logger& inst = get();
    std::lock_guard<std::mutex> lock(inst.logMutex);

    std::string timeStr = getCurrentTimeString();
    inst.messageBuffer.emplace_back(level, msg, timeStr);

    if (inst.messageBuffer.size() > MAX_LOG_MESSAGES) {
        inst.messageBuffer.erase(inst.messageBuffer.begin());
    }

    // Zároveň vypíšeme správu do štandardného terminálu
    if (level == LogLevel::LOG_ERROR) {
        std::cerr << "[" << timeStr << "] [ERROR]: " << msg << std::endl;
    } else if (level == LogLevel::WARNING) {
        std::cout << "[" << timeStr << "] [WARN]: " << msg << std::endl;
    } else {
        std::cout << "[" << timeStr << "] [INFO]: " << msg << std::endl;
    }
}

void Logger::info(const std::string& msg) {
    log(LogLevel::INFO, msg);
}

void Logger::warn(const std::string& msg) {
    log(LogLevel::WARNING, msg);
}

void Logger::error(const std::string& msg) {
    log(LogLevel::LOG_ERROR, msg);
}

void Logger::clear() {
    std::lock_guard<std::mutex> lock(logMutex);
    messageBuffer.clear();
}

std::vector<LogMessage> Logger::getMessages() {
    std::lock_guard<std::mutex> lock(logMutex);
    return messageBuffer;
}

size_t Logger::getMessageCount() {
    std::lock_guard<std::mutex> lock(logMutex);
    return messageBuffer.size();
}

} // namespace PJEngine