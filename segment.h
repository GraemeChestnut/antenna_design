#ifndef SEGMENT_H
#define SEGMENT_H
#include <complex.h>
#include "raylib.h"
typedef struct{
        float start_line[3];
        float end_line[3];
        float midpoint[3];
        float radius;
        double complex current;
        double complex voltage;
        Color color;
} Segment;

void current_distribution(Segment *Antenna, int size, float *voltage, double wavelength);


#endif
