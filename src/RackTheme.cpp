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

#include "RackTheme.hpp"
#include "ThemeCache.hpp"

namespace rack_themer {
    std::shared_ptr<RackTheme> getNullTheme () { return themeCache.getRackTheme (""); }
    std::shared_ptr<RackTheme> loadRackTheme (const std::string& path) { return themeCache.getRackTheme (path); }

    std::shared_ptr<Style> RackTheme::getIdStyle (const KeyedString& name) const {
        if (auto found = idStyles.find (name); found != idStyles.end ())
            return found->second;

        return nullptr;
    }

    std::shared_ptr<Style> RackTheme::getClassStyle (const KeyedString& name) const {
        if (auto found = classStyles.find (name); found != classStyles.end ())
            return found->second;

        return nullptr;
    }

    Style Style::combineStyle (const Style& otherStyle) const {
        auto result = *this;

        if (otherStyle.hasFill ())
            result.setFill (otherStyle.getFill ());
        if (otherStyle.hasStroke ())
            result.setStroke (otherStyle.getStroke ());
        if (otherStyle.hasOpacity ())
            result.setOpacity (otherStyle.getOpacity ());
        if (otherStyle.hasStrokeWidth ())
            result.setStrokeWidth (otherStyle.getStrokeWidth ());
        if (otherStyle.hasStrokeLineCap ())
            result.setStrokeLineCap (otherStyle.getStrokeLineCap ());
        if (otherStyle.hasStrokeLineJoin ())
            result.setStrokeLineJoin (otherStyle.getStrokeLineJoin ());

        return result;
    }

    void handleThemeChange (rack::Widget* widget, std::shared_ptr<RackTheme> theme, bool topLevel) {
        auto themedWidget = dynamic_cast<IThemedWidget*> (widget);
        if (themedWidget != nullptr)
            themedWidget->onThemeChanged (theme);

        for (auto child : widget->children) {
            auto themedChild = dynamic_cast<IThemedWidget*> (child);
            if (themedChild != nullptr)
                themedChild->onThemeChanged (theme);

            if (!child->children.empty ())
                handleThemeChange (child, theme, false);
        }

        if (topLevel) {
            rack::EventContext cDirty;
            rack::Widget::DirtyEvent eDirty;
            eDirty.context = &cDirty;
            widget->onDirty (eDirty);
        }
    }
}