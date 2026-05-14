#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "segment.h"
#include "antenna.h"

void current_distribution(Segment *Antenna, int size){

    for(int i = 0; i < size; ++i){

            Antenna[i].current = 1.0;
        }
}
