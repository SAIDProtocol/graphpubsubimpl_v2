// arpFile
// rtgFile
// subFile
// rpFile
// na, gnrsNa
// dev1, ether1
// dev2, ether2
// dev3, ether3

require(package graphpubsub)

arpTable::GPS_ARPTable(FILENAME $arpFile);
rtgTable::GPS_RoutingTable(FILENAME $rtgFile);
subTable::GPS_SubscriptionTable(FILENAME $subFile);
rp::GPS_RP(FILENAME $rpFile, MY_NA $na)
gnrsCache::GPS_GNRSCache(GNRS_NA $gnrsNa, NA $na);
portSwitch::GPS_PortSwitch;
outEncap1::GPS_EtherEncap(MY_ETHER $ether1);
outEncap2::GPS_EtherEncap(MY_ETHER $ether2);
outEncap3::GPS_EtherEncap(MY_ETHER $ether3);

inQueue::ThreadSafeQueue(65536);
gnrsDataQueue::ThreadSafeQueue(65536);
arpQueue::ThreadSafeQueue(65536);
subQueue::ThreadSafeQueue(65536);
outQueue1::ThreadSafeQueue(65536);
outQueue2::ThreadSafeQueue(65536);
outQueue3::ThreadSafeQueue(65536);


FromDevice($dev1, PROMISC false, SNIFFER true)
	-> classifier1::Classifier(12/27c0, -)
	-> GPS_EtherAnnotator(PORT 1)
	-> Strip(14)
//	-> GPS_TEST_PrintPacket(LABEL "dev1.data")
//	-> GPS_TEST_PrintAnno(LABEL "dev1.anno")
	-> inQueue;

FromDevice($dev2, PROMISC false, SNIFFER true)
	-> classifier2::Classifier(12/27c0, -)
	-> GPS_EtherAnnotator(PORT 2)
	-> Strip(14)
//	-> GPS_TEST_PrintPacket(LABEL "dev2.data")
//	-> GPS_TEST_PrintAnno(LABEL "dev2.anno")
	-> inQueue;

FromDevice($dev3, PROMISC false, SNIFFER true)
	-> classifier3::Classifier(12/27c0, -)
	-> GPS_EtherAnnotator(PORT 3)
	-> Strip(14)
//	-> GPS_TEST_PrintPacket(LABEL "dev3.data")
//	-> GPS_TEST_PrintAnno(LABEL "dev3.anno")
	-> inQueue;


inQueue
	-> inUnqueue::Unqueue
	-> GPS_PrioAnnotator
//	-> GPS_TEST_PrintAnno(LABEL "PrioAnnotator[0]")
	-> inClassifier::Classifier(00/01, 00/81, 00/82, 00/c1, 00/c2, 00/c3, -);

inClassifier[0]	// lsa
//	-> GPS_TEST_PrintPacket(LABEL "LSA")
//	-> GPS_TEST_PrintAnno(LABEL "LSA")
	-> [1]arpTable;

inClassifier[1] // publication
	-> GPS_TEST_PrintPacket(LABEL "PUBLICATION")
	-> GPS_TEST_PrintAnno(LABEL "PUBLICATION")
	-> pubClassifier::Classifier(44/00000000, -);

inClassifier[2]
//	-> GPS_TEST_PrintPacket(LABEL "SUBSCRIPTION")
//	-> GPS_TEST_PrintAnno(LABEL "SUBSCRIPTION")
	-> [1]subTable;

inClassifier[3]
//	-> GPS_TEST_PrintPacket(LABEL "GNRS REQ")
//	-> GPS_TEST_PrintAnno(LABEL "GNRS REQ")
	-> [1]gnrsCache;

inClassifier[4]
//	-> GPS_TEST_PrintPacket(LABEL "GNRS_RESP")
//	-> GPS_TEST_PrintAnno(LABEL "GNRS_RESP")
	-> [2]gnrsCache;

inClassifier[5]
//	-> GPS_TEST_PrintPacket(LABEL "GNRS_ASSO")
//	-> GPS_TEST_PrintAnno(LABEL "GNRS_ASSO")
	-> Discard;  // TODO

pubClassifier[0] // publication to RP
	-> gnrsDataQueue;

pubClassifier[1] // publication to subs
//	-> GPS_TEST_PrintPacket(LABEL "To SubTable")
	-> subQueue;

gnrsDataQueue
	-> gnrsUnqueue::Unqueue
	-> [0]gnrsCache
	-> GPS_TEST_PrintPacket(LABEL "gnrsCache[0]")
	-> GPS_TEST_PrintAnno(LABEL "gnrsCache[0]")
	-> [0]rtgTable
//	-> GPS_TEST_PrintPacket(LABEL "rtgTable[0]")
//	-> GPS_TEST_PrintAnno(LABEL "rtgTable[0]")
	-> arpQueue;

gnrsCache[1]	// to local, do RP expansion
	-> GPS_TEST_PrintPacket(LABEL "gnrsCache[1]")
	-> GPS_TEST_PrintAnno(LABEL "gnrsCache[1]")
	-> rp
	-> GPS_TEST_PrintPacket(LABEL "rp[0]")
	-> GPS_TEST_PrintAnno(LABEL "rp[0]")
	-> subQueue;

rp[1]
	-> GPS_TEST_PrintPacket(LABEL "rp[1]")
	-> GPS_TEST_PrintAnno(LABEL "rp[1]")
	-> gnrsDataQueue;


subQueue
	-> subUnqueue::Unqueue
	-> GPS_TEST_PrintPacket(LABEL "[0]subTable")
	-> GPS_TEST_PrintAnno(LABEL "[0]subTable")
	-> subTable
	-> GPS_TEST_PrintPacket(LABEL "subTable[0]")
	-> GPS_TEST_PrintAnno(LABEL "subTable[0]")
	-> arpQueue;

subTable[1] // subscription
	-> GPS_TEST_PrintPacket(LABEL "subTable[1]")
	-> GPS_TEST_PrintAnno(LABEL "subTable[1]")
	-> Discard; // TODO

arpQueue
	-> arpUnqueue::Unqueue
	-> GPS_TEST_PrintPacket(LABEL "[0]arpTable")
	-> GPS_TEST_PrintAnno(LABEL "[0]arpTable")
	-> arpTable
	-> GPS_TEST_PrintPacket(LABEL "arpTable[0]")
	-> GPS_TEST_PrintAnno(LABEL "arpTable[0]")
	-> portSwitch;

portSwitch[1] -> outQueue1;
portSwitch[2] -> outQueue2;
portSwitch[3] -> outQueue3;

outQueue1
	-> outEncap1
	-> Print("outEncap1", 100)
	-> ToDevice($dev1)

outQueue2
	-> outEncap2
	-> Print("outEncap2", 100)
	-> ToDevice($dev2)

outQueue3
	-> outEncap3
	-> Print("outEncap3", 100)
	-> ToDevice($dev3)


classifier1[1]		// Cannot understand ether type from dev 1
	-> Print("classifier1[1]", 100)
	-> Discard;

classifier2[1]		// Cannot understand ether type from dev 2
	-> Print("classifier2[1]", 100)
	-> Discard;

classifier3[1]		// Cannot understand ether type from dev 3
	-> Print("classifier3[1]", 100)
	-> Discard;

inClassifier[6]		// Cannot understand GPS packet type
	-> Print("inClassifier[6]", 100)
	-> Discard;

rtgTable[1]		// Fail in routing, cannot find next hop
	-> Print("rtgTable[1]", 100)
	-> Discard;

arpTable[1]		// Cannot find neighbor
	-> Print("arpTable[1]", 100)
	-> Discard;

portSwitch[0]		// Cannot find out port (0)
	-> Print("portSwitch[0]", 100)
	-> Discard;

subTable[2]		// Has no subscribers, discard
	-> Print("subTable[2]", 100)
	-> Discard;

