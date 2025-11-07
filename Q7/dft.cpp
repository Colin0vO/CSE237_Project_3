// DFT1024 using 1D lookup tables (fits PYNQ-Z2 BRAM)
// logic refering to textbook Figure 4.15, using mod to ensure size of the table
// module is working because sin and cos are periodic
// We define: c = cos(angle); s = sin(-angle) == -sin(angle).
//   real_op[k] += xr*c - xi*s  -> xr*cos(angle) + xi*sin(angle)
//   imag_op[k] += xr*s + xi*c  -> -xr*sin(angle) + xi*cos(angle)


#include "dft.h"
#include "coefficients1024.h"

void dft(
    DTYPE real_sample[SIZE], DTYPE imag_sample[SIZE], DTYPE real_op[SIZE], DTYPE imag_op[SIZE])
{
    // Loop over output frequency bins k
	// since we changed the order of the loop, we need to have a outside loop to initialize real_op and imag_op
    for (int k = 0; k < SIZE; ++k) {
        real_op[k] = 0.0f;
        imag_op[k] = 0.0f;
    }

    // loop over time index n
    for (int n = 0; n < SIZE; ++n) {
        const DTYPE xr = real_sample[n];
        const DTYPE xi = imag_sample[n];
		
		// In Q7 we switch the order of the loop, this will increase the parallelism to have a better performance
        // we didn't include pragma like pipelining and unrolling because Q7 only asked for implement the interchange the two loops
        // we will leave those optimazation in q8
        for (int k = 0; k < SIZE; ++k) {
        	const int idx = (k * n) % SIZE;
        	const DTYPE c = cos_coefficients_table[idx];
        	const DTYPE s = sin_coefficients_table[idx];
        	real_op[k] += xr * c - xi * s;
        	imag_op[k] += xr * s + xi * c;
    	}
    }
}

