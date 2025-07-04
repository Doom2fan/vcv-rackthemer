/*
 *  RackThemer
 *  Copyright (C) 2024 Chronos "phantombeta" Ouroboros
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
#include <unordered_map>

namespace rack_themer {
    struct ShapeInfo {
        KeyedString shapeId;
        KeyedString styleClass;
    };

    struct ThemeCache {
      private:
        std::unordered_map<std::string, std::shared_ptr<RackTheme>> themeCache = {};
        std::unordered_map<std::string, std::shared_ptr<ThemeableSvg>> svgCache = {};

        std::unordered_map<const NSVGshape*, ShapeInfo> shapeInfoMap;

        std::unordered_map<std::string, KeyedString> stringCache = {};
        std::unordered_map<KeyedString, std::string> keyStringMap = {};
        int nextStringKeyValue = 1;

        std::shared_ptr<RackTheme> createRackTheme (const std::string& path);
        std::shared_ptr<ThemeableSvg> createThemeableSvg (const std::string& path);

      public:
        std::shared_ptr<RackTheme> getRackTheme (const std::string& path);
        std::shared_ptr<ThemeableSvg> getSvg (const std::string& path);

        ShapeInfo getShapeInfo (const NSVGshape* shape);

        KeyedString getKeyedString (const std::string& text);
        std::string getKeyedStringText (const KeyedString& key);
    };

    extern ThemeCache themeCache;
}