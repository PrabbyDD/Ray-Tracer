#ifndef MATERIAL_H
#define MATERIAL_H

#include "rtweekend.h"
#include "hittableObject.h"
/*
    This class defines abstract properties a material should have. 
*/

// add this hit_record class notataion here to tell compiler this class will be defined later
class hit_record; 

class material {

    public: 
        // Add destructor here so that all subclasses will use the default destructor unless overriden
        // important for memory management of subobjects!
        virtual ~material() = default; 

        // Recall virtual means it will be override, and = 0 means that this function is abstract and meant to be overriden. 
        virtual bool scatter (const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const = 0; 

};

// Lambertian material
class lambertian : public material {

    public:
        lambertian(const color& a) : albedo(a) {}

        bool scatter (const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const override {
            
            double does_it_scatter = random_double();
            if (does_it_scatter < 1) {
                // It does scatter, with probability p. 
                vec3 scatter_dir = rec.normal + random_unit_vector(); 

                // Catch degenerate scatter direction (too close to 0)
                if (scatter_dir.near_zero()) scatter_dir = rec.normal; 

                scattered = ray(rec.p, scatter_dir); 
                // Albedo is gonna represent how much energy we lose on a scatter
                attenuation = albedo / 1; 
                return true; 
            } else {
                // Did not scatter, was absorted
                return false; 
            }
        }


    private:
        /*
            Albedo is a measure of how much light or radiation a surface reflects, in [0,1]
            0 albedo means perfectly absorbing surface (no reflection), 1 means always reflecting (no absorption)
            For example, fresh snow has high albedo (light), asphalt has low albedo (dark)
        */
        color albedo; 
};

//Metal material
class metal : public material {
    public: 
        metal(const color& a, double f) : albedo(a), fuzz(f < 1 ? f : 1) {}

         bool scatter (const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const override {
            
            double does_it_scatter = random_double();
            if (does_it_scatter < 1) {
                // It does scatter, with probability p, get the reflected vector
                vec3 reflected = reflect(unit_vectorQ(r.direction()), rec.normal); 
                scattered = ray(rec.p, reflected + fuzz*random_unit_vector());
                // Albedo is gonna represent how much energy we lose on a scatter
                attenuation = albedo / 1; 
                return (dot(scattered.direction(), rec.normal) > 0); 
            } else {
                // Did not scatter, was absorted
                return false; 
            }
        }

    private:
        color albedo;
        double fuzz; 
}; 



// Dielectric material, such as water, glass diamond. When a ray hits these mats, they split into a reflected ray and refracted (trasmitted into new material) ray
class dielectric : public material {
    public: 
        dielectric(double ir_) : ir(ir_) {}

         bool scatter (const ray& r, const hit_record& rec, color& attenuation, ray& scattered) const override {
            
            // Attenuation is always 1 because the glass durface absorbs nothing
            attenuation = color(1.0,1.0,1.0); 
            double refraction_ratio = rec.frontfacing ? (1.0/ir) : ir; 
            vec3 unit_direction = unit_vectorQ(r.direction());
            double cos_theta = fmin(dot(-unit_direction, rec.normal), 1.0); 
            double sin_theta = sqrt(1.0 - cos_theta*cos_theta); 

            // Due to snells law, sometimes we may get total internal reflection
            bool cannot_refract = refraction_ratio * sin_theta > 1.0;
            vec3 direction; 

            if (cannot_refract || reflectance(cos_theta, refraction_ratio) > random_double()) {
                direction = reflect (unit_direction, rec.normal);
            } else {
                direction = refract(unit_direction, rec.normal, refraction_ratio);
            }
            scattered = ray(rec.p, direction);
            return true; 
        }

    private:
        double ir; // refraction index

        // For shlich approximation for making the glass have different reflectivity under different angles
        static double reflectance(double cosine, double ref_idx) {
            auto r0 = (1 - ref_idx) / (1 + ref_idx);
            r0 = r0 * r0; 
            return r0 + (1 - r0) * pow((1 - cosine), 5); 
        }
}; 
#endif