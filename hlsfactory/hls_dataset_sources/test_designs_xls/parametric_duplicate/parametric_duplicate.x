fn double(value: u32) -> u32 {
    value * u32:2
}

fn duplicate<N: u32, M: u32 = {double(N)}>(value: bits[N]) -> bits[M] {
    value ++ value
}

pub fn duplicate_byte(value: u8) -> u16 {
    duplicate(value)
}
