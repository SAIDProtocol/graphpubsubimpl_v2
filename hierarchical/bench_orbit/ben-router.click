// $arpFile
// $subFile
// $gnrsFile
// $fibFile
// $dev1 $ether1
// $dev2 $ether2
// $dev3 $ether3
// $na my NA
require(package hierarchical)

arpTable::HRC_ARPTable(FILENAME $arpFile);
subTable::HRC_SubscriptionTable(FILENAME $subFile);
gnrsCache::HRC_GNRSCache(FILENAME $gnrsFile);
fib::HRC_FIB(FILENAME $fibFile, NA $na);
portSwitch::HRC_PortSwitch;
outEncap1::HRC_EtherEncap(MY_ETHER $ether1);
outEncap2::HRC_EtherEncap(MY_ETHER $ether2);

inQueue::ThreadSafeQueue(65536);
arpQueue::ThreadSafeQueue(65536);
outQueue1::ThreadSafeQueue(65536);
outQueue2::ThreadSafeQueue(65536);


FromDevice($dev1, PROMISC false, SNIFFER true)
    -> cntIn1::Counter
    -> classifier1::Classifier(12/27c1, -)
    -> HRC_EtherAnnotator(PORT 1)
    -> Strip(14)
//    -> HRC_TEST_PrintPacket(LABEL "dev1.data")
//    -> HRC_TEST_PrintAnno(LABEL "dev1.anno")
    -> inQueue;

FromDevice($dev2, PROMISC false, SNIFFER true)
    -> cntIn2::Counter
    -> classifier2::Classifier(12/27c1, -)
    -> HRC_EtherAnnotator(PORT 2)
    -> Strip(14)
//    -> HRC_TEST_PrintPacket(LABEL "dev2.data")
//    -> HRC_TEST_PrintAnno(LABEL "dev2.anno")
    -> inQueue;

inQueue
    -> inUnqueue::Unqueue
    -> HRC_PrioAnnotator
//    -> HRC_TEST_PrintAnno(LABEL "PrioAnnotator[0]")
    -> [1]arpTable[1]
//    -> HRC_TEST_PrintAnno(LABEL "arpTable[1]")
    -> inClassifier::Classifier(00/81, 00/82, 00/83, 00/84, -);

inClassifier[0] // Interest
//    -> HRC_TEST_PrintPacket(LABEL "INTEREST")
//    -> HRC_TEST_PrintAnno(LABEL "INTEREST")
    -> [0]fib[0]
//    -> HRC_TEST_PrintPacket(LABEL "fib[0]")
//    -> HRC_TEST_PrintAnno(LABEL "fib[0]")
    -> arpQueue;

inClassifier[1] // Data
    -> HRC_TEST_PrintPacket(LABEL "DATA")
    -> HRC_TEST_PrintAnno(LABEL "DATA")
    -> Discard;

inClassifier[2] // Publication
//    -> HRC_TEST_PrintPacket(LABEL "PUBLICATION")
//    -> HRC_TEST_PrintAnno(LABEL "PUBLICATION")
    -> [0]subTable[0]
//    -> HRC_TEST_PrintPacket(LABEL "subTable[0]")
//    -> HRC_TEST_PrintAnno(LABEL "subTable[0]")
    -> arpQueue;

inClassifier[3] // Subscription
//    -> HRC_TEST_PrintPacket(LABEL "SUBSCRIPTION")
//    -> HRC_TEST_PrintAnno(LABEL "SUBSCRIPTION")
    -> [1]subTable[1]
//    -> HRC_TEST_PrintPacket(LABEL "subTable[1]")
//    -> HRC_TEST_PrintAnno(LABEL "subTable[1]")
    -> gnrsCache
//    -> HRC_TEST_PrintPacket(LABEL "gnrsCache[0]")
//    -> HRC_TEST_PrintAnno(LABEL "gnrsCache[0]")
    -> [1]fib;


arpQueue
    -> arpUnqueue::Unqueue
    -> arpTable
//    -> HRC_TEST_PrintPacket(LABEL "arpTable[0]")
//    -> HRC_TEST_PrintAnno(LABEL "arpTable[0]")
    -> portSwitch;

portSwitch[1] -> outQueue1;
portSwitch[2] -> outQueue2;

outQueue1
    -> outEncap1
//    -> Print("outEncap1", 100)
    -> cntOut1::Counter
    -> toDev1::ToDevice($dev1);

outQueue2
    -> outEncap2
//    -> Print("outEncap2", 100)
    -> cntOut2::Counter
    -> toDev2::ToDevice($dev2);

GPS_TEST_PrintCounter(ELEM cntIn1, ELEM cntIn2, ELEM cntOut1, ELEM cntOut2);

fib[1]
    -> HRC_TEST_PrintPacket(LABEL "fib[1]")
    -> HRC_TEST_PrintAnno(LABEL "fib[1]")
    -> Discard;

classifier1[1]          // Cannot understand ether type from dev 1
//    -> Print("classifier1[1]", 100)
    -> Discard;

classifier2[1]          // Cannot understand ether type from dev 2
//    -> Print("classifier2[1]", 100)
    -> Discard;

inClassifier[4]         // Cannot understand HRC packet type
    -> Print("inClassifier[4]", 100)
    -> Discard;


portSwitch[0]           // Cannot find out port (0)
    -> Print("portSwitch[0]", 100)
    -> Discard;

arpTable[2]             // Cannot find neighbor
    -> Print("arpTable[2]", 100)
    -> Discard;

gnrsCache[1]            // Cannot find gnrs entry
    -> Print("gnrsCache[1]", 100)
    -> Discard;

subTable[2]             // Cannot find subscribers
    -> HRC_TEST_PrintPacket(LABEL "subTable[2]")
    -> HRC_TEST_PrintAnno(LABEL "subTable[2]")
    -> Discard;

fib[2]                  // Cannot find next hop
    -> Print("fib[2]", 100)
    -> Discard;
