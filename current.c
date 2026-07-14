#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "raylib.h"

#include "segment.h"
#include "antenna.h"
#include <complex.h>

#define EPSILON 1e-6
#define LGWT_EPS 1e-14

double dot(float a[3], float b[3])
{
    return a[0] * b[0]
         + a[1] * b[1]
         + a[2] * b[2];
}


void lgwt(int N, double a, double b, double *x, double *w);
double segment_length(Segment s);

void current_distribution(Segment *Antenna, int size, float *voltage, double wavelength, int num_gauss, int number_of_children){

  printf("%f", Antenna[0].radius);

  const double c = 299792458;
  const double frequency = c/wavelength;
  const double mu = 4e-7 * M_PI;
  const double epsilon = 1.0/(mu*c*c);
  const double w = 2.0 * M_PI * frequency;
  const double k = w*sqrt(mu*epsilon);
  const double eta = sqrt(mu/epsilon);
  const double wire_radius = Antenna[0].radius;

  printf("\n\nhello mom, number of children %d", number_of_children);
  

for(int i=0;i<size;i++)
{
    printf("%d tangent %f %f %f\n",
        i,
        Antenna[i].tangent[0],
        Antenna[i].tangent[1],
        Antenna[i].tangent[2]);
}

  int total_elements = size;
  printf("\n%d\n", size);
  double delta_z = sqrt(
      pow(Antenna[0].end_line[0] - Antenna[0].start_line[0],2)+
      pow(Antenna[0].end_line[1] - Antenna[0].start_line[1],2)+
      pow(Antenna[0].end_line[2] - Antenna[0].start_line[2],2)
      );

  //genertaie gauss points
  
  double *gauss_z = malloc(num_gauss * sizeof(double));
  double *gauss_w = malloc(num_gauss * sizeof(double));

  lgwt(num_gauss,0.0, 1.0, gauss_z, gauss_w);
  //normalize guass intervals
  for(int i=0;i<num_gauss;i++)
  {
      gauss_z[i] *= delta_z;
      gauss_w[i] *= delta_z;
  }

  //self term
  double r2 = wire_radius * wire_radius;

  double temp =
      sqrt(1.0 + 4.0*r2/(delta_z*delta_z));

  double num = temp + 1.0;

  double denom = temp - 1.0;

  double complex self =
      (log(num/denom) - I*k*delta_z)
      /
      (4.0*M_PI);


  double complex (*A)[size] = malloc(size * size * sizeof(double complex));




//----------------------------------------------------------
//

for(int m = 0; m < size; m++)
{
    for(int n = 0; n < size; n++)
    {

        double orientation =
            dot(
                Antenna[m].tangent,
                Antenna[n].tangent
            );


        if(m == n)
        {
            A[m][n] =
                self *
                I*w*mu *
                orientation;

        }
        else
        {

            double complex term = 0.0 + 0.0*I;


            double obs_x = Antenna[m].midpoint[0];
            double obs_y = Antenna[m].midpoint[1];
            double obs_z = Antenna[m].midpoint[2];


            for(int g = 0; g < num_gauss; g++)
            {

                double source_x =
                    Antenna[n].start_line[0]
                    +
                    Antenna[n].tangent[0]
                    *
                    gauss_z[g];


                double source_y =
                    Antenna[n].start_line[1]
                    +
                    Antenna[n].tangent[1]
                    *
                    gauss_z[g];


                double source_z =
                    Antenna[n].start_line[2]
                    +
                    Antenna[n].tangent[2]
                    *
                    gauss_z[g];


                double dx =
                    obs_x - source_x;

                double dy =
                    obs_y - source_y;

                double dz =
                    obs_z - source_z;


                double R =
                    sqrt(
                        dx*dx +
                        dy*dy +
                        dz*dz 
                    );


                term +=
                    cexp(-I*k*R)
                    /
                    R
                    *
                    gauss_w[g];

            }


            
            A[m][n] =
                I*w*mu
                *
                orientation
                *
                term
                /
                (4.0*M_PI);

        }
    }
}


printf("%e %e\n", creal(A[0][1]), cimag(A[0][1]));
printf("%e %e\n", creal(A[1][0]), cimag(A[1][0]));

//
//----------------------------------------------------------------------
double complex *V = calloc(size, sizeof(double complex));

// find first segment starting at origin
//
// create delta gap source, 
//
// NOTE THIS ONLY WORKS WHEN TWO SEGMENTS ARE POSITIONED AT THE FEED POINT, OTHERWISE, IT WILL BREAK DOWN
bool is_feed_segment = false;

for(int i = 0; i < size; i++)
{
    double x = Antenna[i].start_line[0];
    double y = Antenna[i].start_line[1];
    double z = Antenna[i].start_line[2];

    if(fabs(x) < 1e-9 &&
       fabs(y) < 1e-9 &&
       fabs(z) < 1e-9)
    {
        if (is_feed_segment == false)
        {
          V[i] = 0.5;
        }
        else {
          V[i] = -0.5;
        }

        is_feed_segment = true;
    }
}


if(is_feed_segment == -1)
{
    printf("No feed segment found!\n");
    return;
}


printf("Voltage vector:\n");

for(int i = 0; i < size; i++)
{
    if(cabs(V[i]) > 0.0)
    {
        printf("V[%d] = %f + %fj\n",
               i,
               creal(V[i]),
               cimag(V[i]));
    }
}

printf("------------------\n");

//------------------------

  double complex (*upper)[size] = calloc(size * size, sizeof(double complex));
  double complex (*lower)[size] = calloc(size * size, sizeof(double complex));
  
  if(A == NULL){
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
          upper[i][k] = A[i][k] - sum_U;
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
              lower[k][i] = ((A[k][i] - sum_L) / upper[i][i]);
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
        y[i]=V[i];
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


  // END LU


printf("\n--- LU DIAGONAL CHECK ---\n");

for(int i=0;i<size;i++)
{
    printf("U[%d][%d] = %.6e + %.6ei\n",
        i,
        i,
        creal(upper[i][i]),
        cimag(upper[i][i]));
} 


  free(A);
  free(lower);
  free(upper);

for(int i=0;i<size;i++)
{
    printf("%d %.8e %.8f\n",
        i,
        cabs(Iant[i]),
        carg(Iant[i])*180/M_PI);
}


} 





double segment_length(Segment s)
{
    double dx = s.end_line[0] - s.start_line[0];
    double dy = s.end_line[1] - s.start_line[1];
    double dz = s.end_line[2] - s.start_line[2];

    return sqrt(dx*dx + dy*dy + dz*dz);
}





void lgwt(int N, double a, double b, double *x, double *w)
{
    int N1 = N;
    int N2 = N + 1;

    double *y  = malloc(N * sizeof(double));
    double *y0 = malloc(N * sizeof(double));

    double *L  = malloc(N * (N+2) * sizeof(double));
    double *Lp = malloc(N * (N+2) * sizeof(double));

    double *xu = malloc(N * sizeof(double));


    // linspace(-1,1,N)
    for(int i=0;i<N;i++)
    {
        xu[i] = -1.0 + 2.0*i/(N-1);
    }


    // Initial guess
    for(int i=0;i<N;i++)
    {
        y[i] =
            cos((2.0*i+1.0)*M_PI/(2.0*N))
            +
            (0.27/N) *
            sin(M_PI*xu[i]*(N-1)/(N+1));
    }


    double error = 1.0;


    // Newton-Raphson iteration
    while(error > LGWT_EPS)
    {

        for(int i=0;i<N;i++)
        {
            L[i*(N+2)+0] = 1.0;
            Lp[i*(N+2)+0] = 0.0;


            L[i*(N+2)+1] = y[i];
            Lp[i*(N+2)+1] = 1.0;


            // Legendre polynomial recursion
            for(int k=2;k<=N;k++)
            {
                L[i*(N+2)+k] =
                (
                    (2.0*k-1.0)
                    *
                    y[i]
                    *
                    L[i*(N+2)+k-1]
                    -
                    (k-1.0)
                    *
                    L[i*(N+2)+k-2]
                ) / k;
            }


            // Derivative
            Lp[i*(N+2)+N] =
            N *
            (
                L[i*(N+2)+N-1]
                -
                y[i]*L[i*(N+2)+N]
            )
            /
            (1.0-y[i]*y[i]);
        }


        error = 0.0;


        // Newton update
        for(int i=0;i<N;i++)
        {
            y0[i] = y[i];


            y[i] =
                y[i]
                -
                L[i*(N+2)+N]
                /
                Lp[i*(N+2)+N];


            double diff=fabs(y[i]-y0[i]);

            if(diff>error)
                error=diff;
        }

    }



    // Map [-1,1] to [a,b]
    for(int i=0;i<N;i++)
    {
        x[i] =
            (a*(1.0-y[i])
            +
            b*(1.0+y[i]))
            /
            2.0;


        // weights
        w[i] =
            (b-a)
            /
            (
                (1.0-y[i]*y[i])
                *
                Lp[i*(N+2)+N]
                *
                Lp[i*(N+2)+N]
            );
    }

for(int i = 0; i < N/2; i++)
{
    int j = N - 1 - i;

    double temp;

    temp = x[i];
    x[i] = x[j];
    x[j] = temp;

    temp = w[i];
    w[i] = w[j];
    w[j] = temp;
}


    free(y);
    free(y0);
    free(L);
    free(Lp);
    free(xu);
}

