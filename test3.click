require(package "graphpubsub");

gnrsCache::GPS_GNRSCache(GNRS_NA 0x242, NA 0x334);
routingTable::GPS_RoutingTable(FILENAME "test.rtg");
arpTable::GPS_ARPTable(FILENAME test.arp);

TimedSource(1, "", LIMIT 1)
	-> GPS_TEST_GNRSPacketCreator(TYPE 0xc1, GUID 0x1234, SRC_NA 0x241)
	-> GPS_TEST_PrintPacket(LABEL "[1]gnrsCache.data")
	-> GPS_TEST_PrintAnno("[1]gnrsCache.anno")
	-> [1] gnrsCache;

TimedSource(2, "", LIMIT 2)
	-> GPS_TEST_GNRSPacketCreator(TYPE 0xc2, GUID 0x1234, SRC_NA 0x334, DST_NA 0x241, REQUESTED_GUID_NA 0x134, VERISON 1)
	-> GPS_TEST_PrintPacket(LABEL "[2]gnrsCache.data")
	-> GPS_TEST_PrintAnno("[2]gnrsCache.anno")
	-> [2] gnrsCache;


TimedSource(3, "aaaaa", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x1234)
	-> GPS_TEST_PrintPacket(LABEL "[0]gnrsCache.data")
	-> GPS_TEST_PrintAnno("[0]gnrsCache.anno")
	-> [0] gnrsCache;

TimedSource(3.5, "bbbbbb", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x1234)
	-> GPS_TEST_PrintPacket(LABEL "[0]gnrsCache.data")
	-> GPS_TEST_PrintAnno("[0]gnrsCache.anno")
	-> [0] gnrsCache;


TimedSource(5, "bbbbbb", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x1234)
	-> GPS_TEST_PrintPacket(LABEL "[0]gnrsCache.data")
	-> GPS_TEST_PrintAnno("[0]gnrsCache.anno")
	-> [0] gnrsCache;


TimedSource(6, "ccccccc", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x1235)
	-> GPS_TEST_PrintPacket(LABEL "[0]gnrsCache.data")
	-> GPS_TEST_PrintAnno("[0]gnrsCache.anno")
	-> [0] gnrsCache;

TimedSource(7, "", LIMIT 1)
	-> GPS_TEST_GNRSPacketCreator(TYPE 0xc2, GUID 0x1235, SRC_NA 0x242, DST_NA 0x334, REQUESTED_GUID_NA 0x334, VERISON 1)
	-> GPS_TEST_PrintPacket(LABEL "[2]gnrsCache2.data")
	-> GPS_TEST_PrintAnno("[2]gnrsCache2.anno")
	-> [2] gnrsCache;


gnrsCache[0]
	-> GPS_TEST_PrintPacket(LABEL "gnrsCache[0].data")
	-> GPS_TEST_PrintAnno("gnrsCache[0].anno")
	-> routingTable
	-> GPS_TEST_PrintPacket(LABEL "routingTable[0].data")
	-> GPS_TEST_PrintAnno("routingTable[0].anno")
	-> arpTable
	-> GPS_TEST_PrintPacket(LABEL "arpTable[0].data")
	-> GPS_TEST_PrintAnno("arpTable[0].anno")
	-> Discard;

gnrsCache[1]
	-> GPS_TEST_PrintPacket(LABEL "gnrsCache[1].data")
	-> GPS_TEST_PrintAnno("gnrsCache[1].anno")
	-> Discard;

gnrsCache[2]
	-> GPS_TEST_PrintPacket(LABEL "gnrsCache[2].data")
	-> GPS_TEST_PrintAnno("gnrsCache[2].anno")
	-> Discard;

