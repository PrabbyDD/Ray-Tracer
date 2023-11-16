#ifndef INTERVAL_H
#define INTERVAL_H

#include "rtweekend.h"
/*
    This class manages real valued intervals with a min and max value
*/
class interval {
    public: 
        double min, max; 

        // Empty Constructor
        interval() : min(+infinity), max(-infinity) {}

        interval(double _min, double _max) : min(_min), max(_max) {}

        bool contains(double x) const { return (x >= min && x <= max); }

        bool surrounds(double x) const { return (x > min && x < max); }

        double clamp(double x) const {
            if (x < min) return min;
            if (x > max) return max; 
            return x; 
        }

        static const interval empty, universe; 
};

const static interval empty (+infinity, -infinity);
const static interval universe (-infinity, +infinity); 

#endif