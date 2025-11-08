#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <math.h>

#include "hls_stream.h"
#include "dft.h"

struct Rmse {
    int   num_sq;
    float sum_sq;
    float error;

    Rmse() : num_sq(0), sum_sq(0.0f), error(0.0f) {}
    float add_value(float d_n) {
        num_sq++;
        sum_sq += d_n * d_n;
        error = sqrtf(sum_sq / num_sq);
        return error;
    }
};

Rmse rmse_R, rmse_I;

int main() {
    hls::stream<axis_t> real_in;
    hls::stream<axis_t> imag_in;
    hls::stream<axis_t> real_out;
    hls::stream<axis_t> imag_out;

    DTYPE In_R[SIZE];
    DTYPE In_I[SIZE];

    int   index;
    float gold_R, gold_I;

    FILE *fp = fopen("out.gold.dat", "r");
    if (!fp) {
        std::cerr << "ERROR: could not open out.gold.dat\n";
        return 1;
    }

    for (int i = 0; i < SIZE; i++) {
        In_R[i] = (DTYPE)i;
        In_I[i] = 0.0f;

        axis_t pr, pi;
        pr.data  = In_R[i];
        pr.keep = -1; pr.strb = -1;
        pi.data  = In_I[i];
        pr.last  = (i == SIZE - 1);
        pi.last  = (i == SIZE - 1);

        real_in.write(pr);
        imag_in.write(pi);
    }

    dft(real_in, imag_in, real_out, imag_out);

    for (int i = 0; i < SIZE; i++) {
        if (real_out.empty() || imag_out.empty()) {
            std::cerr << "ERROR: output stream underflow at i = " << i << "\n";
            fclose(fp);
            return 1;
        }

        axis_t pr = real_out.read();
        axis_t pi = imag_out.read();

        DTYPE out_R = pr.data;
        DTYPE out_I = pi.data;

        if (fscanf(fp, "%d %f %f", &index, &gold_R, &gold_I) != 3) {
            std::cerr << "ERROR: malformed golden file at i = " << i << "\n";
            fclose(fp);
            return 1;
        }

        rmse_R.add_value(out_R - gold_R);
        rmse_I.add_value(out_I - gold_I);

        if (i == SIZE - 1) {
            if (!pr.last || !pi.last) {
                std::cerr << "ERROR: TLAST not asserted on final output sample\n";
                fclose(fp);
                return 1;
            }
        }
    }

    fclose(fp);

    printf("----------------------------------------------\n");
    printf("   RMSE(R)           RMSE(I)\n");
    printf("%0.15f %0.15f\n", rmse_R.error, rmse_I.error);
    printf("----------------------------------------------\n");

    if (rmse_R.error > 0.1f || rmse_I.error > 0.1f) {
        printf("*******************************************\n");
        printf("FAIL: Output DOES NOT match the golden output\n");
        printf("*******************************************\n");
        return 1;
    } else {
        printf("*******************************************\n");
        printf("PASS: The output matches the golden output!\n");
        printf("*******************************************\n");
        return 0;
    }
}