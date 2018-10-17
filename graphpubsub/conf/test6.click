require(package "graphpubsub");

GPS_TEST_PubGenerator(SRC_NA 0x0, SRC_GUID 0x1234, PAYLOAD_SIZE 69, DST_NA_FILE test.rtg)
    -> GPS_TEST_PrintPacket("PUB: ")
    -> EtherEncap(0x27c0, 1-2-3-4-5-6, 7-8-9-0-a-b)
    -> Print("EtherEncap: ")
    -> BandwidthRatedUnqueue(1kbps, BURST_BYTES 0)
//    -> routingTable::GPS_RoutingTable(FILENAME test.rtg)
//    -> arpTable::GPS_ARPTable(FILENAME test.arp)
//    -> GPS_TEST_PrintAnno("ARP: ")
//    -> GPS_EtherEncap(MY_ETHER 1-2-3-4-5-6)
//    -> Print("EtherEncap: ")
    -> Discard;

//arpTable[1]
//    -> Print("ARP[1]: ")
//    -> Discard;