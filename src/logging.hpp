// =========================================================================
//   Copyright (C) 2009-2026 by Anders Piniesjö <pugo@pugo.org>
//
//   This program is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program.  If not, see <http://www.gnu.org/licenses/>
// =========================================================================

#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <ostream>
#include <sstream>

#include <spdlog/spdlog.h>


namespace auric
{

constexpr spdlog::level::level_enum log_level_trace()
{
    return spdlog::level::trace;
}

constexpr spdlog::level::level_enum log_level_debug()
{
    return spdlog::level::debug;
}

constexpr spdlog::level::level_enum log_level_info()
{
    return spdlog::level::info;
}

constexpr spdlog::level::level_enum log_level_warning()
{
    return spdlog::level::warn;
}

constexpr spdlog::level::level_enum log_level_error()
{
    return spdlog::level::err;
}

constexpr spdlog::level::level_enum log_level_fatal()
{
    return spdlog::level::critical;
}

class LogStream
{
public:
    explicit LogStream(spdlog::level::level_enum level) :
        level_{level}
    {
    }

    ~LogStream()
    {
        spdlog::log(level_, "{}", stream_.str());
    }

    template<typename T>
    LogStream& operator<<(const T& value)
    {
        stream_ << value;
        return *this;
    }

    using OstreamManipulator = std::ostream& (*)(std::ostream&);

    LogStream& operator<<(OstreamManipulator manipulator)
    {
        manipulator(stream_);
        return *this;
    }

private:
    spdlog::level::level_enum level_;
    std::ostringstream stream_;
};

}


#define AURIC_LOG(level) auric::LogStream(auric::log_level_##level())

#endif
