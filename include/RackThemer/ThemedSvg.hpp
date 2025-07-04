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

#include "Common.hpp"
#include "RackTheme.hpp"
#include "ThemeableSvg.hpp"

#include <memory>

namespace rack_themer {
    struct ThemedSvg {
      public:
        std::shared_ptr<ThemeableSvg> svg = nullptr;
        std::shared_ptr<RackTheme> theme = nullptr;

        ThemedSvg (std::shared_ptr<ThemeableSvg> svg, std::shared_ptr<RackTheme> theme) {
            this->svg = svg;
            this->theme = theme;
        }

        bool operator== (const ThemedSvg& rhs) const { return svg == rhs.svg && theme == rhs.theme; }
        bool isValid () const { return svg != nullptr && theme != nullptr; }
        ThemedSvg withSvg (std::shared_ptr<ThemeableSvg> svg) { return ThemedSvg (svg, theme); }
        ThemedSvg withTheme (std::shared_ptr<RackTheme> theme) { return ThemedSvg (svg, theme); }

        /*
         * ThemeableSvg passthroughs.
         */
        rack::math::Vec getSize () { return svg != nullptr ? svg->getSize () : 0; }
        int getNumShapes () { return svg != nullptr ? svg->getNumShapes () : 0; }
        int getNumPaths () { return svg != nullptr ? svg->getNumPaths () : 0; }
        int getNumPoints () { return svg != nullptr ? svg->getNumPoints () : 0; }

        void draw (NVGcontext* vg) {
            WARN ("=== ThemedSvg::draw A");
            if (svg == nullptr || theme == nullptr)
                return;

            WARN ("=== ThemeableSvg::draw Path=%s", svg->path.c_str ());
            WARN ("=== ThemedSvg::draw B");
            svg->draw (vg, theme);
            WARN ("=== ThemedSvg::draw C");
        }
    };
}