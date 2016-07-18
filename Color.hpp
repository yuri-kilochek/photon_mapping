#ifndef PHOTON_MAPPING_COLOR_HPP
#define PHOTON_MAPPING_COLOR_HPP

#include "mathFunctions.hpp"

namespace photon_mapping {
    struct Color {
        float r; float g; float b;

        Color() {}

        Color(float c)
            : r(c), g(c), b(c)
        {}

        Color(float r, float g, float b)
            : r(r), g(g), b(b)
        {}

        static Color const BLACK;
        static Color const RED;
        static Color const GREEN;
        static Color const BLUE;
        static Color const WHITE;
    };

    inline Color operator+(Color const& a, Color const& b) {
        return Color(a.r + b.r, a.g + b.g, a.b + b.b);    
    }

    inline Color& operator+=(Color& a, Color const& b) {
        a.r += b.r;
        a.g += b.g;
        a.b += b.b;
        return a;   
    }

    inline Color operator*(Color const& a, float b) {
        return Color(a.r * b, a.g * b, a.b * b);    
    }

    inline Color operator*(float a, Color const&  b) {
        return Color(a * b.r, a * b.g, a * b.b);    
    }

    inline Color& operator*=(Color& a, float b) {
        a.r *= b;
        a.g *= b;
        a.b *= b;
        return a;   
    }

    inline Color operator/(Color const& a, float b) {
        return Color(a.r / b, a.g / b, a.b / b);    
    }

    inline Color& operator/=(Color& a, float b) {
        a.r /= b;
        a.g /= b;
        a.b /= b;
        return a;   
    }

    inline Color operator*(Color const& a, Color const& b) {
        return Color(a.r * b.r, a.g * b.g, a.b * b.b);    
    }

    inline Color& operator*=(Color& a, Color const& b) {
        a.r *= b.r;
        a.g *= b.g;
        a.b *= b.b;
        return a;    
    }

    inline float luminance(Color const& color) {
        return 0.2126f * color.r + 0.7152f * color.g + 0.0722f * color.b;
    }

    inline Color min(Color const& a, Color const& b) {
        return Color(
            min(a.r, b.r),
            min(a.g, b.g),
            min(a.b, b.b)
        );
    }

    inline Color max(Color const& a, Color const& b) {
        return Color(
            max(a.r, b.r),
            max(a.g, b.g),
            max(a.b, b.b)
        );
    }

    inline Color clamp(Color const& a, Color const& x, Color const& b) {
        return Color(
            clamp(a.r, x.r, b.r),
            clamp(a.g, x.g, b.g),
            clamp(a.b, x.b, b.b)
        );
    }
}

#endif