#ifndef COLOR_H
#define COLOR_H

#include "vec3.h"
#include <iostream>
using color = vec3; 

// Convert linear color space to gamma space, which is what image viewer assumes
inline double linear_to_gamma(double linear_component)
{
    return sqrt(linear_component);
}

// Write a single pixel's color out to the standard output stream
void write_color(std::ostream &out, color pixel_color, int samples_per_pixel = 10) {

    double r = pixel_color.x();
    double g = pixel_color.y();
    double b = pixel_color.z(); 

    // We are going to sample some number of points in a pixel, so just start averaging here. 
    double scale = 1.0 / samples_per_pixel; 
    r*= scale;
    g*= scale;
    b*= scale; 

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b); 

    // Write the translated [0,255] value of each color component, clamp to help us make sure in [0,1]
    static const interval intensity(0.000, 0.999); 
    out << static_cast<int>(255.999 * intensity.clamp(r)) << ' '
        << static_cast<int>(255.999 * intensity.clamp(g)) << ' '
        << static_cast<int>(255.999 * intensity.clamp(b)) << '\n';
}

#endif