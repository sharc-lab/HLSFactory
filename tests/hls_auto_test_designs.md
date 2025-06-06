**Simple Usage**

```cpp
void add(int a, int b, int &c) {
    #pragma HLS_AUTO inline auto{on, off}
    c = a + b;
}
```

**Multiple Independent Pragmas**

```cpp
void fir_filter(const float coeffs[8], const float in[64], float out[64]) {
    #pragma HLS_AUTO pipeline II=auto{1,2,4}


    for (int i = 0; i < 64; ++i) {
        #pragma HLS_AUTO bind_op variable=acc op=fmacc impl=auto{fabric, fulldsp}
        float acc = 0.0f;
        for (int j = 0; j < 8; ++j) {
            acc += coeffs[j] * ((i - j) >= 0 ? in[i - j] : 0.0f);
        }
        out[i] = acc;
    }
}
```

**Tied Pragmas**

```cpp
void vec_add(int a[32], int b[32], int c[32], int n) {
    #pragma HLS_AUTO array_partition variable=a type=cyclic factor=auto{1, 2, 4, 8, 16, 32}[unroll_group]
    #pragma HLS_AUTO array_partition variable=b type=cyclic factor=auto{1, 2, 4, 8, 16, 32}[unroll_group]
    
    for (int i = 0; i < n; i++) {
        #pragma HLS_AUTO unroll factor=auto{1, 2, 4, 8, 16, 32}[unroll_group] 
        c[i] = a[i] + b[i];
    }
}
```
