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

#pragma once

#include <functional>
#include <string>

namespace rack_themer {
    struct ThemeCache;

    struct KeyedString {
        friend ThemeCache;

      private:
        int value;

      public:
        bool operator== (const KeyedString& rhs) const { return value == rhs.value; }
        std::size_t getHash () const { return std::hash<int> {} ((int) value); }
    };

    KeyedString getKeyedString (const std::string& text);
    std::string getKeyedStringText (const KeyedString& key);
}

template <>
struct std::hash<rack_themer::KeyedString> {
    std::size_t operator() (const rack_themer::KeyedString& k) const { return k.getHash (); }
};