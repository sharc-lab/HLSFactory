pub fn match_alu(lhs: u16, rhs: u16, operation: u2) -> u16 {
    match operation {
        u2:0 => lhs + rhs,
        u2:1 => lhs ^ rhs,
        u2:2 => lhs & rhs,
        _ => lhs | rhs,
    }
}
