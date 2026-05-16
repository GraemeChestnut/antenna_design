#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib.h"

#include "segment.h"
#include "antenna.h"
#include <complex.h>

#define EPSILON 1e-6

void current_distribution(Segment *Antenna, int size, float *voltage){
      
  //SET VOLTAGE VALUES
  for(int i = 0; i < size; ++i){
      if(fabs(Antenna[i].start_line[0]) < (EPSILON) &&
      (fabs(Antenna[i].start_line[0]) < (EPSILON))&&
      (fabs(Antenna[i].start_line[0]) < (EPSILON))){

          Antenna[i].voltage = *voltage;

      }
  }

}
