// arp: arpFile
// fib: fibFile
// ether: my ether address
// name: name to put in the interest
// content: payload of the interest packet

require(package hierarchical)

fib::HRC_FIB(FILENAME $fib);
arp::HRC_ARPTable(FILENAME $arp);

TimedSource(2, $content, LIMIT 1)
    -> Unstrip(1)
    -> Print("Before Wrapper", 20, HEADROOM true)
    -> HRC_TEST_InterestWrapper(NAME $name)
    -> HRC_PrioAnnotator
    -> HRC_TEST_PrintPacket("[0]fib")
    -> HRC_TEST_PrintAnno("[0]fib")
    -> fib;

fib[0]
    -> HRC_TEST_PrintPacket("fib[0]")
    -> HRC_TEST_PrintAnno("fib[0]")
    -> arp
    -> HRC_TEST_PrintPacket("arp[0]")
    -> HRC_TEST_PrintAnno("arp[0]")
    -> Print("arp[0]", MAXLENGTH 100, HEADROOM true)
    -> HRC_EtherEncap(MY_ETHER $ether)
    -> Print("etherEncap[0]", MAXLENGTH 100, HEADROOM true)
    -> HRC_TEST_PrintAnno("etherEncap[0]")
    -> Strip(14)
    -> HRC_TEST_PrintPacket("Strip(14)[0]")
    -> HRC_TEST_PrintAnno("Strip(14)[0]")
    -> Discard;

fib[1]
    -> HRC_TEST_PrintPacket("fib[1]")
    -> HRC_TEST_PrintAnno("fib[1]")
    -> Discard;

fib[2]
    -> HRC_TEST_PrintPacket("fib[2]")
    -> HRC_TEST_PrintAnno("fib[2]")
    -> Discard;

fib[3]
    -> HRC_TEST_PrintPacket("fib[3]")
    -> HRC_TEST_PrintAnno("fib[3]")
    -> Discard;
