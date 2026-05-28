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
      
  double beta = (2 * M_PI)/wavelength;

  double complex (*Z)[size] = malloc(size * size * sizeof(double complex));
  double complex (*upper)[size] = calloc(size * size, sizeof(double complex));
  double complex (*lower)[size] = calloc(size * size, sizeof(double complex));
  
   printf("size = %d\n", size);
  printf("bytes needed = %zu\n", size * size * sizeof(double complex));

  if(Z == NULL){
      printf("malloc failed\n");
      return;
  }

  //SET VOLTAGE VALUES
  for(int i = 0; i < size; ++i){
      if(fabs(Antenna[i].start_line[0]) < (EPSILON) &&
        (fabs(Antenna[i].start_line[1]) < (EPSILON))&&
        (fabs(Antenna[i].start_line[2]) < (EPSILON)))
      
     {
          Antenna[i].voltage = *voltage;
      }
      
      //computting R value
      for(int k = 0; k < size; ++k){
        
          double r = 
          sqrt(
          pow(Antenna[i].midpoint[0] - Antenna[k].midpoint[0], 2) +
          pow(Antenna[i].midpoint[1] - Antenna[k].midpoint[1], 2) +
          pow(Antenna[i].midpoint[2] - Antenna[k].midpoint[2], 2)
          );
          
          if (i == k){
            Z[i][k] = greens_function(0.001, beta);
          }
          else{
          Z[i][k] = greens_function(r, beta);
          }
          printf(" r[%d][%d] Z = %f + %f ", i, k, creal(Z[i][k]), cimag(Z[i][k]));


      }

  }

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
      printf("\n");

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
          //printf(" r[%d][%d] Z = %f + %f ", i, k, creal(upper[i][k]), cimag(upper[i][k]));

      }
          
      printf("\n");
    }
        
  free(Z);
  printf("k = %f\n", beta);
printf("wavelength = %f\n", wavelength);



for(int i = 0; i < size; i++) {
    for(int j = 0; j < size; j++) {
        printf("%f + %f ",
               creal(upper[i][j]),
               cimag(upper[i][j]));
    }
    printf("\n");
}

free(Z);
free(lower);
free(upper);
}




double complex greens_function(double r, double beta){ //computing green's function with hemolt'z operator, which is used for 3D free space 
    return (cexp(-I*beta*r)/(4*M_PI*r));
}
