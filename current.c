#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib.h"

#include "segment.h"
#include "antenna.h"
#include <complex.h>

#define EPSILON 1e-6

double wire_radius = 1e-3;




void current_distribution(Segment *Antenna, int size, float *voltage, double wavelength){

    

  const double c = 299792458;
  const double frequency = c/wavelength;
  const double mu = 4e-7 * M_PI;
  const double epsilon = 1.0/(mu*c*c);
  const double w = 2.0 * M_PI * frequency;
  const double k = w*sqrt(mu*epsilon);
  const double eta = sqrt(mu/epsilon);

  int total_elements = size;
  double delta_z = sqrt(
      pow(Antenna[0].end_line[0] - Antenna[0].start_line[0],2)+
      pow(Antenna[0].end_line[1] - Antenna[0].start_line[1],2)+
      pow(Antenna[0].end_line[2] - Antenna[0].start_line[2],2)
      );

  printf("\n hello\n%f,\n %d", delta_z, size);


  double complex (*Z)[size] = malloc(size * size * sizeof(double complex));
  double complex (*upper)[size] = calloc(size * size, sizeof(double complex));
  double complex (*lower)[size] = calloc(size * size, sizeof(double complex));
  
  if(Z == NULL){
      printf("malloc failed\n");
      return;
  }

    //LU DECOMPOSITION
    //
    //
    for(int i = 0; i < size; ++i){
    
      // upper triangular
      for(int k = i; k < size; k++){

          double complex sum_U = 0 + 0.0*I;

          for(int j = 0; j < i; j++){
              //printf("%f + %fi * %f + %fi\n", creal(lower[i][j]), cimag(lower[i][j]), creal(upper[j][k]), cimag(upper[j][k]));
              sum_U += (lower[i][j] * upper[j][k]);

          }
          //printf("%f + %fi\n", creal(sum_U), cimag(sum_U));
          upper[i][k] = Z[i][k] - sum_U;
      }
      //printf("\n");

      // lower triangular
      for(int k = i; k < size; k++){
          if(i == k){
              lower[i][i] = 1;
          } else {

              double complex sum_L = 0 + 0.0*I;

              for(int j = 0; j < i; j++){
                  sum_L += (lower[k][j] * upper[j][i]);
                  //printf("%f + %fi", creal(sum_L), cimag(sum_L));
              }
              lower[k][i] = ((Z[k][i] - sum_L) / upper[i][i]);
          }
          //printf(" r[%d][%d] Z = %f + %f ", i, k, creal(lower[i][k]), cimag(lower[i][k]));
          //printf(" r[%d][%d] Z = %f + %f \n", i, k, creal(upper[i][k]), cimag(upper[i][k]));

      }
          
     //printf("\n");
    }
    
    //SOLVE UL THROUGH FOWARD/BACK SUB
    //
    double complex *y = calloc(size, sizeof(double complex));
    for(int i = 0; i < size; ++i){
        y[i]=Antenna[i].voltage;
        for(int j = 0; j < i; ++j){
          y[i] -= lower[i][j] * y[j];
        }
    }
    double complex *Iant = malloc(size * sizeof(double complex));
    for(int i = size-1; i >= 0; i--){
        Iant[i] = y[i];
        for(int j = i+1; j < size; j++){
            Iant[i] -= upper[i][j] * Iant[j];
        }
        Iant[i] /= upper[i][i];
    }


   


  free(Z);
  free(lower);
  free(upper);
  
}


double dot(Vector3 a, Vector3 b){
    
    return

    a.x * b.x + 
    a.y * b.y + 
    a.z * b.z; 

}

double segment_length(Segment s)
{
    double dx = s.end_line[0] - s.start_line[0];
    double dy = s.end_line[1] - s.start_line[1];
    double dz = s.end_line[2] - s.start_line[2];

    return sqrt(dx*dx + dy*dy + dz*dz);
}

