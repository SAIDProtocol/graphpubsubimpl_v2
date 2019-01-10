// $d: duration waited before stop

require(basictest)

gen::BASIC_PubGenerator(ec:0d:9a:7e:90:c6, ec:0d:9a:7e:91:96, 125, 0x27c1)
    -> BandwidthRatedUnqueue(2Gbps)
    -> Discard;

DriverManager(
    wait $d,
    read gen.packetCount,
    stop
);