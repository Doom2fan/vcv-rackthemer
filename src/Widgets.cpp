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

#include "Widgets.hpp"

namespace rack_themer {
namespace widgets {
    void SvgWidget::onThemeChanged (std::shared_ptr<rack_themer::RackTheme> theme) {
        if (autoSwitchTheme)
            svg = svg.withTheme (theme);
    }

    SvgPanel::SvgPanel () : svg (nullptr, nullptr) {
        fb = new rack::widget::FramebufferWidget;
        addChild (fb);

        sw = new SvgWidget;
        fb->addChild (sw);

        panelBorder = new rack::app::PanelBorder;
        fb->addChild (panelBorder);
    }


    void SvgPanel::step () {
        if (APP->window->pixelRatio < 2.0)
            // Small details draw poorly at low DPI, so oversample when drawing to the framebuffer
            fb->oversample = 2.0;
        else
            fb->oversample = 1.0;

        Widget::step ();
    }


    void SvgPanel::setBackground (ThemedSvg svg) {
        if (svg == this->svg)
            return;

        this->svg = svg;

        sw->setSvg (svg);

        // Round framebuffer size to nearest grid
        fb->box.size = sw->box.size.div (rack::app::RACK_GRID_SIZE).round ().mult (rack::app::RACK_GRID_SIZE);
        panelBorder->box.size = fb->box.size;
        box.size = fb->box.size;

        fb->setDirty ();
    }

    void SvgPanel::onThemeChanged (std::shared_ptr<rack_themer::RackTheme> theme) {
        if (autoSwitchTheme)
            svg = svg.withTheme (theme);
    }
}
}