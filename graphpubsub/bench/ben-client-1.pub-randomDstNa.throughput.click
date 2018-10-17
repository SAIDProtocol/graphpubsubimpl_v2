// $rtgFile
// $myMac $hostMac
// $rate
// $dev
require(package "graphpubsub");

GPS_TEST_PubGenerator(SRC_NA 0x0, SRC_GUID 0x1234, PAYLOAD_SIZE 55, DST_NA_FILE $rtgFile)
    //-> GPS_TEST_PrintPacket("PUB: ")
    -> EtherEncap(0x27c0, $myMac, $hostMac)
    //-> Print("EtherEncap: ")
    -> BandwidthRatedUnqueue($rate, BURST_BYTES 0)
    -> Queue(16)
    -> ToDevice($dev);
