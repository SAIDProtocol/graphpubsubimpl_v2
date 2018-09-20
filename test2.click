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


ts1::TimedSource(1, "aaaaaaa", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x223, DST_NA 0x00000242)
	-> routingQueue;

//StaticThreadSched(ts1 1, ts2 2, ts3 3, ts4 4, arpUnqueue 5);
