// $d: duration waited before stop
// $dev: output device
// $count: count

require(package "basictest")

gen::BASIC_PubGenerator(ec:0d:9a:7e:90:c6, ec:0d:9a:7e:91:96, 125, 0x27c1, $count)
    -> BandwidthRatedUnqueue(8Gbps)
//    -> cnt::Counter
    -> ToDPDKDevice(0000:81:00.0, ALLOW_NONEXISTENT false);

DriverManager(
    wait $d,
    read gen.packetCount,
//    read cnt.count,
    stop
);
