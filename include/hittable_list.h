#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "hittableObject.h"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared; 

/*
    Quickly recall that shared pointers make memory management easy because we do not have 
    to manually free memory when we need it for a pointer. shared and unique pointers are smart pointers
    and keep a reference count to the object. 

    The typical creation of a shared pointer is 
    shared_ptr<type> type_ptr = make_shared<type>(parameters of type constructor)

*/

class hittable_list : public hittableObject 
{
    public:
        // vector of pointers to hittable objects, they are smart pointers so shared_ptr will 
        // die when all references are gone to that object. 
        vector<shared_ptr<hittableObject>> scene_objects; 

        hittable_list() {}
        hittable_list(shared_ptr<hittableObject> obj) { add(obj); }

        void clear() { scene_objects.clear(); }

        void add(shared_ptr<hittableObject> obj) { scene_objects.push_back(obj); }

        // Go through each object and determine if there is a hit with the current ray with any of them
        bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
            hit_record tmp; 
            bool hit_anything = false; 
            double closest_so_far = ray_t.max; 

            for (const auto& obj: scene_objects) {
                if (obj->hit(r, interval(ray_t.min, closest_so_far), tmp)) {
                    hit_anything = true; 
                    closest_so_far = tmp.t;
                    rec = tmp; 
                }
            }
            return hit_anything; 
        }

};



#endif
