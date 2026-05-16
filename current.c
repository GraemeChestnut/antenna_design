#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib.h"

#include "segment.h"
#include "antenna.h"
#include <complex.h>

#define EPSILON 1e-6

double complex greens_function(double r, double k);

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

double complex greens_function(double r, double k){ //computing green's function with hemolt'z operator, which is used for 3D free space 
    return (cexp(-I*k*r)/(4*M_PI*r));
}
