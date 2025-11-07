// DFT1024 using 1D lookup tables (fits PYNQ-Z2 BRAM)
// logic refering to textbook Figure 4.15, using mod to ensure size of the table
// module is working because sin and cos are periodic
// We define: c = cos(angle); s = -sin(angle). Then
//   real += xr*c - xi*s  => xr*cos(angle) + xi*sin(angle)
//   imag += xr*s + xi*c  => -xr*sin(angle) + xi*cos(angle)
//

#include "dft.h"
#include "coefficients1024.h"

void dft(
    DTYPE real_sample[SIZE], DTYPE imag_sample[SIZE], DTYPE real_op[SIZE], DTYPE imag_op[SIZE])
{
    // Loop over output frequency bins k
    for (int k = 0; k < SIZE; ++k) {
        DTYPE sum_real = 0.0f;
        DTYPE sum_imag = 0.0f;

        // loop over time index n
        for (int n = 0; n < SIZE; ++n) {
            const DTYPE xr = real_sample[n];
            const DTYPE xi = imag_sample[n];

            // 1D LUT index for angle = 2π*(k*n)/N
            // sin_coefficients_table[idx] == sin(-angle) == -sin(angle)
            const int idx = (k * n) % SIZE;    // SIZE = 1024 power-of-two; modulo is cheap in HW
            const DTYPE c = (DTYPE)cos_coefficients_table[idx];     
            const DTYPE s = (DTYPE)sin_coefficients_table[idx];     

            // temp_real[k] += xr*c - xi*s; (since s = -sin(angle) this is xr*cos + xi*sin)
            // temp_imag[k] += (xr*(-sin) + xi*cos)
            sum_real += xr * c - xi * s;
            sum_imag += xr * s + xi * c;
        }

        real_op[k] = sum_real;
        imag_op[k] = sum_imag;
    }
}

