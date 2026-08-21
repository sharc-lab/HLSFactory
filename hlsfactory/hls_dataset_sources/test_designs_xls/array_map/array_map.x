fn square(value: u8) -> u16 {
    (value as u16) * (value as u16)
}

pub fn square_array(values: u8[4]) -> u16[4] {
    map(values, square)
}
