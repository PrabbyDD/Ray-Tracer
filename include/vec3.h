/*
    These guards are for conditional compilation (only compile if defined)
    If a header file is defined twice on accident, it might cause compilation issues with 
    duplicate symbols. If the macro (in this case VEC3_H) is not defined, then we run the 
    below code. 

    A translation unit contains:
        Source File: main cpp file
        Header files: contains declarations and definitions meant to be shared. Included with
        #include
        Preprocessor directives: instructions to the preprocessor, a program running before
        compilation. Stuff like ifndef, define, endif, etc
        Global variables and functions: declarations and datum related to global variables and fns

    After compiling a C++ file, each translation unit is compiled separately into an object file. 
    After compiling all TUs, the linker links all their object files into exe. 

    Basically, it prevents the compiler from getting confused when looking at the same header file multiple times. 
    Good explanation: https://www.youtube.com/watch?v=RU5JUHAiR18
    Good practice to include file guards in all the header files. 
    So for example, we have it in this file, so the first time the compiler sees this header file and goes through the ifnef, it will 
    not be defined, so it will define everything here, but the second time it sees it, it will already be defined so it will skip redefining everything


    There are multiple conditionals if there are many header files that have different requirements. For this we use 
    #if SYSTEM1
        #include.. 
    # elif SYSTEM2
        #include..
    #endif 
    etc
    
*/

#ifndef VEC3_H
#define VEC3_H

#include <cmath>
#include <iostream>
#include <vector> 
#include "rtweekend.h"

// Typically bad practice to use namespace std!
using namespace std; 

class vec3 {
    public:
        vector<double> e; 
        /*  
            Not using homogenous coordinates here, so 3 coordinate-vectors
            Lets try this with built in vectors instead of arrays
            Lets also try to do inverse roots with the quake formula for speed purposes
            Color and point3 are aliases for vec3, same thing. 
        */

        // Empty Constructor
        vec3() {
            e.assign(3,0); 
        } 

        // Parameter version constructor
        vec3(double i, double j, double k) {
            e.push_back(i);
            e.push_back(j);
            e.push_back(k); 
        }

        // Return the coordinates individually
        double x() const { return e[0]; }
        double y() const { return e[1]; }
        double z() const { return e[2]; }

        // Define basic math operators with vectors
        vec3 operator-() const { return vec3(-e[0], -e[1], -e[2]); }
        double operator[](int i) const { return e[i]; }
        double& operator[](int i) { return e[i]; }
        
        vec3& operator+=(const vec3 &v) {
            e[0] += v.e[0];
            e[1] += v.e[1];
            e[2] += v.e[2];
            return *this;
        }

        vec3& operator*=(const double t) {
            e[0] *= t;
            e[1] *= t;
            e[2] *= t;
            return *this; 
        }

        vec3& operator /= (const double t) { return *this *= 1/t; }

        double length() const { return sqrt(length_squared()); }

        double length_squared() const { return e[0]*e[0] + e[1]*e[1] + e[2]*e[2]; }

        static vec3 random() { return vec3(random_double(), random_double(), random_double()); }

        static vec3 random(double min, double max) { 
            return vec3(random_double(min, max), random_double(min, max), random_double(min, max)); 
        }

        bool near_zero() const {
            // Return true if the vector is close to zero in all dimensions
            auto s = 1e-8;
            return ( (fabs(e[0]) < s) && (fabs(e[1] < s)) && (fabs(e[2]) < s) ); 
        }
};

// vec3 utility functions

/*  
    Just a note on friend, which we could use if this was inside the class
    friend allows another class to bypass the private, protected restrictions on another class. 
    When a function is declared with "friend" it can accesses those private informations. 
*/ 
inline ostream& operator<<(ostream &out, const vec3 &v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2];
}

inline vec3 operator+(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] + v.e[0], u.e[1] + v.e[1], u.e[2] + v.e[2]);
}

inline vec3 operator+(double t, const vec3 &v) {
    return vec3(t + v.e[0], t + v.e[1], t + v.e[2]);
}

inline vec3 operator-(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] - v.e[0], u.e[1] - v.e[1], u.e[2] - v.e[2]);
}

inline vec3 operator*(const vec3 &u, const vec3 &v) {
    return vec3(u.e[0] * v.e[0], u.e[1] * v.e[1], u.e[2] * v.e[2]);
}

inline vec3 operator*(double t, const vec3 &v) {
    return vec3(t*v.e[0], t*v.e[1], t*v.e[2]);
}

inline vec3 operator*(const vec3 &v, double t) {
    return t * v;
}

inline vec3 operator/(vec3 v, double t) {
    return (1/t) * v;
}

inline double dot(const vec3 &u, const vec3 &v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
}

inline vec3 cross(const vec3 &u, const vec3 &v) {
    return vec3(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0]);
}

inline vec3 unit_vector(vec3 v) {
    return v / v.length();
}

inline vec3 random_in_unit_disk() {
    while (true) {
        auto p = vec3(random_double(-1,1), random_double(-1,1), 0);
        if (p.length_squared() < 1)
            return p;
    }
}

// Look at this image, we are trying to calculate red reflected angle: https://raytracing.github.io/images/fig-1.15-reflection.jpg
inline vec3 reflect(const vec3& v, const vec3& n) {
    return v - 2*dot(v,n)*n; 
}

// Calculating refracted ray, according to snells law
inline vec3 refract(const vec3& uv, const vec3& n, double etai_over_etat) {
    double cos_theta = fmin(dot(-uv, n), 1.0);
    vec3 r_out_perp = etai_over_etat * (uv + cos_theta*n);
    vec3 r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length_squared())) * n;
    return r_out_perp + r_out_parallel; 
}

// Quakes inverse square root functionality as an approximation. Can also convert to double, but need to do some bitmath then.
inline float Qsqrt(float t) {
    long i; // declare 32 bit number
    float x2, y; // 32 bit decimal
    const float threehalfs = 1.5F; // 1.5 as a double
    x2 = t * 0.5F; 
    y = t;
    // Convert this number to 32 bit number so we can do bit manipulation to it, cannot do bit manip tricks with floats in c++
    // makes program think that thing living at address is long, without changing the bits, then we dereference to get the number as a long
    i = * (long *) &y; 
    /*
        In some ways, the bits of a number are its own logarithm, up to a factor, that means in y, we have stored some log(y), up to a factor
        So basically we are now calculating log(1/sqrt(y)) which is -.5*log(y). Instead of dividing by 2 for the .5, we bit shift once instead. 
        That number we are subtracting from is part of the scaling factor, which we get from some simple math relating the bits to the log of 
        the bits. 
    */
    i = 0x5f3759df - (i >> 1); // recall shifting to left doubles, to right halves. 
    y = * ( float * ) &i; // gets us back original solution after doing bit manip, approximately, which is why we do newton iteration below
    /*
        Reminder that Newton iteration finds a root for a given function. Takes an approx and returns a better one. What it needs is 
        a function and its derivative. Takes an x value and tries to guess how much off it is from the root, by calcing f(x) and its derivative. 
        We basically do x - f(x) / f'(x) each iter. Here we are using f(y) = 1/y^2  - x, because if f(y) = 0, then y = 1/sqrt(x). We are doing this
        without dividing again! One iteration is enough for accuracy within 1%. 
    */
    y = y * (threehalfs - (x2 * y * y)); 
    y = y * (threehalfs - (x2 * y * y)); 
    return y; 
}

inline vec3 unit_vectorQ(vec3 v) {
    return v * Qsqrt((float) v.length_squared());
}

// Keep generating samples until we get one that is in the unit sphere. 
// Can also do this with generating random spherical coords and converting to cartesian, but too much work. 
inline vec3 random_in_unit_sphere() {
    while (1) {
        vec3 p = vec3::random(-1,1);
        if (p.length_squared() < 1) return p; 
    }
}

inline vec3 random_unit_vector() {
    return unit_vectorQ(random_in_unit_sphere()); 
}

// Compare this random unit vector against the surface normal to determine if it is on the proper hemisphere
inline vec3 random_on_hemishpere(const vec3& normal) {
    vec3 ran = random_unit_vector();
    if (dot(normal, ran) > 0.0) {
        return ran;
    } else {
        return -ran; 
    }
}

// Type aliases for vec3
using point3 = vec3; // 3D point
using color = vec3; // RGB color 

#endif