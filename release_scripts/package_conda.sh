#!/usr/bin/env sh
rattler-build build --recipe ./recipe/meta.yaml --recipe-dir ./recipe --output-dir ./output
rm -rf output/bld
rm -rf output/src_cache
rm -rf output/linux-64
conda index ./output/