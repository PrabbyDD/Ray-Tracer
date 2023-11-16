#include "include/sphere.h"
#include "include/rtweekend.h"
#include "include/hittable_list.h"
#include "include/camera.h"
#include "include/color.h"
#include "include/material.h"

using namespace std; 
/*
    This code will be used to build a custom ray tracer and compare to my performance in the one I built in 184 in 2021. 
    Recall the basics of a ray tracer and its steps: 

    Ray tracing is used in graphics to make realistic images by simulating light paths in a scene, modelling how light interacts
    with objects in an image and calculates things like colors and intensity of each pixel in the final image. 

    1. Ray generation: For each pixel, a ray is cast from the camera position through the pixel
    2. Intersection testing: this ray is tested for intersections with obejcts in the scene
    3. Shading and Lighting: when an intersection is found, the proeprties of the object at that point are used to 
    calculate its color and intensity at that point
    4. We can add more properties when the light hits the object, like reflection and refraction rays
    5. Recursively repeat: these rays can bounce off more objects, and the simulation continues until our ray limit. 
    6. Color accumulation: as rays propogate through the scene and intersect objects, the colors accumulate for the final image

    Ray tracing is typically computationally expensive compared to other methods, but is really good. The aspect of indirect lighting 
    has made ray tracers a stample in movie rendering. This ray tracer requires a bit of math knowledge, like basic linear algebra.
    I also dont know if we will use homogenous coordinates or quaternions for this until I write code :P. Can also add more complex things 
    from 184 onto this as well down the line. 

    CMAKE would be good too (in linux, once project is done), examples in 184 code. Also grpc calling this service would be good
    In the future can compare performance with a rasterizer, and also add shaders. Additional DS
    optimizations like BVHs can be possible as well, and thread level parallelism. Also making this 
    project compile with cmake is better, but should be done on linux then. Ray marching would be good too
    fresnel equations for refractions and reflection and more realistic stuff. Also doing this stuff again
    with opengl would be handy. Also put it on git!

    try to take into account light polarization in the ray tracer? specifically for underwater rendering, or glass surfaces
*/


/*
    We want to antialias our image. This reduces jaggies and high frequency artifacts 
    from undersampling our image. The real world has infinite resolution and cameras and 
    our eyes can only perceive so much of it. We are not sampling at the nyquist rate, 
    so we will get artifacts. But we can reduce this in various ways: 
        1. Supersampling our image at a higher resolution and then downsampling to target rez
        2. Averaging multiple samples within the same pixel and using their average color
        3. Pass through a low pass filter like gaussian blur
        4. Physically moving the polygons to align them with boundaries better.
    We will do something similar to 2. 
*/
/* 
    Outputting an image - there are many ways to do this and many image formats, but for now lets use PPM. 
    In the future, we can try PNG, JPEG (maybe try to implement them directly and do some cool signal processing), 
    and the QUITE OK format would be cool. Also being able to change it to output a video would also be dope. 

    Example of PPM format and how it stores RGB images
    P3 
    # the p3 means colors in ascii, then 3 columns and 2 rows, then 255 for max color, then rgb triples, there is new line at each line
    3 2 
    255
    255   0  0    0 255   0 0 0 255
    255 255  0  255 255 255 0 0   0
*/

int main(int argc, char* argv[]) {
    
    // Make za warudo
    hittable_list world;

    auto ground_material = make_shared<lambertian>(color(0.5, 0.5, 0.5));
    world.add(make_shared<sphere>(point3(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = random_double();
            point3 center(a + 0.9*random_double(), 0.2, b + 0.9*random_double());

            if ((center - point3(4, 0.2, 0)).length() > 0.9) {
                shared_ptr<material> sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = color::random() * color::random();
                    sphere_material = make_shared<lambertian>(albedo);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else if (choose_mat < 0.95) {
                    // metal
                    auto albedo = color::random(0.5, 1);
                    auto fuzz = random_double(0, 0.5);
                    sphere_material = make_shared<metal>(albedo, fuzz);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                } else {
                    // glass
                    sphere_material = make_shared<dielectric>(1.5);
                    world.add(make_shared<sphere>(center, 0.2, sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<dielectric>(1.5);
    world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
    world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, material3));
    
    camera cam;

    // Samples can be lowered to make faster
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 1200;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.vfov     = 20;
    cam.lookfrom = point3(13,2,3);
    cam.lookat   = point3(0,0,0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(world);
}