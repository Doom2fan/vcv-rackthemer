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

#include "ThemeLoader.hpp"
#include "rack_themer.hpp"

#include <fmt/format.h>

namespace rack_themer {
    ThemeLoader themeLoader;

namespace logging {
    void setLogger (logging::LogCallback logger) { themeLoader.setLogger (logger); }

    const char* severityName (logging::Severity severity) {
        switch (severity) {
            case logging::Severity::Info: return "Info";
            case logging::Severity::Warn: return "Warn";
            case logging::Severity::Error: return "Error";
            case logging::Severity::Critical: return "Critical";

            default: return "UNKNOWN";
        }
    }
}

    static void logNothing (logging::Severity severity, logging::ErrorCode code, std::string info) { }

    ThemeLoader::ThemeLoader () { logger = logNothing; }

    inline int hexValue (unsigned char ch) {
        if (ch > 'f' || ch < '0') return -1;
        if (ch <= '9') return ch & 0xF;
        if (ch < 'A') return -1;
        if (ch < 'G') return 10 + ch - 'A';
        if (ch < 'a') return -1;

        return 10 + ch - 'a';
    }

    bool isValidHexColor (std::string hex) {
        switch (hex.size ()) {
            case 1 + 3:
            case 1 + 4:
            case 1 + 6:
            case 1 + 8: break;
            default: return false;
        }

        if (*hex.begin () != '#')
            return false;

        return hex.find_first_not_of ("0123456789ABCDEFabcdef", 1) == std::string::npos;
    }

    std::vector<uint8_t> parseHex (std::string hex) {
        std::vector<uint8_t> result;

        // Color representations:
        //  short: #rgb, #rgba
        //  long: #rrggbb, #rrggbbaa
        auto longHex = true;
        switch (hex.size ()) {
            case 1 + 3:
            case 1 + 4: longHex = false; break;
            case 1 + 6:
            case 1 + 8: longHex = true; break;
            default: return result;
        }

        enum State { Hex = -1, R1, R2, G1, G2, B1, B2, A1, A2, End };
        int curState = State::Hex;
        int value = 0;

        for (unsigned char ch : hex) {
            if (curState == State::Hex) {
                if (ch == '#')
                    ++curState;
                else
                    return result;
            } else {
                auto nibble = hexValue (ch);

                if (nibble == -1) {
                    result.clear ();
                    return result;
                }

                if (curState & 1) { // odd
                    value |= nibble;
                    result.push_back (value);
                    value = 0;
                    ++curState;
                } else { // even
                    value = nibble << 4;
                    if (longHex)
                        ++curState;
                    else {
                        result.push_back (value);
                        value = 0;
                        curState += 2;
                    }
                }
            }

            if (curState >= State::End)
                break;
        }

        return result;
    }

    bool ThemeLoader::requireValidHexColor (std::string hex, const char* name) {
        if (isValidHexColor (hex))
            return true;

        logError (logging::ErrorCode::InvalidHexColor, fmt::format (FMT_STRING ("'{}': invalid hex color: '{}'"), name, hex));
        return false;
    }

    bool ThemeLoader::requireArray (json_t* j, const char* name) {
        if (json_is_array (j))
            return true;

        logError (logging::ErrorCode::ArrayExpected, fmt::format (FMT_STRING ("'{}': array expected"), name));
        return false;
    }

    bool ThemeLoader::requireObject (json_t* j, const char* name) {
        if (json_is_object (j))
            return true;

        logError (logging::ErrorCode::ObjectExpected, fmt::format (FMT_STRING ("'{}': object expected"), name));
        return false;
    }

    bool ThemeLoader::requireObjectOrString (json_t* j, const char* name) {
        if (json_is_object (j) || json_is_string (j))
            return true;

        logError (logging::ErrorCode::ObjectOrStringExpected, fmt::format (FMT_STRING ("'{}': Object or string expected"), name));
        return false;
    }

    bool ThemeLoader::requireString (json_t* j, const char* name) {
        if (json_is_string (j))
            return true;

        logError (logging::ErrorCode::StringExpected, fmt::format (FMT_STRING ("'{}': String expected"), name));
        return false;
    }

    bool ThemeLoader::requireNumber (json_t* j, const char* name) {
        if (json_is_number (j))
            return true;

        logError (logging::ErrorCode::NumberExpected, fmt::format (FMT_STRING ("'{}': Number expected"), name));
        return false;
    }

    bool ThemeLoader::requireInteger (json_t* j, const char* name) {
        if (json_is_integer (j))
            return true;

        logError (logging::ErrorCode::IntegerExpected, fmt::format (FMT_STRING ("'{}': Integer expected"), name));
        return false;
    }

    NVGcolor parseColor (const char* text) {
        auto parts = parseHex (text);

        if (parts.size () == 3)
            return nvgRGB (parts [0], parts [1], parts [2]);
        if (parts.size () == 4)
            return nvgRGBA (parts [0], parts [1], parts [2], parts [3]);

        // user error coding the color
        return rack::color::BLACK;
    }

    float getNumber (json_t* j) {
        if (json_is_real (j))
            return json_real_value (j);
        if (json_is_number (j))
            return json_number_value (j);
        if (json_is_integer (j))
            return static_cast<float> (json_integer_value (j));

        return 1.f;
    }

    bool ThemeLoader::parseOpacity (json_t* root, std::shared_ptr<Style> style) {
        auto jOpacity = json_object_get (root, "opacity");
        if (jOpacity == nullptr)
            return true;

        if (!requireNumber (jOpacity, "opacity"))
            return false;

        style->setOpacity (std::max (0.f, std::min (1.f, getNumber (jOpacity))));
        return true;
    }

    bool ThemeLoader::parseGradient (json_t* jGradient, Gradient& gradient) {
        if (jGradient == nullptr)
            return true;

        bool ok = true;
        gradient.nstops = 0;

        if (!requireArray (jGradient, "gradient"))
            return false;

        int index = 0;
        auto color = NVGcolor ();
        float offset = 0.f;

        json_t* item;
        size_t n;
        json_array_foreach (jGradient, n, item) {
            if (n > 1) {
                logError (logging::ErrorCode::TwoGradientStopsMax, "A maximum of two gradient stops is allowed");
                return false;
            }

            auto jIndex = json_object_get (item, "index");
            if (jIndex != nullptr) {
                if (requireInteger (jIndex, "index")) {
                    index = json_integer_value (jIndex);

                    if (!(index == 0 || index == 1)) {
                        logError (logging::ErrorCode::GradientStopIndexZeroOrOne, "Gradient stop index must be 0 or 1");
                        ok = false;
                    }
                } else {
                    index = 0;
                    ok = false;
                }
            }

            auto jColor = json_object_get (item, "color");
            if (jColor != nullptr) {
                if (requireString (jColor, "color")) {
                    auto hex = json_string_value (jColor);

                    if (requireValidHexColor (hex, "color"))
                        color = parseColor (hex);
                    else {
                        color = rack::color::BLACK;
                        ok = false;
                    }
                } else {
                    color = rack::color::BLACK;
                    ok = false;
                }
            }

            auto jOffset = json_object_get (item, "offset");
            if (jOffset != nullptr) {
                if (requireNumber (jOffset, "offset"))
                    offset = getNumber (jOffset);
                else {
                    offset = 0.f;
                    ok = false;
                }
            }

            if (ok)
                gradient.stops [index] = GradientStop (index, offset, color);
        }

        if (!ok)
            return false;

        int count = 0;
        if (gradient.stops [0].index >= 0) ++count;
        if (gradient.stops [1].index >= 0) ++count;

        gradient.nstops = count;

        return true;
    }

    bool ThemeLoader::parseFill (json_t* root, std::shared_ptr<Style> style) {
        auto jFill = json_object_get (root, "fill");
        if (jFill == nullptr)
            return true;

        if (!requireObjectOrString (jFill, "fill"))
            return false;

        // Color-only
        if (json_is_string (jFill)) {
            auto value = json_string_value (jFill);

            if (strcmp (value, "none") == 0)
                style->setFill (Paint::makeNone ());
            else {
                if (!requireValidHexColor (value, "fill"))
                    return false;

                style->setFill (Paint::makeColor (parseColor (value)));
            }
            return true;
        }

        // Full object
        auto jColor = json_object_get (jFill, "color");
        if (jColor != nullptr) {
            if (!requireString (jColor, "color"))
                return false;

            auto hex = json_string_value (jColor);
            if (!requireValidHexColor (hex, "color"))
                return false;

            style->setFill (Paint::makeColor (parseColor (hex)));
        }

        auto jGradient = json_object_get (jFill, "gradient");
        if (jGradient != nullptr) {
            if (jColor != nullptr) {
                logError (logging::ErrorCode::OneOfColorOrGradient, "'fill': Only one of 'color' or 'gradient' allowed");
                return false;
            }

            Gradient gradient;
            if (parseGradient (jGradient, gradient) && gradient.nstops > 0)
                style->setFill (Paint::makeGradient (gradient));
        }

        return true;
    }

    bool ThemeLoader::parseStroke (json_t* root, std::shared_ptr<Style> style) {
        auto jStroke = json_object_get (root, "stroke");

        if (jStroke == nullptr)
            return true;

        if (!requireObjectOrString (jStroke, "stroke"))
            return false;

        // Color-only
        if (json_is_string (jStroke)) {
            auto value = json_string_value (jStroke);

            if (strcmp (value, "none") == 0)
                style->setStroke (Paint::makeNone ());
            else {
                if (!requireValidHexColor (value, "stroke"))
                    return false;

                style->setStroke (Paint::makeColor (parseColor (value)));
            }

            return true;
        }

        // Full object
        auto jWidth = json_object_get (jStroke, "width");
        if (jWidth != nullptr) {
            if (!requireNumber (jWidth, "width"))
                return false;

            style->setStrokeWidth (getNumber (jWidth));
        }

        auto jColor = json_object_get (jStroke, "color");
        if (jColor != nullptr) {
            if (!requireString (jColor, "color"))
                return false;

            auto hex = json_string_value (jColor);
            if (!requireValidHexColor (hex, "color"))
                return false;

            style->setStroke (Paint::makeColor (parseColor (hex)));
        }

        auto jGradient = json_object_get (jStroke, "gradient");
        if (jGradient != nullptr) {
            if (jColor != nullptr) {
                logError (logging::ErrorCode::OneOfColorOrGradient, "'stroke': Only one of 'color' or 'gradient' allowed");
                return false;
            }

            Gradient gradient;
            if (parseGradient (jGradient, gradient) && gradient.nstops > 0)
                style->setStroke (Paint::makeGradient (gradient));
        }

        auto jLineCap = json_object_get (jStroke, "line_cap");
        if (jLineCap != nullptr) {
            if (!requireString (jLineCap, "line_cap"))
                return false;

            auto valueStr = json_string_value (jLineCap);
            NVGlineCap value;
            if (strcmp (valueStr, "butt") == 0)
                value = NVG_BUTT;
            else if (strcmp (valueStr, "round") == 0)
                value = NVG_ROUND;
            else if (strcmp (valueStr, "square") == 0)
                value = NVG_SQUARE;
            else if (strcmp (valueStr, "bevel") == 0)
                value = NVG_BEVEL;
            else if (strcmp (valueStr, "miter") == 0)
                value = NVG_MITER;
            else {
                logError (logging::ErrorCode::InvalidLineCap, fmt::format (FMT_STRING ("'line_cap': Unrecognized line cap type '{}'"), valueStr));
                return false;
            }

            style->setStrokeLineCap (value);
        }

        return true;
    }

    bool ThemeLoader::parseStyle (const char* name, json_t* root, std::shared_ptr<RackTheme> theme) {
        if (strlen (name) < 1)
            return false;

        logInfo (fmt::format (FMT_STRING ("Parsing '{}'"), name));
        auto style = std::make_shared<Style> ();

        if (!parseFill (root, style) ||
            !parseStroke (root, style) ||
            !parseOpacity (root, style))
            return false;

        if (name [0] == '.')
            theme->idStyles [getKeyedString (name + 1)] = style;
        else
            theme->classStyles [getKeyedString (name)] = style;

        return true;
    }

    bool ThemeLoader::parseTheme (json_t* root, std::shared_ptr<RackTheme>& theme) {
        if (!json_is_object (root)) {
            logError (logging::ErrorCode::ArrayExpected, "The top level element must be an object");
            return false;
        }

        auto jName = json_object_get (root, "name");
        const char* name = nullptr;

        if (jName && json_is_string (jName))
            name = json_string_value (jName);

        if (name == nullptr || !(*name)) {
            logError (logging::ErrorCode::NameExpected, "The theme must have a non-empty name");
            return false;
        }

        auto jStyles = json_object_get (root, "styles");

        if (jStyles == nullptr || !json_is_object (jStyles)) {
            logError (logging::ErrorCode::ThemeExpected, "Expected a 'styles' object");
            return false;
        }

        logInfo (fmt::format (FMT_STRING ("Parsing theme '{}'"), name));

        theme = std::make_shared<RackTheme> ();
        theme->name = name;

        void* n = nullptr;
        const char* key = nullptr;
        json_t* jStyle = nullptr;
        json_object_foreach_safe (jStyles, n, key, jStyle) {
            if (!json_is_object (jStyle)) {
                logError (logging::ErrorCode::ObjectExpected, fmt::format (FMT_STRING ("Theme '{}': Each style must be an object"), theme->name));
                return false;
            }

            if (!parseStyle (key, jStyle, theme))
                return false;
        }

        return true;
    }

    std::shared_ptr<RackTheme> ThemeLoader::loadTheme (std::string filePath) {
        auto file = std::fopen (filePath.c_str (), "r");
        if (file == nullptr) {
            logger (logging::Severity::Critical, logging::ErrorCode::CannotOpenJsonFile, filePath.c_str ());
            return nullptr;
        }

        json_error_t error;
        auto root = json_loadf (file, 0, &error);
        if (root == nullptr) {
            logError (logging::ErrorCode::JsonParseFailed, fmt::format (FMT_STRING ("Parse error - {} {}:{} {}"),
                error.source,
                error.line,
                error.column,
                error.text
            ));

            std::fclose (file);
            return nullptr;
        }

        std::shared_ptr<RackTheme> theme = nullptr;
        if (!parseTheme (root, theme))
            theme = nullptr;

        json_decref (root);
        std::fclose (file);

        return theme;
    }
}