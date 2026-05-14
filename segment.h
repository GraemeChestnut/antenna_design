#ifndef SEGMENT_H
#define SEGMENT_H

typedef struct{
        float start_line[3];
        float end_line[3];
        float current;
} Segment;

void current_distribution(Segment *Antenna, int size);

#endif
