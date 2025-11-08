#include "dft.h"
#include "coefficients32_2D.h"



void dft(DTYPE XX_R[SIZE], DTYPE XX_I[SIZE])
{
    DTYPE out_real[SIZE];
    DTYPE out_imag[SIZE];

#pragma HLS ARRAY_PARTITION variable=XX_R     block factor=PART_FACTOR dim=1
#pragma HLS ARRAY_PARTITION variable=XX_I     block factor=PART_FACTOR dim=1
#pragma HLS ARRAY_PARTITION variable=out_real block factor=PART_FACTOR dim=1
#pragma HLS ARRAY_PARTITION variable=out_imag block factor=PART_FACTOR dim=1

#pragma HLS ARRAY_PARTITION variable=cos_coeff_table block factor=PART_FACTOR dim=2
#pragma HLS ARRAY_PARTITION variable=sin_coeff_table block factor=PART_FACTOR dim=2

loop_k:
    for (int k = 0; k < SIZE; k++) {
        DTYPE sum_real = 0.0f;
        DTYPE sum_imag = 0.0f;

    loop_n:
        for (int n = 0; n < SIZE; n++) {
#pragma HLS PIPELINE II=1
            DTYPE xr = XX_R[n];
            DTYPE xi = XX_I[n];

            DTYPE c = cos_coeff_table[k][n];
            DTYPE s = sin_coeff_table[k][n];


            sum_real += xr * c - xi * s;
            sum_imag += xr * s + xi * c;
        }

        out_real[k] = sum_real;
        out_imag[k] = sum_imag;
    }

write_back:
    for (int k = 0; k < SIZE; k++) {
#pragma HLS PIPELINE II=1
        XX_R[k] = out_real[k];
        XX_I[k] = out_imag[k];
    }
}
