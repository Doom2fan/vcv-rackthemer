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
#include "ThemedSvg.hpp"
#include "ThemedWidget.hpp"

#include <rack.hpp>

#include <memory>

namespace rack_themer {
namespace widgets {
    struct SvgWidget : rack::widget::Widget, IThemedWidget {
        ThemedSvg svg;
        bool autoSwitchTheme = true;

        SvgWidget () : svg (nullptr, nullptr) { box.size = rack::math::Vec (); }

        void wrap () { box.size = svg.getSize (); }
        void setSvg (std::shared_ptr<ThemeableSvg> svg) { setSvg (this->svg.withSvg (svg)); }
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
        void setBackground (std::shared_ptr<ThemeableSvg> svg) { setBackground (svgWidget->svg.withSvg (svg)); }
        void setBackground (ThemedSvg svg);
    };

    struct SvgPort : rack::app::PortWidget {
        rack::widget::FramebufferWidget* framebuffer;
        rack::app::CircularShadow* shadow;
        SvgWidget* svgWidget;

        SvgPort ();

        void setSvg (std::shared_ptr<ThemeableSvg> svg) { setSvg (svgWidget->svg.withSvg (svg)); }
        void setSvg (ThemedSvg svg);
    };

    struct SvgScrew : rack::widget::Widget {
        rack::widget::FramebufferWidget* framebuffer;
        SvgWidget* svgWidget;

        SvgScrew ();

        void setSvg (std::shared_ptr<ThemeableSvg> svg) { setSvg (svgWidget->svg.withSvg (svg)); }
        void setSvg (ThemedSvg svg);
    };

    struct SvgButton : rack::widget::OpaqueWidget {
        rack::widget::FramebufferWidget* framebuffer;
        rack::app::CircularShadow* shadow;
        SvgWidget* svgWidget;
        std::vector<std::shared_ptr<ThemeableSvg>> frames;

        SvgButton ();

        void addFrame (std::shared_ptr<ThemeableSvg> svg);
        void onButton (const ButtonEvent& e) override;
        void onDragStart (const DragStartEvent& e) override;
        void onDragEnd (const DragEndEvent& e) override;
        void onDragDrop (const DragDropEvent& e) override;
    };

    struct SvgSwitch : rack::app::Switch {
        rack::widget::FramebufferWidget* framebuffer;
        rack::app::CircularShadow* shadow;
        SvgWidget* svgWidget;
        std::vector<std::shared_ptr<ThemeableSvg>> frames;

        /** Use frames 0 and 1 when the mouse is pressed and released, instead of using the param value as the frame index. */
        bool latch = false;

        SvgSwitch ();
        ~SvgSwitch ();
        /** Adds an SVG file to represent the next switch position */
        void addFrame (std::shared_ptr<ThemeableSvg> svg);

        void onDragStart (const DragStartEvent& e) override;
        void onDragEnd (const DragEndEvent& e) override;
        void onChange (const ChangeEvent& e) override;
    };
}
}