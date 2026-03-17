#!/bin/bash

BENCHMARKS=(
    "2mm"
    # "3mm"
    # "adi"
    # "aes"
    # "atax"
    # "atax-medium"
    # "bicg"
    # "bicg-large"
    # "bicg-medium"
    # "correlation"
    # "covariance"
    # "doitgen"
    # "doitgen-red"
    # "fdtd-2d"
    # "fdtd-2d-large"
    # "gemm-blocked"
    # "gemm-ncubed"
    # "gemm-p"
    # "gemm-p-large"
    # "gemver"
    # "gemver-medium"
    # "gesummv"
    # "gesummv-medium"
    # "heat-3d"
    # "jacobi-1d"
    # "jacobi-2d"
    # "md"
    # "mvt"
    # "mvt-medium"
    # "nw"
    # "seidel-2d"
    # "spmv-crs"
    # "spmv-ellpack"
    # "stencil-3d"
    # "stencil_stencil2d"
    # "symm"
    # "symm-opt"
    # "symm-opt-medium"
    # "syr2k"
    # "syrk"
    # "trmm"
    # "trmm-opt"
)

echo "🚀 Multi-benchmark DSE launcher"
echo "Total benchmarks: ${#BENCHMARKS[@]}"
echo ""

SUCCESS=0
FAILED=0
START=$(date +%s)

for BENCHMARK in "${BENCHMARKS[@]}"; do
    echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
    echo "🎯 Running: $BENCHMARK"

    BENCH_START=$(date +%s)
    export BENCHMARK_NAME="$BENCHMARK"

    if ./local_single_run.sh; then
        BENCH_END=$(date +%s)
        echo "✅ $BENCHMARK completed in $((BENCH_END - BENCH_START))s"
        ((SUCCESS++))
    else
        BENCH_END=$(date +%s)
        echo "❌ $BENCHMARK failed after $((BENCH_END - BENCH_START))s"
        ((FAILED++))
    fi
    echo ""
done

END=$(date +%s)
echo "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━"
echo "📊 Summary: ✅ $SUCCESS | ❌ $FAILED | ⏱️  $((END - START))s"

[ $FAILED -eq 0 ] && exit 0 || exit 1
