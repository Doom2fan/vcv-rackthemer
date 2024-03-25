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

#include "Common.hpp"
#include "RackTheme.hpp"

#include <rack.hpp>

namespace rack_themer {
    void handleThemeChange (rack::Widget* widget, std::shared_ptr<RackTheme> theme, bool topLevel);
    void performThemeRequest (rack::widget::Widget* parent);

    struct IThemedWidget {
        virtual void onThemeChanged (std::shared_ptr<RackTheme> theme) = 0;
    };

    struct IThemeHolder {
        virtual void requestTheme () = 0;
    };

    template<typename T = rack::widget::Widget>
    struct ThemedWidgetBase : T, IThemedWidget {
      public:
        typedef ThemedWidgetBase<T> _ThemedWidgetBase;

      protected:
        bool needTheme = true;

      public:
        void onAdd (const rack::event::Add& e) override {
            T::onAdd (e);
            needTheme = true;
        }

        void onRemove (const rack::event::Remove& e) override {
            T::onRemove (e);
            needTheme = true;
        }

        void step () override {
            if (needTheme) {
                performThemeRequest (this->parent);
                needTheme = false;
            }

            T::step ();
        }

        void onThemeChanged (std::shared_ptr<RackTheme> theme) override { needTheme = false; }
    };

    template<typename T = rack::widget::Widget>
    struct ThemeHolderWidgetBase : T, IThemeHolder {
      public:
        typedef ThemeHolderWidgetBase<T> _ThemeHolderWidgetBase;

      protected:
        bool themeRequested = true;

        virtual std::shared_ptr<RackTheme> getTheme () = 0;

      public:
        void requestTheme () override { themeRequested = true; }

        void step () override {
            T::step ();

            if (themeRequested) {
                handleThemeChange (this, getTheme (), true);
                themeRequested = false;
            }
        }
    };
}