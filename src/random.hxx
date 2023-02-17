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

#ifndef RANDOM_HXX
#define RANDOM_HXX

#include <chrono>
#include <cstdint>
#include <thread>

#define PCG32_DEFAULT_STATE 0xffc5d3a96def55f9ULL
#define PCG32_DEFAULT_SEQ   0xc3579e39d9baf6b8ULL

class pcg32 final {
public:
    pcg32() {
        seed(PCG32_DEFAULT_STATE, PCG32_DEFAULT_SEQ);
    }

    pcg32(uint64_t initstate, uint64_t initseq) {
        seed(initstate, 2 * initseq + 1);
    }

    void seed(uint64_t initstate, uint64_t initseq) {
        state = 0U;
        inc = (initseq << 1U) | 1U;
        next_uint();
        state += initstate;
        next_uint();
    }

    uint32_t next_uint() {
        uint64_t oldstate = state;
        state = oldstate * 6364136223846793005ULL + inc;
        uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
        uint32_t rot = oldstate >> 59u;
        return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
    }

    float next_float() {
        // Sebastiano Vigna's method of converting uint -> float
        return (next_uint() >> 8) * (1.0f / ((uint32_t)1 << 24));
    }
private:
    uint64_t state;
    uint64_t inc;
};

#endif // RANDOM_HXX
