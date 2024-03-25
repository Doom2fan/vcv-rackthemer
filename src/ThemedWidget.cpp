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

#include "rack_themer.hpp"

namespace rack_themer {
    void handleThemeChange (rack::Widget* widget, std::shared_ptr<RackTheme> theme, bool topLevel) {
        auto themedWidget = dynamic_cast<IThemedWidget*> (widget);
        if (themedWidget != nullptr)
            themedWidget->onThemeChanged (theme);

        for (auto child : widget->children)
            handleThemeChange (child, theme, false);

        if (topLevel) {
            rack::EventContext cDirty;
            rack::Widget::DirtyEvent eDirty;
            eDirty.context = &cDirty;
            widget->onDirty (eDirty);
        }
    }

    void performThemeRequest (rack::widget::Widget* parent) {
        while (parent != nullptr) {
            if (auto parentHolder = dynamic_cast<IThemeHolder*> (parent)) {
                parentHolder->requestTheme ();
                break;
            }

            parent = parent->parent;
        }
    }
}