#include "dft.h"
#include "coefficients1024.h"
#include <math.h>

void dft(hls::stream<axis_t> &real_sample,
         hls::stream<axis_t> &imag_sample,
         hls::stream<axis_t> &real_op,
         hls::stream<axis_t> &imag_op
){
#pragma HLS INTERFACE axis     port=real_sample
#pragma HLS INTERFACE axis     port=imag_sample
#pragma HLS INTERFACE axis     port=real_op
#pragma HLS INTERFACE axis     port=imag_op
#pragma HLS INTERFACE s_axilite port=return bundle=CTRL

    DTYPE real_buf[SIZE];
    DTYPE imag_buf[SIZE];
#pragma HLS bind_storage variable=real_buf type=ram_2p impl=bram
#pragma HLS bind_storage variable=imag_buf type=ram_2p impl=bram

    axis_t xr_last_pkt, xi_last_pkt;
    for (int n = 0; n < SIZE; n++) {
#pragma HLS pipeline II=1
        axis_t rin = real_sample.read();
        axis_t iin = imag_sample.read();
        real_buf[n] = rin.data;
        imag_buf[n] = iin.data;
        xr_last_pkt = rin;
        xi_last_pkt = iin;
    }

    for (int k = 0; k < SIZE; k++) {
#pragma HLS loop_tripcount min=SIZE max=SIZE
        DTYPE sum_real = 0.0f;
        DTYPE sum_imag = 0.0f;

        for (int n = 0; n < SIZE; n++) {
#pragma HLS pipeline II=1
            int idx = (k * n) & (SIZE - 1);

            DTYPE c = cos_coefficients_table[idx];
            DTYPE s = sin_coefficients_table[idx];

            DTYPE a = real_buf[n];
            DTYPE b = imag_buf[n];

            sum_real += a * c - b * s;
            sum_imag += a * s + b * c;
        }

        bool is_last = (k == SIZE - 1);

        axis_t rout = xr_last_pkt;
        axis_t iout = xi_last_pkt;
        
        rout.data = sum_real;
        iout.data = sum_imag;
        rout.last = is_last ? 1 : 0;
        iout.last = is_last ? 1 : 0;

        real_op.write(rout);
        imag_op.write(iout);
    }
}