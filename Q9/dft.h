#include "ap_axi_sdata.h"
#include "hls_stream.h"

typedef float DTYPE;
typedef hls::axis<DTYPE, 0, 0, 0> axis_t;
#define SIZE 1024

void dft(hls::stream<axis_t> &real_sample,
         hls::stream<axis_t> &imag_sample,
         hls::stream<axis_t> &real_op,
         hls::stream<axis_t> &imag_op);