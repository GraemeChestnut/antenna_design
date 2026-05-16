#ifndef SEGMENT_H
#define SEGMENT_H
#include <complex.h>
typedef struct{
        float start_line[3];
        float end_line[3];
        double complex current;
        double complex voltage;
        float midpoint[3];
} Segment;

void current_distribution(Segment *Antenna, int size, float *voltage);


#endif
