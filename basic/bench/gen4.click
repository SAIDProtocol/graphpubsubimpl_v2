// $d: duration waited before stop
// $dev: output device

require(basictest)

gen::BASIC_PubGenerator(ec:0d:9a:7e:90:c6, ec:0d:9a:7e:91:96, 125, 0x27c1)
    -> uq::BandwidthRatedUnqueue(800Mbps)
    -> q::ThreadSafeQueue(65536)
    -> cnt1::Counter
    -> td::ToDevice($dev);

q[1]
    -> cnt2::Counter
    -> Discard;

DriverManager(
    wait $d,
    read gen.packetCount,
    read cnt1.count,
    read cnt2.count,
    stop
);

StaticThreadSched(gen 0, uq 1);