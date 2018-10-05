// $arp arpFile
// $fib fibFile
// $content content of payload
// $name name added to interest
// $srcEther ether address of the previous hop
// $port port from the previous hop
// $myNa my NA
// $myEther my Ether

require(package hierarchical)

arp::HRC_ARPTable(FILENAME $arp);
fib::HRC_FIB(FILENAME $fib, NA $myNa);

TimedSource(1, $content, LIMIT 1, STOP true)
    -> HRC_TEST_InterestWrapper(NAME $name)
    -> HRC_TEST_PrintPacket("**interestWrapper[0]")
    -> HRC_TEST_PrintAnno("**interestWrapper[0]")
    -> EtherEncap(0x27c0, $srcEther, 7:8:9:a:b:c)
    -> Print("**etherEncap[0]")
    -> HRC_TEST_PrintAnno("**etherEncap[0]")
    -> HRC_EtherAnnotator(PORT $port)
    -> HRC_TEST_PrintAnno("**etherAnnotator[0]")
    -> Strip(14)
    -> HRC_TEST_PrintPacket("**strip[0]")
    -> HRC_TEST_PrintAnno("**strip[0]")
    -> HRC_PrioAnnotator
    -> HRC_TEST_PrintPacket("**prioAnnotator[0]")
    -> HRC_TEST_PrintAnno("**prioAnnotator[0]")
    -> [1]arp;

arp[1]
    -> HRC_TEST_PrintPacket("**arp[1]")
    -> HRC_TEST_PrintAnno("**arp[1]")
    -> [0]fib;

// to another router
fib[0]
    -> HRC_TEST_PrintPacket("**fib[0]")
    -> HRC_TEST_PrintAnno("**fib[0]")
    -> [0]arp;

// to local
fib[1]
    -> HRC_TEST_PrintPacket("**fib[1]")
    -> HRC_TEST_PrintAnno("**fib[1]")
    -> Discard;

// cannot find next hop for the name, discard
fib[2]
    -> HRC_TEST_PrintPacket("**fib[2]")
    -> HRC_TEST_PrintAnno("**fib[2]")
    -> Discard;

// data to another node
arp[0]
    -> HRC_TEST_PrintPacket("**arp[0]")
    -> HRC_TEST_PrintAnno("**arp[0]")
    -> HRC_EtherEncap(MY_ETHER $myEther)
    -> Print("**etherEncap[0]")
    -> Discard;

// from unknown prev hop, or to known next hop, discard
arp[2]
    -> HRC_TEST_PrintPacket("**arp[2]")
    -> HRC_TEST_PrintAnno("**arp[2]")
    -> Discard;
