require(package hierarchical)

fib::HRC_FIB(FILENAME $fib);
arp::HRC_ARPTable(FILENAME $arp);

TimedSource(2, "aaaa", LIMIT 1)
    -> Unstrip(1)
    -> Print("Before Wrapper", 20, HEADROOM true)
    -> HRC_TEST_InterestWrapper(NAME "/a/bb/ccc/dddd")
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
