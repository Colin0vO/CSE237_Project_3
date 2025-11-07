#ifndef DFT_H
#define DFT_H

#define SIZE 32
typedef float DTYPE;

void dft(DTYPE in_R[SIZE], DTYPE in_I[SIZE],
         DTYPE out_R[SIZE], DTYPE out_I[SIZE]);

#endif
