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

#include "ThemedSvg.hpp"

#include <rack.hpp>

#include <memory>

namespace rack_themer {
namespace widgets {
    struct SvgWidget : rack::widget::Widget, IThemedWidget {
        ThemedSvg svg;
        bool autoSwitchTheme = true;

        SvgWidget () : svg (nullptr, nullptr) { box.size = rack::math::Vec (); }
        void wrap () { box.size = svg.getSize (); }
        void setSvg (ThemedSvg svg) {
            this->svg = svg;
            wrap ();
        }
        void draw (const DrawArgs& args) override { svg.draw (args.vg); }

        void onThemeChanged (std::shared_ptr<rack_themer::RackTheme> theme) override;
    };

    struct SvgPanel : rack::widget::Widget {
        rack::widget::FramebufferWidget* framebuffer;
        SvgWidget* svgWidget;
        rack::app::PanelBorder* panelBorder;

        SvgPanel ();

        void step () override;
        void setBackground (std::shared_ptr<ThemeableSvg> svg) { setBackground (ThemedSvg (svg, svgWidget->svg.theme)); }
        void setBackground (ThemedSvg svg);
    };

    struct SvgPort : rack::app::PortWidget {
        rack::widget::FramebufferWidget* framebuffer;
        rack::app::CircularShadow* shadow;
        SvgWidget* svgWidget;

        SvgPort ();
        void setSvg (ThemedSvg svg);
    };
}
}