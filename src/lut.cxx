// Copyright (C) 2022  Fernando García Liñán
//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
//
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

#include "lut.hxx"

#include <algorithm>
#include <limits>

float
lut_lerp(const LookupTable &table, float x)
{
    // Saturate if key is out of bounds
    if (x > table.back().first)  return table.back().second;
    if (x < table.front().first) return table.front().second;

    // Get the first element whose key (first value on the pair) is greater
    // or equal than x.
    auto it = std::lower_bound(
        table.begin(), table.end(),
        std::make_pair(x, std::numeric_limits<float>::min()));

    // The found element turned out to be the first on the table, so just
    // return it as we have no other data to interpolate with.
    if (it == table.begin())
        return it->second;

    // Get the previous element
    auto it2 = it;
    --it2;
    // Perform linear interpolation between both elements
    return it2->second + (it->second - it2->second) * (x - it2->first)
        / (it->first - it2->first);
}
