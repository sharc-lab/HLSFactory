proc RunningSum {
    input_values: chan<u32> in;
    output_sums: chan<u32> out;

    config(input_values: chan<u32> in, output_sums: chan<u32> out) {
        (input_values, output_sums)
    }

    init { u32:0 }

    next(state: u32) {
        let (tok, value) = recv(join(), input_values);
        let next_state = state + value;
        send(tok, output_sums, next_state);
        next_state
    }
}
