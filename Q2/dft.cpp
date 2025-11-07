#include "dft.h"
#include "coefficients32_2D.h"

void dft(DTYPE XX_R[SIZE], DTYPE XX_I[SIZE]) {
    DTYPE temp_R[SIZE];
    DTYPE temp_I[SIZE];

    for (int k = 0; k < SIZE; k++) {
        DTYPE sum_R = 0.0f;
        DTYPE sum_I = 0.0f;

        for (int n = 0; n < SIZE; n++) {
            DTYPE xr = XX_R[n];
            DTYPE xi = XX_I[n];

            DTYPE c = cos_coeff_table[k][n];
            DTYPE s = sin_coeff_table[k][n];

            sum_R += xr * c - xi * s;
            sum_I += xr * s + xi * c;
        }

        temp_R[k] = sum_R;
        temp_I[k] = sum_I;
    }

    for (int k = 0; k < SIZE; k++) {
        XX_R[k] = temp_R[k];
        XX_I[k] = temp_I[k];
    }
}
