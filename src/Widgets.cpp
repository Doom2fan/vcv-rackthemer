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

#include "rack_themer.hpp"

namespace rack_themer {
namespace widgets {
    /*
     * SvgWidget
     */
    void SvgWidget::onThemeChanged (std::shared_ptr<rack_themer::RackTheme> theme) {
        if (autoSwitchTheme)
            svg = svg.withTheme (theme);
    }

    /*
     * SvgPanel
     */
    SvgPanel::SvgPanel () {
        framebuffer = new rack::widget::FramebufferWidget;
        addChild (framebuffer);

        svgWidget = new SvgWidget;
        framebuffer->addChild (svgWidget);

        panelBorder = new rack::app::PanelBorder;
        framebuffer->addChild (panelBorder);
    }

    void SvgPanel::step () {
        // Small details draw poorly at low DPI, so oversample when drawing to the framebuffer.
        framebuffer->oversample = APP->window->pixelRatio < 2.0 ? 2.0 : 1.0;

        Widget::step ();
    }

    void SvgPanel::setBackground (ThemedSvg svg) {
        if (svg == svgWidget->svg)
            return;

        svgWidget->setSvg (svg);

        // Round framebuffer size to nearest grid.
        framebuffer->box.size = svgWidget->box.size.div (rack::app::RACK_GRID_SIZE).round ().mult (rack::app::RACK_GRID_SIZE);
        panelBorder->box.size = framebuffer->box.size;
        box.size = framebuffer->box.size;

        framebuffer->setDirty ();
    }

    /*
     * SvgPort
     */
    SvgPort::SvgPort () {
        framebuffer = new rack::widget::FramebufferWidget;
        addChild (framebuffer);

        shadow = new rack::app::CircularShadow;
        framebuffer->addChild (shadow);
        // Avoid breakage if plugins fail to call `setSvg ()`
        // In that case, just disable the shadow.
        shadow->box.size = rack::math::Vec ();

        svgWidget = new SvgWidget;
        framebuffer->addChild (svgWidget);
    }

    void SvgPort::setSvg (ThemedSvg svg) {
        if (svg == svgWidget->svg)
            return;

        svgWidget->setSvg (svg);
        box.size = framebuffer->box.size = svgWidget->box.size;

        // Move shadow downward by 10%.
        shadow->box.size = svgWidget->box.size;
        shadow->box.pos = rack::math::Vec (0, svgWidget->box.size.y * .1f);

        framebuffer->setDirty ();
    }

    /*
     * SvgScrew
     */
    SvgScrew::SvgScrew () {
        framebuffer = new rack::widget::FramebufferWidget;
        addChild (framebuffer);

        svgWidget = new SvgWidget;
        framebuffer->addChild (svgWidget);
    }

    void SvgScrew::setSvg (ThemedSvg svg) {
        if (svg == svgWidget->svg)
            return;

        svgWidget->setSvg (svg);
        framebuffer->box.size = svgWidget->box.size;
        box.size = svgWidget->box.size;

        framebuffer->setDirty ();
    }

    /*
     * SvgButton
     */
    SvgButton::SvgButton () {
        framebuffer = new rack::widget::FramebufferWidget;
        addChild (framebuffer);

        shadow = new rack::app::CircularShadow;
        framebuffer->addChild (shadow);
        shadow->box.size = rack::math::Vec ();

        svgWidget = new SvgWidget;
        framebuffer->addChild (svgWidget);
    }

    void SvgButton::onButton (const ButtonEvent& e) {
        OpaqueWidget::onButton (e);

        // Dispatch ActionEvent on left click.
        if (e.action == GLFW_PRESS && e.button == GLFW_MOUSE_BUTTON_LEFT) {
            ActionEvent eAction;
            onAction (eAction);
        }
    }

    void SvgButton::addFrame (std::shared_ptr<ThemeableSvg> svg) {
        frames.push_back (svg);

        // If this is our first frame, automatically set SVG and size.
        if (svgWidget->svg.svg == nullptr) {
            svgWidget->setSvg (svg);
            box.size = svgWidget->box.size;
            framebuffer->box.size = svgWidget->box.size;
            // Move shadow downward by 10%.
            shadow->box.size = svgWidget->box.size;
            shadow->box.pos = rack::math::Vec (0, svgWidget->box.size.y * .10f);
            framebuffer->setDirty ();
        }
    }

    void SvgButton::onDragStart (const DragStartEvent& e) {
        if (e.button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        if (frames.size () >= 2) {
            svgWidget->setSvg (frames [1]);
            framebuffer->setDirty ();
        }
    }

    void SvgButton::onDragEnd (const DragEndEvent& e) {
        if (frames.size () >= 1) {
            svgWidget->setSvg (frames [0]);
            framebuffer->setDirty ();
        }
    }

    void SvgButton::onDragDrop (const DragDropEvent& e) {
        // Don't dispatch ActionEvent on DragDrop because it's already called on mouse down.
    }

    /*
     * SvgSwitch
     */
    SvgSwitch::SvgSwitch () {
        framebuffer = new rack::widget::FramebufferWidget;
        addChild (framebuffer);

        shadow = new rack::app::CircularShadow;
        framebuffer->addChild (shadow);
        shadow->box.size = rack::math::Vec ();

        svgWidget = new SvgWidget;
        framebuffer->addChild (svgWidget);
    }

    SvgSwitch::~SvgSwitch () {
    }

    void SvgSwitch::addFrame (std::shared_ptr<ThemeableSvg> svg) {
        frames.push_back (svg);

        // If this is our first frame, automatically set SVG and size.
        if (svgWidget->svg.svg == nullptr) {
            svgWidget->setSvg (svg);
            box.size = svgWidget->box.size;
            framebuffer->box.size = svgWidget->box.size;
            // Move shadow downward by 10%.
            shadow->box.size = svgWidget->box.size;
            shadow->box.pos = rack::math::Vec (0, svgWidget->box.size.y * .10f);
            framebuffer->setDirty ();
        }
    }

    void SvgSwitch::onDragStart (const DragStartEvent& e) {
        Switch::onDragStart (e);

        if (e.button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        // Set down frame if latch
        if (latch && frames.size () >= 2) {
            svgWidget->setSvg (frames [1]);
            framebuffer->setDirty ();
        }
    }


    void SvgSwitch::onDragEnd (const DragEndEvent& e) {
        Switch::onDragEnd (e);

        if (e.button != GLFW_MOUSE_BUTTON_LEFT)
            return;

        // Set up frame if latch
        if (latch) {
            if (frames.size () >= 1) {
                svgWidget->setSvg (frames [0]);
                framebuffer->setDirty ();
            }
        }
    }


    void SvgSwitch::onChange (const ChangeEvent& e) {
        if (!latch) {
            auto pq = getParamQuantity ();
            if (!frames.empty () && pq) {
                auto index = (int) std::round (pq->getValue () - pq->getMinValue ());
                index = rack::math::clamp (index, 0, (int) frames.size () - 1);
                svgWidget->setSvg (frames [index]);
                framebuffer->setDirty ();
            }
        }
        ParamWidget::onChange (e);
    }
}
}