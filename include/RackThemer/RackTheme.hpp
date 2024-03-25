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
#include "KeyedString.hpp"

#include <rack.hpp>

#include <memory>
#include <string>
#include <unordered_map>

namespace rack_themer {
    struct GradientStop {
        int index = -1;
        float offset = 0.f;
        NVGcolor color = NVGcolor ();

        GradientStop () {}
        GradientStop (int i, float off, NVGcolor co) : index (i), offset (off), color (co) { }
    };

    struct Gradient {
        int nstops = 0;
        GradientStop stops [2];
    };

    enum class PaintKind {
        Unset,
        Color,
        Gradient,
        None
    };

    class Paint {
      private:
        PaintKind kind = PaintKind::Unset;

        union {
            NVGcolor color;
            Gradient gradient;
        };

      public:
        Paint () {}
        Paint (NVGcolor color) { setColor (color); }
        Paint (const Gradient& gradient) { setGradient (gradient); }

        static Paint makeUnset () { Paint p; p.unset (); return p; }
        static Paint makeNone () { Paint p; p.setNone (); return p; }
        static Paint makeColor (NVGcolor color) { Paint p; p.setColor (color); return p; }
        static Paint makeGradient (const Gradient& gradient) { Paint p; p.setGradient (gradient); return p; }

        PaintKind Kind () const { return kind; }
        void unset () { kind = PaintKind::Unset; }
        void setNone () { kind = PaintKind::None; }
        void setColor (NVGcolor newColor) {
            kind = PaintKind::Color;
            color = newColor;
        }
        void setGradient (const Gradient& g) {
            kind = PaintKind::Gradient;
            gradient = g;
        }

        bool isApplicable () const { return kind != PaintKind::Unset; }
        bool isColor () const { return kind == PaintKind::Color; }
        bool isGradient () const { return kind == PaintKind::Gradient; }
        bool isNone ()  const { return kind == PaintKind::None; }

        NVGcolor getColor () const { return isColor () ? color : rack::color::BLACK; }
        const Gradient* getGradient () const { return isGradient () ? &gradient : nullptr; }

        NVGpaint getNVGPaint (const NVGpaint& basePaint) const;
    };

    struct Style {
      private:
        Paint fill;
        Paint stroke;

        bool _hasOpacity = false;
        float opacity = 1.f;

        bool _hasStrokeWidth = false;
        float strokeWidth = 1.f;

        bool _hasStrokeLineCap = false;
        NVGlineCap strokeLineCap = (NVGlineCap) 0;

        bool _hasStrokeLineJoin = false;
        int strokeLineJoin = 0;

      public:
        void setFill (Paint paint) { fill = paint; }
        void setStroke (Paint paint) { stroke = paint; }
        void setOpacity (float alpha) {
            opacity = alpha;
            _hasOpacity = true;
        }
        void setStrokeWidth (float width) {
            strokeWidth = width;
            _hasStrokeWidth = true;
        }
        void setStrokeLineCap (NVGlineCap lineCap) {
            strokeLineCap = lineCap;
            _hasStrokeLineCap = true;
        }
        void setStrokeLineJoin (int lineJoin) {
            strokeLineJoin = lineJoin;
            _hasStrokeLineJoin = true;
        }

        void unsetFill () { fill.unset (); }
        void unsetStroke () { stroke.unset (); }
        void unsetOpacity () { _hasOpacity = false; }
        void unsetStrokeWidth () { _hasStrokeWidth = false; }
        void unsetStrokeLineCap () { _hasStrokeLineCap = false; }
        void unsetStrokeLineJoin () { _hasStrokeLineJoin = false; }

        bool hasFill () const { return fill.isApplicable (); }
        bool hasStroke () const { return stroke.isApplicable (); }
        bool hasOpacity () const { return _hasOpacity; }
        bool hasStrokeWidth () const { return _hasStrokeWidth; }
        bool hasStrokeLineCap () const { return _hasStrokeLineCap; }
        bool hasStrokeLineJoin () const { return _hasStrokeLineJoin; }

        const Paint& getFill () const { return fill; }
        const Paint& getStroke () const { return stroke; }
        float getOpacity () const { return hasOpacity () ? opacity : 1.f; }
        float getStrokeWidth () const { return hasStrokeWidth () ? strokeWidth : 1.f; }
        NVGlineCap getStrokeLineCap () const { return hasStrokeLineCap () ? strokeLineCap : NVG_BUTT; }
        int getStrokeLineJoin () const { return hasStrokeLineJoin () ? strokeLineJoin : 0; }

        Style combineStyle (const std::shared_ptr<Style>& otherStyle) const { return combineStyle (*otherStyle); }
        Style combineStyle (const Style& otherStyle) const;
    };

    struct ThemeLoader;
    struct RackTheme {
        friend ThemeLoader;

      private:
        std::string name;
        std::unordered_map<KeyedString, std::shared_ptr<Style>> classStyles;
        std::unordered_map<KeyedString, std::shared_ptr<Style>> idStyles;

      public:
        std::string getName () const { return name; }
        std::shared_ptr<Style> getIdStyle (const KeyedString& name) const;
        std::shared_ptr<Style> getClassStyle (const KeyedString& name) const;
    };

    std::shared_ptr<RackTheme> getNullTheme ();
    std::shared_ptr<RackTheme> loadRackTheme (const std::string& path);
}