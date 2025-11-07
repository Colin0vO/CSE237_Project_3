// DFT1024 using 1D lookup tables (fits PYNQ-Z2 BRAM)
// logic refering to textbook Figure 4.15, using mod to ensure size of the table
// module is working because sin and cos are periodic
// We define: c = cos(angle); s = sin(-angle) == -sin(angle).
//   real_op[k] += xr*c - xi*s  -> xr*cos(angle) + xi*sin(angle)
//   imag_op[k] += xr*s + xi*c  -> -xr*sin(angle) + xi*cos(angle)
//
// 8(a): 
// optimization choice: loop pipelining + loop unroll + array partition
// I applied loop pipelining to the inner loop to allow multiple iterations to execute concurrently.
// By setting the initiation interval (II) to 1, new iterations can start every clock cycle, which significantly improves throughput and reduces latency.
// I used loop unrolling with a factor of 8 to duplicate the computation units for eight frequency bins.
// This allows eight iterations of the inner loop to be computed in parallel, increasing throughput at the cost of additional DSP and LUT resources.
// After unrolling the loop, I used array partitioning to eliminate memory access bottlenecks.
// Each array was cyclically partitioned into 8 memory banks, enabling eight parallel reads/writes in a single cycle.
// This allowed the unrolled computation units to operate fully in parallel.

// reference: lecture 12 slide 7 - 14 
#include "dft.h"
#include "coefficients1024.h"

void dft(
    DTYPE real_sample[SIZE], DTYPE imag_sample[SIZE], DTYPE real_op[SIZE], DTYPE imag_op[SIZE])
{
#pragma HLS ARRAY_PARTITION variable=cos_coefficients_table cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=sin_coefficients_table cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=real_op cyclic factor=8 dim=1
#pragma HLS ARRAY_PARTITION variable=imag_op cyclic factor=8 dim=1
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
        for (int k = 0; k < SIZE; ++k) {
		#pragma HLS PIPELINE II=1
		#pragma HLS UNROLL factor=8
        	const int idx = (k * n) % SIZE;
        	const DTYPE c = cos_coefficients_table[idx];
        	const DTYPE s = sin_coefficients_table[idx];
        	real_op[k] += xr * c - xi * s;
        	imag_op[k] += xr * s + xi * c;
    	}
    }
}

