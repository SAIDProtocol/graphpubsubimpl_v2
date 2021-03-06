require(package graphpubsub)

arpTable::GPS_ARPTable(FILENAME $arpFile)
routingTable::GPS_RoutingTable(FILENAME $rtgFile);
outSwitch::GPS_PortSwitch;
outEncap1::GPS_EtherEncap(MY_ETHER $ether);

routingQueue::ThreadSafeQueue(65536);
arpQueue::ThreadSafeQueue(65536);
outQueue1::ThreadSafeQueue(65536);



TimedSource(1, "bbbbbbbbbbbbbbbbbbbbbb", LIMIT $lim, HEADROOM 70, STOP true)
	-> GPS_TEST_PubWrapper(SRC_GUID $srcGuid, SRC_NA $src, DST_GUID_FILE $subFile)
	-> GPS_TEST_PrintPacket(LABEL "pubWrapper[0].data")
	-> routingQueue;

routingQueue
	-> routingUnQueue::Unqueue
	-> routingTable
	-> GPS_TEST_PrintPacket(LABEL "routingTable[0].data")
	-> arpQueue;

arpQueue
	-> arpUnqueue::Unqueue
	-> arpTable
	-> GPS_TEST_PrintPacket(LABEL "arpTable[0].data")
	-> outSwitch[1]
	-> outQueue1;

outQueue1
	-> outEncap1
	-> Print(LABEL "outEncap1[0]", MAXLENGTH 100)
	-> ToDevice($dev);


routingTable[1]
	-> GPS_TEST_PrintPacket(LABEL "routingTable[1].data")
	-> Discard;

arpTable[1]
	-> GPS_TEST_PrintPacket(LABEL "arpTable[1].data")
	-> Discard;

outSwitch[0]
	-> GPS_TEST_PrintPacket(LABEL "outSwitch[0].data")
	-> Discard;

