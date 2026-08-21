type Packet = (u16, u16);

fn pack_packet(packet: Packet) -> u32 {
    packet.0 ++ packet.1
}

fn checksum_step(state: u32, packet: Packet) -> u32 {
    let word = pack_packet(packet);
    (state + word) ^ (word >> u32:3)
}

proc PacketChecksum {
    input_packets: chan<Packet> in;
    output_checksums: chan<u32> out;

    config(input_packets: chan<Packet> in, output_checksums: chan<u32> out) {
        (input_packets, output_checksums)
    }

    init { u32:0 }

    next(state: u32) {
        let (tok, packet) = recv(join(), input_packets);
        let next_checksum = checksum_step(state, packet);
        send(tok, output_checksums, next_checksum);
        next_checksum
    }
}
