#include <math.h>
#include "dft.h"

void dft(DTYPE real_sample[SIZE], DTYPE imag_sample[SIZE]) {
    const DTYPE PI = 3.14159265358979323846f;

    // Temporary arrays to avoid overwriting inputs while we still need them
    DTYPE temp_real[SIZE];
    DTYPE temp_imag[SIZE];

    // Outer loop over output frequency bins k
    for (int k = 0; k < SIZE; k++) {
        DTYPE sum_real = 0.0f;
        DTYPE sum_imag = 0.0f;

        // Inner loop over time index n
        for (int n = 0; n < SIZE; n++) {
            DTYPE xr = real_sample[n];
            DTYPE xi = imag_sample[n];

            // DFT kernel: W_N^{kn} = e^{-j 2πkn/N}
            DTYPE angle = (2.0f * PI * k * n) / (DTYPE)SIZE;
            DTYPE c = cosf(angle);       // cos(2πkn/N)
            DTYPE s = -sinf(angle);      // -sin(2πkn/N), matches e^{-jθ}

            // (xr + j xi) * (c + j s) where (c + j s) = e^{-jθ}
            sum_real += xr * c - xi * s;
            sum_imag += xr * s + xi * c;
        }

        temp_real[k] = sum_real;
        temp_imag[k] = sum_imag;
    }

    // Write results back in-place
    for (int k = 0; k < SIZE; k++) {
        real_sample[k] = temp_real[k];
        imag_sample[k] = temp_imag[k];
    }
}
