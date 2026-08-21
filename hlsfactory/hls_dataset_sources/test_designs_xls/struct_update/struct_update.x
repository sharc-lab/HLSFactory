struct Point {
    x: u16,
    y: u16,
}

pub fn translate_x(point: Point, delta_x: u16) -> Point {
    Point { x: point.x + delta_x, ..point }
}
