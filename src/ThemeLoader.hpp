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

#include "rack_themer.hpp"

#include <rack.hpp>

#include <memory>
#include <string>

namespace rack_themer {
    struct ThemeLoader {
      private:
        logging::LogCallback logger;

      public:
        ThemeLoader ();

        // Set a logging callback to receive more detailed information, warnings,
        // and errors when working with svg themes.
        void setLogger (logging::LogCallback logger) { this->logger = logger; }

        std::shared_ptr<RackTheme> loadTheme (std::string filePath);

      private:
        void logInfo (std::string info) { logger (logging::Severity::Info, logging::ErrorCode::NoError, info); }
        void logError (logging::ErrorCode code, std::string info) { logger (logging::Severity::Error, code, info); }
        void logWarning (logging::ErrorCode code, std::string info) { logger (logging::Severity::Warn, code, info); }

        bool requireValidHexColor (std::string hex, const char* name);
        bool requireArray (json_t* j, const char* name);
        bool requireObject (json_t* j, const char* name);
        bool requireObjectOrString (json_t* j, const char* name);
        bool requireString (json_t* j, const char* name);
        bool requireNumber (json_t* j, const char* name);
        bool requireInteger (json_t* j, const char* name);

        bool parseGradient (json_t* root, Gradient& gradient);
        bool parseFill (json_t* root, std::shared_ptr<Style>);
        bool parseStroke (json_t* root, std::shared_ptr<Style>);
        bool parseOpacity (json_t* root, std::shared_ptr<Style>);
        bool parseStyle (const char* name, json_t* root, std::shared_ptr<RackTheme> theme);
        bool parseTheme (json_t* root, std::shared_ptr<RackTheme>& theme);
    };

    extern ThemeLoader themeLoader;
}