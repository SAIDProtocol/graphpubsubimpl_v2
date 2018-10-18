// $type
// $file
// $size
// $duration
// $rate
// $myMac, $hostMac
// $dev
require(package "hierarchical")

HRC_TEST_DataGenerator(TYPE $type, NAME_FILE $file, SIZE $size, DURATION $duration)
    -> EtherEncap(0x27c1, $myMac, $hostMac)
    -> BandwidthRatedUnqueue($rate)
    -> q::Queue(65536)
    -> ToDevice($dev);

q[1]
    -> Print
    -> Discard;