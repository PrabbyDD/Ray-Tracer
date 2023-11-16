#ifndef HITTABLE_H
#define HITTABLE_H 

#include "ray.h"
#include "interval.h"
#include "rtweekend.h"

// Define this here to tell compiler that we will define material later, we just want to use it right now. 
// Since we are defining a pointer to the class, compiler doesnt need to know details of class. 
class material; 

// Class to store info about when a ray hits a specific surface. 
class hit_record {
    public: 
        point3 p;
        double t;
        vec3 normal; 
        bool frontfacing; 

        // This mat pointer will be set to the material the object was assigned in main() when we start.  
        shared_ptr<material> mat; 
        

        /*
            We are going to assume that outward_normal vectors passed in are unit length. 
            This function helps us set the normal vector and determine if the intersecting ray
            is facing the front of the object. Our convention is that the normal always points
            in the "outward" direction of the object, so if a ray intersects from the outside, 
            then the normal will be outside, if the ray is intersecting from the inside, the 
            normal will still be outside.

            However, our convention is that we will always make the normal point against
            the intersecting ray. For this to work, we need to store the normal manually. 

            This functions makes it so the normal always points against the incoming ray. 
        */
       void set_face_normal(const ray& r, const vec3& outward_normal) {
            frontfacing = dot(r.direction(), outward_normal) < 0; 
            normal = frontfacing ? outward_normal : -outward_normal; 
       }
};

/*
    Virtual keyword allows polymorphism to take place in derived classes so that 
    downstream objects can have their own implementations of the functions. 
*/
class hittableObject {
    public:
        virtual ~hittableObject() = default;

        virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0; 
};

#endif

