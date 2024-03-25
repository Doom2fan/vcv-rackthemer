/*
 *  RackThemer
 *  Copyright (C) 2024 Chronos "phantombeta" Ouroboros
 *  Copyright (C) 2023 Dustin Lacewell [vcv-svghelper]
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
#include "ThemeableSvg.hpp"
#include "ThemedSvg.hpp"
#include "Widgets.hpp"

#include <nanosvg.h>
#include <rack.hpp>

#include <optional>
#include <regex>

namespace rack_themer {
    template<class TPanel = widgets::SvgPanel>
    TPanel* createPanel (ThemedSvg svg) {
        auto panel = new TPanel;
        panel->setBackground (svg);
        return panel;
    }

    template<typename T>
    struct SvgHelper {
    private:
        ThemedSvg svg;

        rack::app::ModuleWidget* moduleWidget () {
            auto t = static_cast<T*> (this);
            return static_cast<rack::app::ModuleWidget*> (t);
        }

    public:
        SvgHelper () : svg (nullptr, nullptr) { }

        void loadPanel (ThemedSvg svg) {
            auto panel = static_cast<widgets::SvgPanel*> (moduleWidget ()->getPanel ());
            if (panel == nullptr) {
                panel = createPanel (svg);
                moduleWidget ()->setPanel (panel);
            } else
                panel->setBackground (svg);

            this->svg = svg;
        }

        void loadPanel (const std::string& filename, std::shared_ptr<RackTheme> theme) {
            loadPanel (ThemedSvg (loadSvg (filename), theme));
        }

        void setTheme (std::shared_ptr<RackTheme> theme) { loadPanel (svg.withTheme (theme)); }

        void forEachShape (const std::function<void (NSVGshape*)>& callback) {
            if (svg.svg == nullptr)
                return;

            svg.svg->forEachShape (callback);
        }

        std::optional<rack::math::Vec> findNamed (std::string name) {
            std::optional<rack::math::Vec> result;

            if (svg.svg == nullptr)
                return result;

            forEachShape ([&] (NSVGshape* shape) {
                if (getShapeId (shape) == name) {
                    auto bounds = shape->bounds;
                    result = rack::math::Vec (
                        (bounds [0] + bounds [2]) / 2,
                        (bounds [1] + bounds [3]) / 2
                    );
                    return;
                }
            });

            return result;
        }

        std::vector<rack::math::Vec> findPrefixed (std::string prefix) {
            std::vector<rack::math::Vec> result;

            if (svg.svg == nullptr)
                return result;

            forEachShape ([&] (NSVGshape* shape) {
                if (getShapeId (shape).find (prefix) == 0) {
                    auto bounds = shape->bounds;
                    auto center = rack::math::Vec (
                        (bounds [0] + bounds [2]) / 2,
                        (bounds [1] + bounds [3]) / 2
                    );
                    result.push_back (center);
                }
            });

            return result;
        }

        std::vector<std::pair<std::vector<std::string>, rack::math::Vec>> findMatched (const std::string& pattern) {
            std::vector<std::pair<std::vector<std::string>, rack::math::Vec>> result;

            if (svg.svg == nullptr)
                return result;

            std::regex regex (pattern);

            forEachShape ([&] (NSVGshape* shape) {
                auto id = getShapeId (shape);
                std::vector<std::string> captures;
                std::smatch match;

                if (std::regex_search (id, match, regex)) {
                    for (unsigned int i = 1; i < match.size (); i++)
                        captures.push_back (match [i]);

                    auto bounds = shape->bounds;
                    auto center = rack::math::Vec (
                        (bounds [0] + bounds [2]) / 2,
                        (bounds [1] + bounds [3]) / 2
                    );
                    result.emplace_back (captures, center);
                }
            });

            return result;
        }

        void forEachPrefixed (std::string prefix, const std::function<void (unsigned int i, rack::math::Vec)>& callback) {
            if (svg.svg == nullptr)
                return;

            auto positions = findPrefixed (prefix);
            for (unsigned int i = 0; i < positions.size (); i++)
                callback (i, positions [i]);
        }

        void forEachMatched (const std::string& regex, const std::function<void (std::vector<std::string>, rack::math::Vec)>& callback) {
            if (svg.svg == nullptr)
                return;

            auto matches = findMatched (regex);
            for (const auto& match : matches)
                callback (match.first, match.second);
        }
    };
}