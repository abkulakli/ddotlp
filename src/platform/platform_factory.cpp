#include "platform/platform_factory.h"
#include "logger.h"
#include <memory>

// Forward declarations for platform-specific factory functions
// Only declare functions for the current platform
#if defined(__linux__)
std::unique_ptr<ISystemMonitor> createLinuxSystemMonitor();
std::unique_ptr<IPowerManager> createLinuxPowerManager();
std::unique_ptr<IPlatformUtils> createLinuxPlatformUtils();
std::unique_ptr<IDaemon> createLinuxDaemon();
#elif defined(_WIN32) || defined(_WIN64)
std::unique_ptr<ISystemMonitor> createWindowsSystemMonitor();
std::unique_ptr<IPowerManager> createWindowsPowerManager();
std::unique_ptr<IPlatformUtils> createWindowsPlatformUtils();
std::unique_ptr<IDaemon> createWindowsDaemon();
#elif defined(__APPLE__) && defined(__MACH__)
std::unique_ptr<ISystemMonitor> createMacOSSystemMonitor();
std::unique_ptr<IPowerManager> createMacOSPowerManager();
std::unique_ptr<IPlatformUtils> createMacOSPlatformUtils();
std::unique_ptr<IDaemon> createMacOSDaemon();
#endif

/**
 * Create a system monitor for the current platform
 * @return unique_ptr to platform-specific system monitor implementation
 */
std::unique_ptr<ISystemMonitor> PlatformFactory::createSystemMonitor() {
#if defined(__linux__)
    Logger::debug("Creating Linux system monitor");
    return createLinuxSystemMonitor();
#elif defined(_WIN32) || defined(_WIN64)
    Logger::debug("Creating Windows system monitor");
    return createWindowsSystemMonitor();
#elif defined(__APPLE__) && defined(__MACH__)
    Logger::debug("Creating macOS system monitor");
    return createMacOSSystemMonitor();
#else
    Logger::error("Unsupported platform for system monitor");
    return nullptr;
#endif
}

/**
 * Create a power manager for the current platform
 * @return unique_ptr to platform-specific power manager implementation
 */
std::unique_ptr<IPowerManager> PlatformFactory::createPowerManager() {
#if defined(__linux__)
    Logger::debug("Creating Linux power manager");
    return createLinuxPowerManager();
#elif defined(_WIN32) || defined(_WIN64)
    Logger::debug("Creating Windows power manager");
    return createWindowsPowerManager();
#elif defined(__APPLE__) && defined(__MACH__)
    Logger::debug("Creating macOS power manager");
    return createMacOSPowerManager();
#else
    Logger::error("Unsupported platform for power manager");
    return nullptr;
#endif
}

/**
 * Create platform utilities for the current platform
 * @return unique_ptr to platform-specific platform utilities implementation
 */
std::unique_ptr<IPlatformUtils> PlatformFactory::createPlatformUtils() {
#if defined(__linux__)
    Logger::debug("Creating Linux platform utilities");
    return createLinuxPlatformUtils();
#elif defined(_WIN32) || defined(_WIN64)
    Logger::debug("Creating Windows platform utilities");
    return createWindowsPlatformUtils();
#elif defined(__APPLE__) && defined(__MACH__)
    Logger::debug("Creating macOS platform utilities");
    return createMacOSPlatformUtils();
#else
    Logger::error("Unsupported platform for platform utilities");
    return nullptr;
#endif
}

/**
 * Create a daemon for the current platform
 * @return unique_ptr to platform-specific daemon implementation
 */
std::unique_ptr<IDaemon> PlatformFactory::createDaemon() {
#if defined(__linux__)
    Logger::debug("Creating Linux daemon");
    return createLinuxDaemon();
#elif defined(_WIN32) || defined(_WIN64)
    Logger::debug("Creating Windows daemon");
    return createWindowsDaemon();
#elif defined(__APPLE__) && defined(__MACH__)
    Logger::debug("Creating macOS daemon");
    return createMacOSDaemon();
#else
    Logger::error("Unsupported platform for daemon");
    return nullptr;
#endif
}

/**
 * Get the current platform name
 * @return "linux", "windows", or "unknown"
 */
std::string PlatformFactory::getCurrentPlatform() {
#if defined(__linux__)
    return "linux";
#elif defined(_WIN32) || defined(_WIN64)
    return "windows";
#else
    return "unknown";
#endif
}
