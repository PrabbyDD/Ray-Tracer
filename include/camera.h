#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"
#include "color.h"
#include "hittableObject.h"
#include "material.h"

/*
    This class is responsible for:
        1. Constructing and dispatching rays into the world
        2. Using the results of these rays and their intersections to make the rendered image
*/

class camera
{
    public:
        double aspect_ratio      = 1.0;  // Ratio of image width over height
        int    image_width       = 100;  // Rendered image width in pixel count
        int    samples_per_pixel = 10;   // Count of random samples for each pixel
        int    max_depth         = 10;   // Maximum number of ray bounces into scene

        double vfov     = 90;              // Vertical view angle (field of view)
        point3 lookfrom = point3(0,0,-1);  // Point camera is looking from
        point3 lookat   = point3(0,0,0);   // Point camera is looking at
        vec3   vup      = vec3(0,1,0);     // Camera-relative "up" direction

        double defocus_angle = 0;  // Variation angle of rays through each pixel
        double focus_dist = 10;    // Distance from camera lookfrom point to plane of perfect focus

        void render(const hittableObject& world) {
            initialize(); 

            // Render it
            cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

            /*  
                Go pixel by pixel
                it goes left to right, top to bottom, top left is (0,0)
                We go row by row, starting at top row

                Each rgb value is [0.0,1.0] right now. 
                need to redirect output to an image file, so use the redirection operator
                Command on windows is : ./{filename}.exe > {imagename}.ppm
                On linux/mac is ./{filename} > image.ppm
            */ 

            /*
                This code will row by row, scanning each pixel individually. 
            */
            for (int j = 0; j < image_height; ++j) {
                // Flushing is the same as endl, except endl also adds an newline. It just makes sure our log is printed in synch always. 
                std::clog << "\rScanline remaining: " << (image_height - j) << ' ' << std::flush; 
                for (int i = 0; i < image_width; ++i) {
                    color pixel_color(0,0,0); 
                    // Multisampling in each pixel loop, add them all to get averaged value for that pixel
                    for (int s = 0; s < samples_per_pixel; ++s) {
                        ray r = get_ray(i,j); 
                        pixel_color += ray_color(r, max_depth, world); 
                    }
                    // function writes a single pixel's color to console out (cout)
                    // Averaging of all samples done in this function
                    write_color(cout, pixel_color, samples_per_pixel); 
                }
            }

            // A checkpoint, write to standard error 
            clog << "\rDone.                 \n"; 
    }

    
    private:
        int    image_height;    // Rendered image height
        point3 camera_origin;   // Camera center
        point3 pixel00_loc;     // Location of pixel 0, 0
        vec3   pixel_delta_u;   // Offset to pixel to the right
        vec3   pixel_delta_v;   // Offset to pixel below
        vec3   u, v, w;         // Camera frame basis vectors
        vec3   defocus_disk_u;  // Defocus disk horizontal radius
        vec3   defocus_disk_v;  // Defocus disk vertical radius

        // Gets the ray going through pixel at (i, j)
        ray get_ray(int i, int j) const {
             // Get a randomly-sampled camera ray for the pixel at location i,j, originating from
             // the camera defocus disk.
            point3 curr_pixel_center = pixel00_loc + (i * pixel_delta_u) + (j * pixel_delta_v); 
            point3 pixel_sample = curr_pixel_center + pixel_sample_square(); 
            point3 ray_origin = (defocus_angle <= 0) ? camera_origin : defocus_disk_sample();
            return ray(ray_origin, pixel_sample - ray_origin);
        }

        // Initialize the camera and za warudo
        void initialize() {
            image_height = static_cast<int>(image_width / aspect_ratio);
            image_height = (image_height < 1) ? 1 : image_height; 
            camera_origin = lookfrom; 


            // Camera setup, calculate with img_w/img_h instead of aspect ratio because aspect_ratio is ideal, but not real
            double theta = degrees_to_radians(vfov);
            double h = tan(theta/2); 
            double viewport_height = 2 * h * focus_dist;
            double viewport_width = viewport_height * (static_cast<double>(image_width) / image_height);
             
            // Calculate the u,v,w unit basis vectors for the camera coordinate frame.
            w = unit_vector(lookfrom - lookat);
            u = unit_vector(cross(vup, w));
            v = cross(w, u);

            // Calculate the vectors across the horizontal and down the vertical viewport edges.
            vec3 viewport_u = viewport_width * u;    // Vector across viewport horizontal edge
            vec3 viewport_v = viewport_height * -v;  // Vector down viewport vertical edge

            // The space between centers of pixels in the (u)horizontal direction and (v)vertical directions
            pixel_delta_u = viewport_u / image_width;
            pixel_delta_v = viewport_v / image_height; 

            // Need upper left corner pixel, thats where we start iterating for the pixels
            // If you look at this image, the calculation makes more sense: https://raytracing.github.io/images/fig-1.03-cam-geom.jpg
            point3 upper_left_corner = camera_origin - (focus_dist * w) - viewport_u/2 - viewport_v/2;
            pixel00_loc = upper_left_corner + .5 * (pixel_delta_u + pixel_delta_v);  

            // Calculate the camera defocus disk basis vectors.
            double defocus_radius = focus_dist * tan(degrees_to_radians(defocus_angle / 2));
            defocus_disk_u = u * defocus_radius;
            defocus_disk_v = v * defocus_radius;
        }

        point3 defocus_disk_sample() const {
            // Returns a random point in the camera defocus disk.
            auto p = random_in_unit_disk();
            return camera_origin + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
        }

        /*  
            This function calcualtes the color at the point of intersection of an object with a ray r
            Need to test with the object if we hit it, store it in the record, and then calculate
            the color based on a lerp. 
        */
        color ray_color(const ray& r, int depth, const hittableObject& world) {
            
            // limit depth of the rays
            if (depth <= 0) {
                return color(0,0,0); 
            }

            // Test if we ever hit any objects in the world, and which was the closest
            // Interval min is .001 to prevent FP errors from giving us bounceback rays that are too close to object 
            hit_record rec; 
            if (world.hit(r, interval(0.001, infinity), rec)) {
                ray scattered; 
                color attenuation; 
                // If the ray scattered again
                if (rec.mat->scatter(r, rec, attenuation, scattered)) {
                    return attenuation * ray_color(scattered, depth - 1, world); 
                }
                return color(0,0,0); 
            }
            vec3 unit_dir = unit_vectorQ(r.direction());
            double a = .5*(unit_dir.y() + 1);
            return (1.0 - a)*color(1.0,1.0,1.0) + a*color(.5,.7,1.0); 
        } 

        // Return random sample within a pixel's bounds
        vec3 pixel_sample_square() const {
            double px = -.5 + random_double();
            double py = -.5 + random_double(); 
            return (px * pixel_delta_u) + (py * pixel_delta_v); 
        }
};




#endif