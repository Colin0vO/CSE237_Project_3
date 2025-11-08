#ifndef DFT_H
#define DFT_H

typedef float DTYPE;
#define SIZE 32          


#define PART_FACTOR 32

void dft(DTYPE XX_R[SIZE], DTYPE XX_I[SIZE]);

#endif
