// $d: duration waited before stop
// $dev: output device

require(basictest)

gen::BASIC_PubGenerator(ec:0d:9a:7e:90:c6, ec:0d:9a:7e:91:96, 125, 0x27c1)
    -> cnt::Counter
    -> ToDevice($dev);

DriverManager(
    wait $d,
    read gen.packetCount,
    read cnt.count,
    stop
);