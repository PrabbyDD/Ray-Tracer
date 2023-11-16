/*
    This class defines a ray in cpp code. 
    A ray P(t) = A + tb, where A is the ray origin as a 3D point, and b is the direction, as a 3D vector. t is a parameter
    that moves the ray along a line. 
*/
#ifndef RAY_H
#define RAY_H
#include "vec3.h"

class ray {
    public: 
        point3 orig;
        vec3 dir; 

        // Empty constructor
        ray() {}

        // Parameter constructor
        ray(const point3& origin, const vec3& direction):
            orig(origin), dir(direction)
        {}

        point3 origin() const {return orig;}
        vec3 direction() const {return dir;}

        // Returns what point the ray is at. 
        point3 at(double t) const {
            return orig + t*dir;
        }
        
            
};
#endif