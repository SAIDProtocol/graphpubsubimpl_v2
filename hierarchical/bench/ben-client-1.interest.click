// $fibFile
// $arpFile
// $gnrsFile
// $ether $dev
// $lim
// $name
// $na
require(package hierarchical)

fib::HRC_FIB(FILENAME $fibFile, NA $na);
arpTable::HRC_ARPTable(FILENAME $arpFile);
outSwitch::HRC_PortSwitch;
outEncap1::HRC_EtherEncap(MY_ETHER $ether);
pubEncap::HRC_PublicationEncapsulator(FILENAME $gnrsFile);

outQueue1::ThreadSafeQueue(65536);

TimedSource($gap, "bbbbbbbbbbbbbbbbbbbbbb", LIMIT $lim, HEADROOM 200, STOP true)
	-> HRC_TEST_DataWrapper(PUBLICATION, $name, $isFile)
	-> HRC_TEST_PrintPacket("pubWrapper[0].data")
	-> pubEncap
	-> HRC_TEST_PrintPacket("pubEncap[0].data")
	-> HRC_PrioAnnotator
	-> HRC_TEST_PrintAnno("prioAnno[0].anno")
	-> fib
	-> HRC_TEST_PrintPacket("fib[0].data")
	-> HRC_TEST_PrintAnno("fib[0].anno")
	-> arpTable
	-> HRC_TEST_PrintPacket("arp[0].data")
	-> HRC_TEST_PrintAnno("arp[0].anno")
	-> outSwitch[1]
	-> outQueue1
	-> outEncap1
	-> Print(outEncap1[0], 100)
	-> ToDevice($dev);

pubEncap[1]		// do not know which name to encap
	-> Print(pubEncap[1], 100)
	-> Discard;

arpTable[1]		// should not reach
	-> Print(arpTable[1], 100)
	-> Discard;

arpTable[2]		// cannot find neighbor
	-> Print(arpTable[2], 100)
	-> Discard;

fib[1]			// to local, discard
	-> Print(fib[1], 100)
	-> Discard;

fib[2]			// cannot find destination
	-> Print(fib[2], 100)
	-> Discard;

outSwitch[0]		// cannot find out port
	-> Print(outSwitch[0], 100)
	-> Discard;
