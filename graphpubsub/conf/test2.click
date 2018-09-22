require(package "graphpubsub");


routingQueue::ThreadSafeQueue(65536);
arpQueue::ThreadSafeQueue(65536);
outQueue::ThreadSafeQueue(65536);

routingTable::GPS_RoutingTable(FILENAME test.rtg);
arpTable::GPS_ARPTable(FILENAME test.arp);

routingQueue
    -> routingUnqueue::Unqueue
	-> GPS_TEST_PrintPacket(LABEL "[0]RoutingTable.data")
	-> GPS_TEST_PrintAnno("[0]RoutingTable.anno")
	-> routingTable
	-> arpQueue;

arpQueue
    -> arpUnqueue::Unqueue
	-> GPS_TEST_PrintPacket(LABEL "[0]ArpTable.data")
	-> GPS_TEST_PrintAnno("[0]ArpTable.anno")
	-> arpTable
	-> outQueue;

outQueue
    -> outUnqueue::Unqueue
	-> GPS_TEST_PrintPacket(LABEL "ArpTable[0].data")
	-> GPS_TEST_PrintAnno("ArpTable[0].anno")
	-> Discard;

routingTable[1]
	-> GPS_TEST_PrintPacket(LABEL "RoutingTable[1].data")
	-> GPS_TEST_PrintAnno("RoutingTable[1].anno")
	-> Discard;

arpTable[1]
	-> GPS_TEST_PrintPacket(LABEL "arpTable[1].data")
	-> GPS_TEST_PrintAnno("arpTable[1].anno")
	-> Discard;


TimedSource(1, "aaaaaaa", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x223, DST_NA 0x242)
	-> routingQueue;

TimedSource(2, "aaaaaaaa", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x223, DST_NA 0x134)
	-> routingQueue;

TimedSource(3, "aaaaaaaaa", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x223, DST_NA 0x135)
	-> routingQueue;

TimedSource(4, "aaaaaaaaaa", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x223, DST_NA 0x136)
	-> routingQueue;


StaticThreadSched(routingUnqueue 1, arpUnqueue 2, outUnqueue 3);
