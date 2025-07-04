#include "logger.h"
#include <iostream>
#include <fstream>
#include <chrono>
#include <iomanip>
#include <sstream>

std::string Logger::m_logFile = "/var/log/ddotlp.log";

void Logger::init(const std::string& logFile) {
    m_logFile = logFile;
    log(LogLevel::INFO, "Logger initialized");
}

void Logger::log(LogLevel level, const std::string& message) {
    // Get current time with milliseconds
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
    ss << '.' << std::setfill('0') << std::setw(3) << ms.count();

    std::string logEntry = "[" + ss.str() + "] [" + levelToString(level) + "] " + message;

    // Write to log file
    std::ofstream logFileStream(m_logFile, std::ios::app);
    if (logFileStream.is_open()) {
        logFileStream << logEntry << std::endl;
        logFileStream.close();
    }

    // Also write to stderr for debugging (when not daemonized)
    if (level == LogLevel::ERROR || level == LogLevel::WARNING) {
        std::cerr << logEntry << std::endl;
    }
}

void Logger::debug(const std::string& message) {
    log(LogLevel::DEBUG, message);
}

void Logger::info(const std::string& message) {
    log(LogLevel::INFO, message);
}

void Logger::warning(const std::string& message) {
    log(LogLevel::WARNING, message);
}

void Logger::error(const std::string& message) {
    log(LogLevel::ERROR, message);
}

std::string Logger::levelToString(LogLevel level) {
    switch (level) {
        case LogLevel::DEBUG:   return "DEBUG";
        case LogLevel::INFO:    return "INFO";
        case LogLevel::WARNING: return "WARNING";
        case LogLevel::ERROR:   return "ERROR";
        default:                return "UNKNOWN";
    }
}
