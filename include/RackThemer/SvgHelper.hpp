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

        rack::math::Rect getShapeBoundsBox (NSVGshape* shape) {
            auto bounds = shape->bounds;
            return rack::math::Rect (
                bounds [0], bounds [1],
                bounds [2] - bounds [0], bounds [3] - bounds [1]
            );
        }

        rack::math::Vec getShapeBoundsCenter (NSVGshape* shape) {
            auto bounds = shape->bounds;
            return rack::math::Vec (
                (bounds [0] + bounds [2]) / 2,
                (bounds [1] + bounds [3]) / 2
            );
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

        void forEachPrefixed (const std::string& prefix, const std::function<void (unsigned int i, NSVGshape*)>& callback) {
            unsigned int i = 0;
            forEachShape ([&] (NSVGshape* shape) {
                if (getShapeId (shape).find (prefix) == 0)
                    callback (i++, shape);
            });
        }

        void forEachPrefixed (const std::string& prefix, const std::function<void (unsigned int i, rack::math::Rect)>& callback) {
            forEachPrefixed (prefix, [&] (unsigned int i, NSVGshape* shape) {
                callback (i, getShapeBoundsBox (shape));
            });
        }

        void forEachPrefixed (const std::string& prefix, const std::function<void (unsigned int i, rack::math::Vec)>& callback) {
            forEachPrefixed (prefix, [&] (unsigned int i, NSVGshape* shape) {
                callback (i, getShapeBoundsCenter (shape));
            });
        }

        void forEachMatched (const std::string& pattern, const std::function<void (std::vector<std::string>, NSVGshape*)>& callback) {
            if (svg.svg == nullptr)
                return;

            std::regex regex (pattern);
            forEachShape ([&] (NSVGshape* shape) {
                auto id = getShapeId (shape);
                std::vector<std::string> captures;
                std::smatch match;

                if (std::regex_search (id, match, regex)) {
                    for (unsigned int i = 1; i < match.size (); i++)
                        captures.push_back (match [i]);

                    callback (captures, shape);
                }
            });
        }

        void forEachMatched (const std::string& pattern, const std::function<void (std::vector<std::string>, rack::math::Rect)>& callback) {
            forEachMatched (pattern, [&] (std::vector<std::string> captures, NSVGshape* shape) {
                callback (captures, getShapeBoundsBox (shape));
            });
        }

        void forEachMatched (const std::string& pattern, const std::function<void (std::vector<std::string>, rack::math::Vec)>& callback) {
            forEachMatched (pattern, [&] (std::vector<std::string> captures, NSVGshape* shape) {
                callback (captures, getShapeBoundsCenter (shape));
            });
        }

        void findNamed (const std::string& name, const std::function<void (NSVGshape* shape)>& callback) {
            forEachShape ([&] (NSVGshape* shape) {
                if (getShapeId (shape) == name)
                    callback (shape);
            });
        }

        void findNamed (const std::string& name, const std::function<void (rack::math::Vec)>& callback) {
            findNamed (name, [&] (NSVGshape* shape) { callback (getShapeBoundsCenter (shape)); });
        }

        void findNamed (const std::string& name, const std::function<void (rack::math::Rect)>& callback) {
            findNamed (name, [&] (NSVGshape* shape) { callback (getShapeBoundsBox (shape)); });
        }

        std::optional<rack::math::Vec> findNamed (const std::string& name) {
            std::optional<rack::math::Vec> result;

            findNamed (name, [&] (rack::math::Vec center) {
                result = center;
            });

            return result;
        }

        std::optional<rack::math::Rect> findNamedBox (const std::string& name) {
            std::optional<rack::math::Rect> result;

            findNamed (name, [&] (rack::math::Rect box) {
                result = box;
            });

            return result;
        }

        std::vector<rack::math::Vec> findPrefixed (const std::string& prefix) {
            std::vector<rack::math::Vec> result;

            findPrefixed (prefix, [&] (rack::math::Vec center) {
                result.push_back (center);
            });

            return result;
        }

        std::vector<rack::math::Rect> findPrefixedBox (const std::string& prefix) {
            std::vector<rack::math::Rect> result;

            findPrefixed (prefix, [&] (rack::math::Rect box) {
                result.push_back (box);
            });

            return result;
        }

        auto findMatched (const std::string& pattern) {
            std::vector<std::pair<std::vector<std::string>, rack::math::Vec>> result;

            forEachMatched (pattern, [&] (std::vector<std::string> captures, rack::math::Vec center) {
                result.emplace_back (captures, center);
            });

            return result;
        }

        auto findMatchedBox (const std::string& pattern) {
            std::vector<std::pair<std::vector<std::string>, rack::math::Rect>> result;

            forEachMatched (pattern, [&] (std::vector<std::string> captures, rack::math::Rect box) {
                result.emplace_back (captures, box);
            });

            return result;
        }
    };
}