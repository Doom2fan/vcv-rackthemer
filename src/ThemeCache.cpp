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

#include "ThemeCache.hpp"
#include "rack_themer.hpp"
#include "ThemeLoader.hpp"

namespace rack_themer {
    ThemeCache themeCache;

    std::shared_ptr<RackTheme> ThemeCache::createRackTheme (std::string path) {
        if (path.empty ()) {
            auto nullTheme = std::make_shared<RackTheme> ();
            themeCache [path] = nullTheme;
            return nullTheme;
        }

        auto theme = themeLoader.loadTheme (path);
        if (theme == nullptr)
            return nullptr;

        themeCache [path] = theme;

        return theme;
    }

    std::shared_ptr<ThemeableSvg> ThemeCache::createThemeableSvg (std::string path) {
        auto handle = nsvgParseFromFile (path.c_str (), "px", rack::window::SVG_DPI);
        if (!handle) {
            WARN ("Failed to load SVG %s", path.c_str ());
            return nullptr;
        }

        INFO ("Loaded SVG %s", path.c_str ());

        auto svg = std::make_shared<ThemeableSvg> ();
        svg->handle = handle;
        svgCache [path] = svg;

        return svg;
    }

    std::shared_ptr<RackTheme> ThemeCache::getRackTheme (const std::string& path) {
        if (auto themeSearch = themeCache.find (path); themeSearch != themeCache.end ())
            return themeSearch->second;

        return createRackTheme (path);
    }

    std::shared_ptr<ThemeableSvg> ThemeCache::getSvg (const std::string& path) {
        if (auto svgSearch = svgCache.find (path); svgSearch != svgCache.end ())
            return svgSearch->second;

        return createThemeableSvg (path);
    }

    ShapeInfo ThemeCache::getShapeInfo (const NSVGshape* shape) {
        if (auto infoSearch = shapeInfoMap.find (shape); infoSearch != shapeInfoMap.end ())
            return infoSearch->second;

        auto id = std::string (shape->id);
        auto dashes = id.rfind ("--");

        auto shapeId = dashes != std::string::npos ? id.substr (0, dashes) : id;
        auto styleClass = dashes != std::string::npos ? id.substr (dashes + 2) : "";

        ShapeInfo info;
        info.shapeId = getKeyedString (shapeId);
        info.styleClass = getKeyedString (styleClass);

        shapeInfoMap [shape] = info;
        return info;
    }

    KeyedString ThemeCache::getKeyedString (const std::string& text) {
        if (auto keySearch = stringCache.find (text); keySearch != stringCache.end ())
            return keySearch->second;

        KeyedString key;
        key.value = nextStringKeyValue++;
        stringCache [text] = key;
        keyStringMap [key] = text;
        return key;
    }

    std::string ThemeCache::getKeyedStringText (const KeyedString& key) {
        if (auto stringSearch = keyStringMap.find (key); stringSearch != nullptr)
            return stringSearch->second;

        return nullptr;
    }
}