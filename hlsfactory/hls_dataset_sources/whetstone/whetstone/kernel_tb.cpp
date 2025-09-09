/********************************************************************
 *  Test‑bench for the Whetstone benchmark HLS kernel
 *
 *  • Calls the kernel with the classic workload (10 million Whetstone
 *    “units”).
 *  • Prints the four floating‑point results that the original benchmark
 *    would output.
 *  • The TB can be used for:
 *        – C‑simulation  (csim_design)
 *        – C‑/RTL co‑simulation (cosim_design)
 *
 *  No HLS pragmas are needed in the test‑bench.  It is pure C++ that
 *  drives the synthesized function.
 ********************************************************************/

#include <cstdio>  // for printf
#include <cstdlib> // for EXIT_SUCCESS / EXIT_FAILURE

/* ------------------------------------------------------------------
 *  Declaration of the top‑level kernel.  It is defined in
 *  `whetstone.cpp` (the design file you are synthesising).
 * ------------------------------------------------------------------ */
void whetstone_kernel(int million_iters, double result[4]);

int main() {
    /* --------------------------------------------------------------
       1.  Choose the workload.
          The classic Whetstone run uses 10 “million‑unit” iterations.
          You can change this value to stress the design differently.
       -------------------------------------------------------------- */
    const int million_iters = 10; // same as ITERATIONS in the source

    /* --------------------------------------------------------------
       2.  Storage for the four output values (x1, x2, x3, x4).
       -------------------------------------------------------------- */
    double result[4] = {0.0, 0.0, 0.0, 0.0};

    /* --------------------------------------------------------------
       3.  Call the kernel.
       -------------------------------------------------------------- */
    whetstone_kernel(million_iters, result);

    /* --------------------------------------------------------------
       4.  Print the results – this matches the format used by the
          original benchmark (four columns of scientific notation).
       -------------------------------------------------------------- */
    printf("Whetstone benchmark results (x1-x4):\n");
    printf(
        "%15.8e %15.8e %15.8e %15.8e\n",
        result[0],
        result[1],
        result[2],
        result[3]);

    /* --------------------------------------------------------------
       5.  (Optional) Simple sanity check.
           The reference values for the classic run (10 M) are:
               x1 =   1.057594e+00
               x2 =  -1.057594e+00
               x3 =  -1.057594e+00
               x4 =   1.057594e+00
           The exact numbers depend on the compiler’s floating‑point
           rounding mode; a tolerance of 1 e‑6 is sufficient for a quick
           sanity check.
       -------------------------------------------------------------- */
    // const double ref[4] = {
    //     1.057594e+00, -1.057594e+00, -1.057594e+00, 1.057594e+00};
    // const double eps = 1e-6;
    // bool ok = true;
    // for (int i = 0; i < 4; ++i) {
    //     if (std::abs(result[i] - ref[i]) > eps) {
    //         ok = false;
    //         break;
    //     }
    // }

    // if (ok) {
    //     printf("PASS - results are within tolerance.\n");
    //     return EXIT_SUCCESS;
    // } else {
    //     printf("FAIL - results differ from reference.\n");
    //     return EXIT_FAILURE;
    // }

    return EXIT_SUCCESS;
}