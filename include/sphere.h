#ifndef SPHERE_H
#define SPHERE_H

#include "hittableObject.h"
#include "vec3.h"

class sphere : public hittableObject { 

    public: 
        sphere(point3 _center, double _radius, shared_ptr<material> _material)
            : center(_center), radius(_radius), mat(_material) {} 

    /* 
        Test if a ray intersects a sphere via quadratic formula
        Compute the root part, if negative, we have no real roots and no intersection
        If 0, we have 1 real root and an intersection on the surface of sphere
        If root > 0 then we have 2 real roots and a cut through the sphere

        We also want to use this to shade surface normals on spheres, which means
        determining how a surface should appear when illuminated by light sources based
        on the normal vector. 

        Also have a tmin and tmax that lets us define a valid range we where a hit is only
        valid in that range. 

        function is const because it is promising not to modify internal data of this object,
        only the hit record object, which is in a different class. 
    */
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            vec3 oc = r.origin() - center; 
            // Dot product of a vector with itself is just length squared
            double a = r.direction().length_squared(); 
            double half_b = dot(r.direction(), oc); 
            double c = oc.length_squared() - radius*radius; 

            double discriminant = half_b*half_b - a*c; 
            double d_sqrt = sqrt(discriminant); 
            if (discriminant < 0) {
                return false; 
            }

            // Find nearest intersection that lies in acceptable range on ray
            double root = (-half_b - d_sqrt) / a; 
            // Situations in which we are not in acceptable range
            if (!ray_t.surrounds(root)) {
            //if (root <= ray_t.min || root >= ray_t.max) {
                root = (-half_b + d_sqrt) / a;  
                if (!ray_t.surrounds(root)) {
                //if (root <= ray_t.min || root >= ray_t.max) {
                    return false; 
                }
            }

            // We hit the sphere in acceptable range, record the spot we hit it 
            rec.t = root; 
            rec.p = r.at(root); 
            // Normal that always points outward of sphere, which we adjust with set_face_normal
            // normalized with radius because the length of the vector from center to intersecting point is r
            vec3 outward_normal = (rec.p - center) / radius; 
            rec.set_face_normal(r, outward_normal);
            rec.mat = mat; 
            return true;
        }

    private:
        double radius;
        point3 center;
        shared_ptr<material> mat; 
};

#endif