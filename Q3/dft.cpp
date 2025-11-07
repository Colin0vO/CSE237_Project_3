#include "dft.h"
#include "coefficients32_2D.h"

void dft(DTYPE in_R[SIZE], DTYPE in_I[SIZE],
         DTYPE out_R[SIZE], DTYPE out_I[SIZE]) {

    for (int k = 0; k < SIZE; k++) {
        DTYPE sum_R = 0.0f;
        DTYPE sum_I = 0.0f;

        for (int n = 0; n < SIZE; n++) {
            DTYPE xr = in_R[n];
            DTYPE xi = in_I[n];

            DTYPE c = cos_coeff_table[k][n];
            DTYPE s = sin_coeff_table[k][n];

            sum_R += xr * c - xi * s;
            sum_I += xr * s + xi * c;
        }

        out_R[k] = sum_R;
        out_I[k] = sum_I;
    }
}
