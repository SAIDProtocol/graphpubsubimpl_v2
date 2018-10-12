// $subFile
// $arpFile
// $gap
// $lim
// $name $isFile
// $ether
// $dev
require(package hierarchical)

subTable::HRC_SubscriptionTable(FILENAME $subFile);
arpTable::HRC_ARPTable(FILENAME $arpFile);
outSwitch::HRC_PortSwitch;
outEncap1::HRC_EtherEncap(MY_ETHER $ether);

outQueue1::ThreadSafeQueue(65536);

TimedSource($gap, "bbbbbbbbbbbbbbbbbbbcbb", LIMIT $lim, HEADROOM 200, STOP true)
   	-> HRC_TEST_DataWrapper(PUBLICATION, $name, $isFile)
	-> HRC_TEST_PrintPacket("pubWrapper[0].data")
	-> HRC_PrioAnnotator
	-> HRC_TEST_PrintAnno("prioAnno[0].anno")
	-> HRC_TEST_PrintAnno("prioAnno[0].anno")
	-> subTable
	-> HRC_TEST_PrintAnno("subTable[0].anno")
	-> HRC_TEST_PrintAnno("subTable[0].anno")
	-> arpTable
	-> HRC_TEST_PrintAnno("arpTable[0].anno")
	-> HRC_TEST_PrintAnno("arpTable[0].anno")
	-> outSwitch[1]
	-> outQueue1
	-> outEncap1
	-> Print(outEncap1[0], 100)
    -> ToDevice($dev);

arpTable[1]		// should not reach
	-> Print(arpTable[1], 100)
	-> Discard;

arpTable[2]		// cannot find neighbor
	-> Print(arpTable[2], 100)
	-> Discard;

outSwitch[0]		// cannot find out port
	-> Print(outSwitch[0], 100)
	-> Discard;
