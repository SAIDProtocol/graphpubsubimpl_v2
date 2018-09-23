require(package "graphpubsub");


subQueue::ThreadSafeQueue(65536);
arpQueue::ThreadSafeQueue(65536);

subTable::GPS_SubscriptionTable(FILENAME $subFile);
arpTable::GPS_ARPTable(FILENAME $arpFile)

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
	-> GPS_TEST_PrintAnno("ArpTable[0].anno")
	-> portSwitch::GPS_PortSwitch;

portSwitch[0]
	-> GPS_TEST_PrintPacket(LABEL "[0]portSwitch.data")
	-> Discard;


portSwitch[1]
	-> GPS_TEST_PrintPacket(LABEL "[1]portSwitch.data")
	-> GPS_TEST_PrintAnno("[1]portSwitch.anno")
	-> GPS_EtherEncap(MY_ETHER $ether)
	-> Print(LABEL "[1]EtherEncap.data", MAXLENGTH 100)
	-> Discard;

portSwitch[2]
	-> GPS_TEST_PrintPacket(LABEL "[2]portSwitch.data")
	-> GPS_TEST_PrintAnno("[2]portSwitch.anno")
	-> GPS_EtherEncap(MY_ETHER $ether)
	-> Print(LABEL "[2]EtherEncap.data", MAXLENGTH 100)
	-> Discard;

portSwitch[3]
	-> GPS_TEST_PrintPacket(LABEL "[3]portSwitch.data")
	-> GPS_TEST_PrintAnno("[3]portSwitch.anno")
	-> GPS_EtherEncap(MY_ETHER $ether)
	-> Print(LABEL "[3]EtherEncap.data", MAXLENGTH 100)
	-> Discard;

portSwitch[4]
	-> GPS_TEST_PrintPacket(LABEL "[4]portSwitch.data")
	-> GPS_TEST_PrintAnno("[4]portSwitch.anno")
	-> GPS_EtherEncap(MY_ETHER $ether)
	-> Print(LABEL "[4]EtherEncap.data", MAXLENGTH 100)
	-> Discard;

portSwitch[5]
	-> GPS_TEST_PrintPacket(LABEL "[5]portSwitch.data")
	-> GPS_TEST_PrintAnno("[5]portSwitch.anno")
	-> GPS_EtherEncap(MY_ETHER $ether)
	-> Print(LABEL "[5]EtherEncap.data", MAXLENGTH 100)
	-> Discard;

portSwitch[6]
	-> GPS_TEST_PrintPacket(LABEL "[6]portSwitch.data")
	-> GPS_TEST_PrintAnno("[6]portSwitch.anno")
	-> GPS_EtherEncap(MY_ETHER $ether)
	-> Print(LABEL "[6]EtherEncap.data", MAXLENGTH 100)
	-> Discard;

portSwitch[7]
	-> GPS_TEST_PrintPacket(LABEL "[7]portSwitch.data")
	-> GPS_TEST_PrintAnno("[7]portSwitch.anno")
	-> GPS_EtherEncap(MY_ETHER $ether)
	-> Print(LABEL "[7]EtherEncap.data", MAXLENGTH 100)
	-> Discard;

portSwitch[8]
	-> GPS_TEST_PrintPacket(LABEL "[8]portSwitch.data")
	-> GPS_TEST_PrintAnno("[8]portSwitch.anno")
	-> GPS_EtherEncap(MY_ETHER $ether)
	-> Print(LABEL "[8]EtherEncap.data", MAXLENGTH 100)
	-> Discard;

portSwitch[9]
	-> GPS_TEST_PrintPacket(LABEL "[9]portSwitch.data")
	-> GPS_TEST_PrintAnno("[9]portSwitch.anno")
	-> GPS_EtherEncap(MY_ETHER $ether)
	-> Print(LABEL "[9]EtherEncap.data", MAXLENGTH 100)
	-> Discard;


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


StaticThreadSched(subUnqueue 1, arpUnqueue 2);
