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
#include "ThemeCache.hpp"

namespace rack_themer {
    std::shared_ptr<ThemeableSvg> loadSvg (const std::string& path) { return themeCache.getSvg (path); }
    std::string getShapeId (const NSVGshape* shape) {
        if (shape != nullptr)
            return getKeyedStringText (themeCache.getShapeInfo (shape).shapeId);
        else
            return "";
    }

    rack::math::Vec ThemeableSvg::getSize () {
        if (handle == nullptr)
            return rack::math::Vec ();

        return rack::math::Vec (handle->width, handle->height);
    }

    int ThemeableSvg::getNumShapes () {
        if (handle == nullptr)
            return 0;

        int count = 0;
        for (auto shape = handle->shapes; shape != nullptr; shape = shape->next)
            count++;

        return count;
    }

    int ThemeableSvg::getNumPaths () {
        if (handle == nullptr)
            return 0;

        int count = 0;
        for (auto shape = handle->shapes; shape != nullptr; shape = shape->next) {
            for (auto path = shape->paths; path != nullptr; path = path->next)
                count++;
        }

        return count;
    }

    int ThemeableSvg::getNumPoints () {
        if (handle == nullptr)
            return 0;

        int count = 0;
        for (auto shape = handle->shapes; shape != nullptr; shape = shape->next) {
            for (auto path = shape->paths; path != nullptr; path = path->next)
                count += path->npts / 3;
        }

        return count;
    }

    void ThemeableSvg::forEachShape (const std::function<void (NSVGshape*)>& callback) {
        if (handle == nullptr)
            return;

        auto shapes = handle->shapes;
        for (NSVGshape* shape = shapes; shape != nullptr; shape = shape->next)
            callback (shape);
    }

    static NVGcolor getNVGColor (uint32_t color) {
        return nvgRGBA (
            (color >> 0) & 0xff,
            (color >> 8) & 0xff,
            (color >> 16) & 0xff,
            (color >> 24) & 0xff
        );
    }

    static NVGpaint getGradient (NVGcontext* vg, NSVGpaint* paint, const Paint& stylePaint) {
        assert (paint != nullptr);
        assert (paint->type == NSVG_PAINT_LINEAR_GRADIENT || paint->type == NSVG_PAINT_RADIAL_GRADIENT);
        assert (stylePaint.isGradient ());

        auto gradient = paint->gradient;
        auto styleGradient = stylePaint.getGradient ();
        assert (styleGradient->nstops >= 1);

        float inverse [6];
        nvgTransformInverse (inverse, gradient->xform);

        auto innerCol = styleGradient->stops [0].color;
        auto outerCol = styleGradient->stops [styleGradient->nstops - 1].color;

        // Is it always the case that the gradient should be transformed from (0, 0) to (0, 1)?
        rack::math::Vec s, e;
        nvgTransformPoint (&s.x, &s.y, inverse, 0, 0);
        nvgTransformPoint (&e.x, &e.y, inverse, 0, 1);

        return paint->type == NSVG_PAINT_LINEAR_GRADIENT
               ? nvgLinearGradient (vg, s.x, s.y, e.x, e.y, innerCol, outerCol)
               : nvgRadialGradient (vg, s.x, s.y, 0.0, 160, innerCol, outerCol);
    }

    /** Returns the parameterized value of the line p2--p3 where it intersects with p0--p1 */
    static float getLineCrossing (rack::math::Vec p0, rack::math::Vec p1, rack::math::Vec p2, rack::math::Vec p3) {
        auto b = p2.minus (p0);
        auto d = p1.minus (p0);
        auto e = p3.minus (p2);
        auto m = d.x * e.y - d.y * e.x;

        // Check if lines are parallel, or if either pair of points are equal
        if (std::abs (m) < 1e-6)
            return NAN;

        return -(d.x * b.y - d.y * b.x) / m;
    }

    Paint getShapePaint (const NSVGpaint paint) {
        switch (paint.type) {
            case NSVG_PAINT_NONE:
                return Paint::makeNone ();

            case NSVG_PAINT_COLOR:
                return Paint::makeColor (getNVGColor (paint.color));

            case NSVG_PAINT_LINEAR_GRADIENT:
            case NSVG_PAINT_RADIAL_GRADIENT: {
                auto gradient = paint.gradient;
                auto styleGradient = Gradient ();

                styleGradient.nstops = gradient->nstops;
                styleGradient.stops [0].color = getNVGColor (gradient->stops [0].color);
                styleGradient.stops [gradient->nstops - 1].color = getNVGColor (
                    gradient->stops [gradient->nstops - 1].color
                );

                return Paint::makeGradient (styleGradient);
            }
        }

        return Paint::makeColor (rack::color::MAGENTA);
    }

    void getStyle (Style& style, const std::shared_ptr<RackTheme>& themePtr, const NSVGshape* shape) {
        if (themePtr == nullptr)
            return;

        auto shapeInfo = themeCache.getShapeInfo (shape);
        auto classStyle = themePtr->getClassStyle (shapeInfo.styleClass);
        auto idStyle = themePtr->getIdStyle (shapeInfo.shapeId);

        // Shape style
        style = Style ();

        // Fill
        style.setFill (getShapePaint (shape->fill));

        // Stroke
        style.setStroke (getShapePaint (shape->stroke));
        style.setStrokeWidth (shape->strokeWidth);
        style.setStrokeLineCap (static_cast<NVGlineCap> (shape->strokeLineCap));
        style.setStrokeLineJoin (static_cast<int> (shape->strokeLineJoin));

        // Combine theme styles
        if (classStyle != nullptr)
            style = style.combineStyle (classStyle);
        if (idStyle != nullptr)
            style = style.combineStyle (idStyle);
    }

    void ThemeableSvg::draw (NVGcontext* vg, std::shared_ptr<RackTheme> themePtr) {
        if (vg == nullptr || handle == nullptr)
            return;

        int shapeIndex = 0;
        Style shapeStyle;

        // Iterate shape linked list
        for (auto shape = handle->shapes; shape; shape = shape->next, shapeIndex++) {
            // Skip shapes with no paths
            if (shape->paths == nullptr)
                continue;

            // Visibility
            if (!(shape->flags & NSVG_FLAGS_VISIBLE))
                continue;

            nvgSave (vg);
            getStyle (shapeStyle, themePtr, shape);

            // Opacity
            auto opacity = shape->opacity * shapeStyle.getOpacity ();
            if (opacity < 1.0)
                nvgAlpha (vg, opacity);

            // Build path
            nvgBeginPath (vg);

            // Iterate path linked list
            for (auto path = shape->paths; path; path = path->next) {
                // Skip if pts is somehow null
                if (path->pts == nullptr)
                    continue;

                nvgMoveTo (vg, path->pts [0], path->pts [1]);
                for (auto i = 1; i < path->npts; i += 3) {
                    auto p = &path->pts [2 * i];
                    nvgBezierTo (vg, p [0], p [1], p [2], p [3], p [4], p [5]);
                }

                // Close path
                if (path->closed)
                    nvgClosePath (vg);

                // Compute whether this is a hole or a solid.
                // Assume that no paths are crossing (usually true for normal SVG graphics).
                // Also assume that the topology is the same if we use straight lines rather than Beziers (not always
                // the case but usually true).
                // Using the even-odd fill rule, if we draw a line from a point on the path to a point outside the
                // boundary (e.g. top left) and count the number of times it crosses another path, the parity of this
                // count determines whether the path is a hole (odd) or solid (even).
                int crossings = 0;
                auto p0 = rack::math::Vec (path->pts [0], path->pts [1]);
                auto p1 = rack::math::Vec (path->bounds [0] - 1.0, path->bounds [1] - 1.0);

                // Iterate all other paths
                for (auto path2 = shape->paths; path2; path2 = path2->next) {
                    if (path2 == path)
                        continue;

                    // Iterate all lines on the path
                    if (path2->npts < 4)
                        continue;

                    for (auto i = 1; i < path2->npts + 3; i += 3) {
                        auto p = &path2->pts [2 * i];

                        // The previous point
                        auto p2 = rack::math::Vec (p [-2], p [-1]);

                        // The current point
                        auto p3 = (i < path2->npts)
                                ? rack::math::Vec (p [4], p [5])
                                : rack::math::Vec (path2->pts [0], path2->pts [1]);

                        auto crossing = getLineCrossing (p0, p1, p2, p3);
                        auto crossing2 = getLineCrossing (p2, p3, p0, p1);
                        if (0. <= crossing && crossing < 1. && 0. <= crossing2)
                            crossings++;
                    }
                }

                nvgPathWinding (vg, (crossings % 2 == 0) ? NVG_SOLID : NVG_HOLE);
            }

            // Fill shape
            auto fillPaint = shapeStyle.getFill ();
            if (!fillPaint.isNone ()) {
                auto hasGradient =
                    shape->fill.type == NSVG_PAINT_LINEAR_GRADIENT ||
                    shape->fill.type == NSVG_PAINT_RADIAL_GRADIENT;
                if (fillPaint.isGradient () && !hasGradient)
                    nvgFillColor (vg, getNVGColor (shape->fill.color));
                else if (fillPaint.isColor ())
                    nvgFillColor (vg, fillPaint.getColor ());
                else if (fillPaint.isGradient () && shape->fill.gradient != nullptr)
                    nvgFillPaint (vg, getGradient (vg, &shape->fill, fillPaint));

                nvgFill (vg);
            }

            // Stroke shape
            auto strokePaint = shapeStyle.getStroke ();
            if (!strokePaint.isNone ()) {
                nvgStrokeWidth (vg, shapeStyle.getStrokeWidth ());
                // strokeDashOffset, strokeDashArray, strokeDashCount not yet supported
                nvgLineCap (vg, shapeStyle.getStrokeLineCap ());
                nvgLineJoin (vg, shapeStyle.getStrokeLineJoin ());

                auto hasGradient =
                    shape->stroke.type == NSVG_PAINT_LINEAR_GRADIENT ||
                    shape->stroke.type == NSVG_PAINT_RADIAL_GRADIENT;
                if (strokePaint.isGradient () && !hasGradient)
                    nvgStrokeColor (vg, getNVGColor (shape->stroke.color));
                else if (strokePaint.isColor ())
                    nvgStrokeColor (vg, strokePaint.getColor ());
                else if (strokePaint.isGradient () && shape->stroke.gradient != nullptr)
                    nvgStrokePaint (vg, getGradient (vg, &shape->stroke, strokePaint));

                nvgStroke (vg);
            }

            nvgRestore (vg);
        }
    }
}
