pub fn reverse_bytes(value: u32) -> u32 {
    let byte_0 = value[0:8];
    let byte_1 = value[8:16];
    let byte_2 = value[16:24];
    let byte_3 = value[24:32];
    byte_0 ++ byte_1 ++ byte_2 ++ byte_3
}
