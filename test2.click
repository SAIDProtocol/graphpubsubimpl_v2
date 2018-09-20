require(package "graphpubsub");

routingTable::GPS_RoutingTable("test.rtg");
arpTable::GPS_ARPTable(FILENAME test.arp);

ts1::TimedSource(1, "aaaaaaa", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x223, DST_NA 0x00000242)
	-> GPS_TEST_PrintPacket(LABEL "[0]RoutingTable1.data")
	-> GPS_TEST_PrintAnno("[0]RoutingTable1.anno")
	-> routingTable
	-> GPS_TEST_PrintPacket(LABEL "RoutingTable[0].data")
	-> GPS_TEST_PrintAnno("RoutingTable[0].anno")
	-> arpTable
	-> GPS_TEST_PrintPacket(LABEL "ArpTable[0].data")
	-> GPS_TEST_PrintAnno("ArpTable[0].anno")
	-> Discard;

routingTable[1]
	-> GPS_TEST_PrintPacket(LABEL "RoutingTable[1].data")
	-> GPS_TEST_PrintAnno("RoutingTable[1].anno")
	-> Discard;

//StaticThreadSched(ts1 1, ts2 2, ts3 3, ts4 4, arpUnqueue 5);
