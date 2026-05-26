#ifndef ANTENNA_H
#define ANTENNA_H

#include "segment.h"

Segment* init_antenna(int *n, float *voltage, double *wavelength);
int edit_antenna(void);

#endif
