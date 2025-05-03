
# Final Project Topic 5: Hardware Design for Cryptographic Primitive - Modular Multiplication

## Overview

This project focuses on designing and optimizing hardware accelerators for **modular multiplication** of the form:

$(a \cdot b) \mod p, \text{where } p = 2^{255} - 19$

Modular multiplication is a **cryptographic primitive**, a fundamental algorithm widely used in cryptographic systems to ensure **data security, integrity, and confidentiality**. 
Examples include
RSA (Rivest-Shamir-Adleman) for public-key encryption,
ECC (Elliptic Curve Cryptography) for secure communications, and
Diffie-Hellman Key Exchange for secure key sharing.

### Why Modular Multiplication in Hardware?

Cryptographic applications demand **high-performance** computation for **real-time security processing**. Software implementations running on general-purpose CPUs are often **too slow**, especially for operations with **large bitwidths** (e.g., 256 bits). Hardware accelerators, particularly those implemented on **FPGAs**, can leverage **parallelism** and **arbitrary precision arithmetic**, achieving **higher performance** and **efficiency**.

## Problem Statement

One interesting feature of this problem is that, given different precisions (bitwidth) of \(a\) and \(b\), the corresponding modular multiplication algorithm (implementation) is different. For CPUs, since only limited data types are supported (16, 32, 64 bits), the solution space is also limited.

However, on FPGA, we can implement **arbitrary data precision**, from **2 to 512 bits**, or even more. This provides a **huge design space** and raises an interesting research question:

- **What is the best bitwidth and hardware design with its corresponding modular multiplication algorithm?**

In this project, the goal is to conduct an **initial study** for the three data types—16, 32, and 64 bits—and explore their **resource** and **latency** trade-offs.

## Project Setup

### Inputs

The repository includes three modular multiplication implementations targeting:
- **16-bit (16.h)**
- **32-bit (32.h)**
- **64-bit (64.h)**

The **test.c** file evaluates performance using two 255-bit input numbers stored as arrays of smaller integers. The iterations for testing can be controlled by variables **i** and **j**.

To compile and test all implementations, run:
```
./build.sh
```

### Requirements

1. **Optimize Each Implementation**
   - Focus on improving **latency** and minimizing **resource usage** (e.g., DSPs, LUTs).
2. **Generate Pareto Curves**
   - Plot trade-offs between **resource utilization** and **performance**.
3. **Compare with CPU Baselines**
   - Measure speedup and efficiency gains relative to the CPU versions.

## Open-Ended Challenge

Manually optimizing three types of precision is already a significant task, but FPGA allows **arbitrary precision**, ranging from **2-bit to 512-bit**, or more. This creates a **vast design space** for both algorithm and hardware implementations.

Key questions include:
- **Can we enable any automation** to explore this design space efficiently?
- **Can we derive insights or conclusions** to guide the search for optimal implementations?

## Deliverables

1. **Optimized Hardware Implementations**
   - Modular multiplication designs for **16-bit**, **32-bit**, and **64-bit** precision.
2. **Performance Benchmarks**
   - Comparison with CPU baselines and FPGA implementations.
3. **Pareto Curves**
   - Visualizing trade-offs between **latency** and **resource utilization**.
4. **Design Report**
   - Analysis of performance trends, insights, and recommendations for future work.
