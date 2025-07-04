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
            if (!frames.empty () && pq != nullptr) {
                auto index = (int) std::round (pq->getValue () - pq->getMinValue ());
                index = rack::math::clamp (index, 0, (int) frames.size () - 1);
                svgWidget->setSvg (frames [index]);
                framebuffer->setDirty ();
            }
        }
        ParamWidget::onChange (e);
    }

    /*
     * SvgKnob
     */
    SvgKnob::SvgKnob () {
        framebuffer = new rack::widget::FramebufferWidget;
        addChild (framebuffer);

        shadow = new rack::app::CircularShadow;
        framebuffer->addChild (shadow);
        shadow->box.size = rack::math::Vec ();

        transformWidget = new rack::widget::TransformWidget;
        framebuffer->addChild (transformWidget);

        svgWidget = new SvgWidget;
        transformWidget->addChild (svgWidget);
    }

    void SvgKnob::setSvg (ThemedSvg svg) {
        if (svg == svgWidget->svg)
            return;

        svgWidget->setSvg (svg);
        transformWidget->box.size = svgWidget->box.size;
        framebuffer->box.size = svgWidget->box.size;
        box.size = svgWidget->box.size;

        shadow->box.size = svgWidget->box.size;
        // Move shadow downward by 10%.
        shadow->box.pos = rack::math::Vec (0, svgWidget->box.size.y * .10f);

        framebuffer->setDirty ();
    }

    void SvgKnob::onChange (const ChangeEvent& e) {
        auto angle = 0.f;

        // Calculate angle from value
        if (auto pq = getParamQuantity ()) {
            auto value = pq->getValue ();
            if (!pq->isBounded ()) // Number of rotations equals value for unbounded range.
                angle = value * (2 * M_PI);
            else if (pq->getRange () == 0.f) // Center angle for zero range.
                angle = (minAngle + maxAngle) / 2.f;
            else // Proportional angle for finite range.
                angle = rack::math::rescale (value, pq->getMinValue (), pq->getMaxValue (), minAngle, maxAngle);

            angle = std::fmod (angle, 2 * M_PI);
        }

        transformWidget->identity ();
        // Rotate SVG.
        auto center = svgWidget->box.getCenter ();
        transformWidget->translate (center);
        transformWidget->rotate (angle);
        transformWidget->translate (center.neg ());
        framebuffer->setDirty ();

        Knob::onChange (e);
    }

    /*
     * SvgSlider
     */
    SvgSlider::SvgSlider () {
        framebuffer = new rack::widget::FramebufferWidget;
        addChild (framebuffer);

        background = new SvgWidget;
        framebuffer->addChild (background);

        handle = new SvgWidget;
        framebuffer->addChild (handle);

        speed = 2.f;
    }

    void SvgSlider::setBackgroundSvg (ThemedSvg svg) {
        if (svg == background->svg)
            return;

        background->setSvg (svg);
        box.size = background->box.size;
        framebuffer->box.size = background->box.size;
        framebuffer->setDirty ();
    }

    void SvgSlider::setHandleSvg (ThemedSvg svg) {
        if (svg == handle->svg)
            return;

        handle->setSvg (svg);
        handle->box.pos = maxHandlePos;
        framebuffer->setDirty ();
    }

    void SvgSlider::setHandlePos (rack::math::Vec minHandlePos, rack::math::Vec maxHandlePos) {
        this->minHandlePos = minHandlePos;
        this->maxHandlePos = maxHandlePos;

        // Set handle pos to maximum by default.
        handle->box.pos = maxHandlePos;
    }

    void SvgSlider::setHandlePosCentered (rack::math::Vec minHandlePosCentered, rack::math::Vec maxHandlePosCentered) {
        setHandlePos (
            minHandlePosCentered.minus (handle->box.size.div (2)),
            maxHandlePosCentered.minus (handle->box.size.div (2))
        );
    }

    void SvgSlider::onChange (const ChangeEvent& e) {
        // Default position is max value.
        auto v = 1.f;
        if (auto pq = getParamQuantity ())
            v = pq->getScaledValue ();

        // Interpolate handle position.
        handle->box.pos = minHandlePos.crossfade (maxHandlePos, v);
        framebuffer->setDirty ();

        ParamWidget::onChange (e);
    }
}
}