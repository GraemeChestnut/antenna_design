#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib.h"

#include "segment.h"
#include "antenna.h"
#include <complex.h>

#define EPSILON 1e-6


double complex greens_function(double r, double k);
double compute_R(float *midpoint_m, float *midpoint_n);

void current_distribution(Segment *Antenna, int size, float *voltage, double wavelength){
      
  double k = (2 * M_PI)/wavelength;\

  double Z[size][size];
  //SET VOLTAGE VALUES
  for(int i = 0; i < size; ++i){
      if(fabs(Antenna[i].start_line[0]) < (EPSILON) &&
        (fabs(Antenna[i].start_line[1]) < (EPSILON))&&
        (fabs(Antenna[i].start_line[2]) < (EPSILON)))
      
      {
          Antenna[i].voltage = *voltage;
      }
      
      //computting R value
      for(int o = 0; o < size; ++o){
        
        double r = 
        sqrt(
        pow(Antenna[i].midpoint[0] - Antenna[o].midpoint[0], 2) +
        pow(Antenna[i].midpoint[1] - Antenna[o].midpoint[1], 2) +
        pow(Antenna[i].midpoint[2] - Antenna[o].midpoint[2], 2)
        );

        Z[i][o] = greens_function(r, k);
      }
  }
}




double complex greens_function(double r, double k){ //computing green's function with hemolt'z operator, which is used for 3D free space 
    return (cexp(-I*k*r)/(4*M_PI*r));
}
