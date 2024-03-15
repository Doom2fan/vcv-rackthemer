/*
 *  RackThemer
 *  Copyright (C) 2024 Chronos "phantombeta" Ouroboros
 *  Copyright (C) 2016-2023 VCV
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

#include "KeyedString.hpp"
#include "RackTheme.hpp"

#include <rack.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace rack_themer {
    struct ThemeCache;

    struct ThemeableSvg {
        friend ThemeCache;

      private:
        NSVGimage* handle = nullptr;

      public:
        rack::math::Vec getSize ();
        int getNumShapes ();
        int getNumPaths ();
        int getNumPoints ();
        void draw (NVGcontext* vg, std::shared_ptr<RackTheme> theme);

        /*
         * FOR INTERNAL USE ONLY! DO NOT USE!
         */
        void forEachShape (const std::function<void (NSVGshape*)>& callback);
    };

    std::string getShapeId (const NSVGshape* shape);
    std::shared_ptr<ThemeableSvg> loadSvg (const std::string& path);
}