// $rtgFile
// $myMac $hostMac
// $rate
// $dev
// $duration
require(package "graphpubsub");

GPS_TEST_PubGenerator(SRC_NA 0x0, SRC_GUID 0x1234, PAYLOAD_SIZE 55, DST_NA_FILE $rtgFile, DURATION $duration)
    //-> GPS_TEST_PrintPacket("PUB: ")
//RandomSource(236)
    -> EtherEncap(0x27c0, $myMac, $hostMac)
    //-> Print("EtherEncap: ")
    -> BandwidthRatedUnqueue($rate)
    -> q::Queue(65536)
    -> ToDevice($dev);

q[1]
    -> Print
    -> Discard;
