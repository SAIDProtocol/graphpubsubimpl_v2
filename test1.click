require(package "graphpubsub");


subQueue::ThreadSafeQueue(65536);
arpQueue::ThreadSafeQueue(65536);
outQueue::ThreadSafeQueue(65536);

subTable::GPS_SubscriptionTable(FILENAME test.sub);
arpTable::GPS_ARPTable(FILENAME test.arp)

subQueue
	-> subUnqueue::Unqueue
	-> GPS_TEST_PrintPacket(LABEL "[0]SubTable.data")
	-> GPS_TEST_PrintAnno("[0]SubTable.anno")
	-> subTable
	-> arpQueue;

arpQueue
	-> arpUnqueue::Unqueue
	-> GPS_TEST_PrintAnno("[0]arpTable.anno")
	-> arpTable
	-> outQueue;

outQueue
	-> outUnqueue::Unqueue
	-> GPS_TEST_PrintAnno("ArpTable[0].anno")
	-> Discard

subTable[1]
	-> GPS_TEST_PrintPacket(LABEL "SubTable[1].data")
	-> GPS_TEST_PrintAnno("SubTable[1].anno")
	-> Discard;

subTable[2]
	-> GPS_TEST_PrintPacket(LABEL "SubTable[2].data")
	-> GPS_TEST_PrintAnno("SubTable[2].anno")
	-> Discard;

arpTable[1]
	-> GPS_TEST_PrintPacket(LABEL "arpTable[1].data")
	-> GPS_TEST_PrintAnno("arpTable[1].anno")
	-> Discard;


TimedSource(1, "aaaaaaa", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x223, SRC_NA 0x225)
	-> subQueue;

TimedSource(2, "xxxxxxxxx", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PrintPacket(LABEL "[1]SubTable.data")
	-> [1]subTable;

TimedSource(3, "bbbbbbbb", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x123, SRC_NA 0x225)
	-> subQueue;

TimedSource(4, "rrrrrr", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x225, SRC_NA 0x225)
	-> subQueue;

TimedSource(5, "rrrrr", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x226, SRC_NA 0x225)
	-> subQueue;


StaticThreadSched(subUnqueue 1, arpUnqueue 2, outUnqueue 3);
