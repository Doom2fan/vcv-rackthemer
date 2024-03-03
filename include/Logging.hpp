/*
 *  RackThemer
 *  Copyright (C) 2024 Chronos "phantombeta" Ouroboros
 *  Copyright (C) 2023 Paul Chase Dempsey pcdempsey@live.com [svg_theme]
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <functional>
#include <memory>
#include <string>

namespace rack_themer {
namespace logging {
    enum class Severity {
        Info,
        Warn,
        Error,
        Critical,
    };

    enum class ErrorCode {
        Unspecified                  = 0,
        NoError                      = 1,
        CannotOpenJsonFile           = 2,
        JsonParseFailed              = 3,
        ArrayExpected                = 4,
        ObjectExpected               = 5,
        ObjectOrStringExpected       = 6,
        StringExpected               = 7,
        NumberExpected               = 8,
        IntegerExpected              = 9,
        NameExpected                 = 10,
        ThemeExpected                = 11,
        InvalidHexColor              = 12,
        OneOfColorOrGradient         = 13,
        TwoGradientStopsMax          = 14,
        GradientStopIndexZeroOrOne   = 15,
        GradientStopNotPresent       = 16,
        RemovingGradientNotSupported = 17,
        GradientNotPresent           = 18,
        InvalidLineCap               = 19,
    };

    // Logging callback function you provide.
    typedef std::function<void (Severity severity, ErrorCode code, std::string info)> LogCallback;

    void setLogger (LogCallback logger);
    const char* severityName (Severity severity);
}
}