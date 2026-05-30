#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib.h"

#include "segment.h"
#include "antenna.h"
#include <complex.h>

#define EPSILON 1e-6

double complex efie_kernel(double r, double k_variable, Vector3 s_hat, Vector3 s_hat_prime);
double compute_R(float *midpoint_m, float *midpoint_n);

void current_distribution(Segment *Antenna, int size, float *voltage, double wavelength){
      
  int voltage_point;
  double k_variable = (2 * M_PI)/wavelength;

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

      printf("%f, %f, %f", Antenna[i].tangent[0], Antenna[i].tangent[1], Antenna[i].tangent[2]);


       
      if(fabs(Antenna[i].start_line[0]) < (EPSILON) &&
        (fabs(Antenna[i].start_line[1]) < (EPSILON))&&
        (fabs(Antenna[i].start_line[2]) < (EPSILON)))
      
     {
          voltage_point = i;
          Antenna[i].voltage = *voltage;
      }
      else Antenna[i].voltage = 0;
      
      //computting R value
      for(int k = 0; k < size; ++k){
        
          double r = 
          sqrt(
          pow(Antenna[i].midpoint[0] - Antenna[k].midpoint[0], 2) +
          pow(Antenna[i].midpoint[1] - Antenna[k].midpoint[1], 2) +
          pow(Antenna[i].midpoint[2] - Antenna[k].midpoint[2], 2)
          );
          if(r == 0) r = EPSILON;

          if (i == k){
            Z[i][k] = efie_kernel(0.001, k_variable, (Vector3){Antenna[i].tangent[0], Antenna[i].tangent[1], Antenna[i].tangent[2]} , 
                                               (Vector3){Antenna[k].tangent[0], Antenna[k].tangent[1], Antenna[k].tangent[2]});
          }
          else{
             Z[i][k] = efie_kernel(r, k_variable, (Vector3){Antenna[i].tangent[0], Antenna[i].tangent[1], Antenna[i].tangent[2]} , 
                                            (Vector3){Antenna[k].tangent[0], Antenna[k].tangent[1], Antenna[k].tangent[2]});
          }
         // printf(" r[%d][%d] Z = %f + %f ", i, k, creal(Z[i][k]), cimag(Z[i][k]));


      }

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
          //printf(" r[%d][%d] Z = %f + %f ", i, k, creal(upper[i][k]), cimag(upper[i][k]));

      }
          
     // printf("\n");
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


    //DISPLAY COLOR FOR EACH SEGMENT BASED ON MAGNITUDE OF CURRENT
    int max;
    for(int i = 0; i < size; i++){
        printf("I[%d] = %.12e + %.12ej\n", i, creal(Iant[i]), cimag(Iant[i]));
        Antenna[i].current = Iant[i];


        if(creal(Antenna[i].voltage) > 0){
          Antenna[i].color = (Color){255,0,0,255};
          max = i;
        }
        if (creal(Antenna[i].current) <= 0.000001){
          Antenna[i].color = (Color){0,255,0,255};
        }
        else {
          Antenna[i].color = (Color){255,255,0,255};
        }
    }
        
  printf("k = %f\n", k_variable);
  printf("wavelength = %f\n", wavelength);





  double complex Z_in = Antenna[voltage_point].voltage / Iant[voltage_point];
  printf("Input Impedance = %f + %fj ohms\n", creal(Z_in), cimag(Z_in));

  free(Z);
  free(lower);
  free(upper);
  
}




double complex efie_kernel(double r, double k_variable, Vector3 s_hat, Vector3 s_hat_prime){ //computing green's function with hemolt'z operator, which is used for 3D free space 
   

  double dot_product_tangent = 
    
    s_hat.x * s_hat_prime.x + 
    s_hat.y * s_hat_prime.y + 
    s_hat.z * s_hat_prime.z;   

  dot_product_tangent = k_variable * k_variable * dot_product_tangent;


  return dot_product_tangent * (cexp(-I*k_variable*r)/(r));
}
