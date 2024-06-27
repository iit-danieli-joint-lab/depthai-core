#pragma once

#include <mutex>
#include <vector>
#include <unordered_map>
#include <string>
#include <cstdint>
#include <thread>

// libraries
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <fmt/ostream.h>
#include <fmt/chrono.h>

// shared
#include <depthai-shared/log/LogConstants.hpp>

// project
#include <depthai/device/Device.hpp>
#include <depthai/device/DeviceBootloader.hpp>
#include <depthai/openvino/OpenVINO.hpp>
#include <depthai/utility/Path.hpp>
#include "Environment.hpp"

#include <depthai/common/CameraBoardSocket.hpp>
#include <depthai-shared/datatype/DatatypeEnum.hpp>

namespace dai {

class Logging {
    // private constructor
    Logging();
    ~Logging() {}

public:
    static Logging& getInstance() {
        static Logging logging;
        return logging;
    }
    Logging(Logging const&) = delete;
    void operator=(Logging const&) = delete;

    // Public API
    spdlog::logger logger;
    spdlog::level::level_enum parseLevel(std::string lvl);
};


namespace logger
{

inline spdlog::level::level_enum get_level()
{
    return Logging::getInstance().logger.level();
}

template<typename FormatString, typename... Args>
inline void log(spdlog::source_loc source, spdlog::level::level_enum lvl, const FormatString &fmt, Args&&...args)
{
    Logging::getInstance().logger.log(source, lvl, fmt, std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
inline void log(spdlog::level::level_enum lvl, const FormatString &fmt, Args&&...args)
{
    Logging::getInstance().logger.log(spdlog::source_loc{}, lvl, fmt, std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
inline void trace(const FormatString &fmt, Args&&...args)
{
    Logging::getInstance().logger.trace(fmt, std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
inline void debug(const FormatString &fmt, Args&&...args)
{
    Logging::getInstance().logger.debug(fmt, std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
inline void info(const FormatString &fmt, Args&&...args)
{
    Logging::getInstance().logger.info(fmt, std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
inline void warn(const FormatString &fmt, Args&&...args)
{
    Logging::getInstance().logger.warn(fmt, std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
inline void error(const FormatString &fmt, Args&&...args)
{
    Logging::getInstance().logger.error(fmt, std::forward<Args>(args)...);
}

template<typename FormatString, typename... Args>
inline void critical(const FormatString &fmt, Args&&...args)
{
    Logging::getInstance().logger.critical(fmt, std::forward<Args>(args)...);
}

template<typename T>
inline void log(spdlog::source_loc source, spdlog::level::level_enum lvl, const T &msg)
{
    Logging::getInstance().logger.log(source, lvl, msg);
}

template<typename T>
inline void log(spdlog::level::level_enum lvl, const T &msg)
{
    Logging::getInstance().logger.log(lvl, msg);
}

template<typename T>
inline void trace(const T &msg)
{
    Logging::getInstance().logger.trace(msg);
}

template<typename T>
inline void debug(const T &msg)
{
    Logging::getInstance().logger.debug(msg);
}

template<typename T>
inline void info(const T &msg)
{
    Logging::getInstance().logger.info(msg);
}

template<typename T>
inline void warn(const T &msg)
{
    Logging::getInstance().logger.warn(msg);
}

template<typename T>
inline void error(const T &msg)
{
    Logging::getInstance().logger.error(msg);
}

template<typename T>
inline void critical(const T &msg)
{
    Logging::getInstance().logger.critical(msg);
}

} // namespace logger


} // namespace dai

template <>
struct fmt::formatter<dai::CameraBoardSocket> : ostream_formatter {};

//template <>
//struct fmt::formatter<std::chrono::microseconds> : ostream_formatter {};

//template <>
//struct fmt::formatter<dai::DatatypeEnum> : ostream_formatter {};

template <>
struct fmt::formatter<dai::DatatypeEnum> : fmt::formatter<std::string> {
    auto format(dai::DatatypeEnum my, format_context& ctx) const -> decltype(ctx.out()) {
        return fmt::format_to(ctx.out(), "{}", static_cast<int32_t>(my));
    }
};
