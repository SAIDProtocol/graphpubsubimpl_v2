require(package "graphpubsub");

gnrsCache::GPS_GNRSCache(GNRS_NA 0x242, NA 0x334);
routingTable::GPS_RoutingTable(FILENAME "test.rtg");
arpTable::GPS_ARPTable(FILENAME test.arp);

routingQueue::ThreadSafeQueue(512);
arpQueue::ThreadSafeQueue(512);
outQueue::ThreadSafeQueue(512);

gnrsData::Null
	-> GPS_TEST_PrintPacket(LABEL "[0]gnrsCache.data")
	-> GPS_TEST_PrintAnno("[0]gnrsCache.anno")
    -> [0]gnrsCache
    -> routingQueue;

gnrsRequest::Null
	-> GPS_TEST_PrintPacket(LABEL "[1]gnrsCache.data")
	-> GPS_TEST_PrintAnno("[1]gnrsCache.anno")
    -> [1]gnrsCache;

gnrsResponse::Null
	-> GPS_TEST_PrintPacket(LABEL "[2]gnrsCache.data")
	-> GPS_TEST_PrintAnno("[2]gnrsCache.anno")
    -> [2]gnrsCache;

gnrsCache[1]
	-> GPS_TEST_PrintPacket(LABEL "gnrsCache[1].data")
	-> GPS_TEST_PrintAnno("gnrsCache[1].anno")
	-> Discard;

routingQueue
    -> routingUnqueue::Unqueue
	-> GPS_TEST_PrintPacket(LABEL "[0]routingTable.data")
	-> GPS_TEST_PrintAnno("[0]routingTable.anno")
    -> routingTable
    -> arpQueue;

routingTable[1]
	-> GPS_TEST_PrintPacket(LABEL "routingTable[1].data")
	-> GPS_TEST_PrintAnno("routingTable[1].anno")
	-> Discard;

arpQueue
    -> arpUnqueue::Unqueue
	-> GPS_TEST_PrintPacket(LABEL "[0]arpTable.data")
	-> GPS_TEST_PrintAnno("[0]arpTable.anno")
    -> arpTable
    -> outQueue;

arpTable[1]
	-> GPS_TEST_PrintPacket(LABEL "arpTable[1].data")
	-> GPS_TEST_PrintAnno("arpTable[1].anno")
	-> Discard;

outQueue
    -> outUnqueue::Unqueue
	-> GPS_TEST_PrintPacket(LABEL "arpTable[0].data")
	-> GPS_TEST_PrintAnno("arpTable[0].anno")
    -> Discard;


TimedSource(1, "a", LIMIT 1)
	-> GPS_TEST_GNRSPacketCreator(TYPE 0xc1, GUID 0x1234, SRC_NA 0x241)
	-> gnrsRequest;

TimedSource(2, "aa", LIMIT 2)
	-> GPS_TEST_GNRSPacketCreator(TYPE 0xc2, GUID 0x1234, SRC_NA 0x334, DST_NA 0x241, REQUESTED_GUID_NA 0x134, VERISON 1)
	-> gnrsResponse;

TimedSource(3, "aaaaa", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x1234)
	-> gnrsData;

TimedSource(3.5, "bbbbbb", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x1234)
	-> gnrsData;


TimedSource(5, "bbbbbb", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x1234)
	-> gnrsData;


TimedSource(6, "ccccccc", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x1235)
	-> gnrsData;

TimedSource(7, "", LIMIT 1)
	-> GPS_TEST_GNRSPacketCreator(TYPE 0xc2, GUID 0x1235, SRC_NA 0x242, DST_NA 0x334, REQUESTED_GUID_NA 0x334, VERISON 1)
	-> gnrsResponse;

TimedSource(8, "", LIMIT 1)
	-> GPS_TEST_GNRSPacketCreator(TYPE 0xc2, GUID 0x1236, SRC_NA 0x242, DST_NA 0x334, REQUESTED_GUID_NA 0x334, VERISON 1)
	-> gnrsResponse;

TimedSource(9, "ccccccc", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x1236)
	-> gnrsData;



StaticThreadSched(routingUnqueue 1, arpUnqueue 2, outUnqueue 3);
