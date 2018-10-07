// $sub subscription file
// $gnrs gnrs file
// $fib fib file
// $name named to be added for publication
// $content packet payload

require(package hierarchical)


/*
pubEncap::HRC_PublicationEncapsulator(FILENAME $gnrs);

st::HRC_SubscriptionTable(FILENAME $sub);

TimedSource(1, $content, LIMIT 1, STOP true, HEADROOM 100)
    -> HRC_TEST_DataWrapper(PUBLICATION, NAME $name)
    -> HRC_TEST_PrintPacket("**publicationWrapper[0]")
    -> HRC_TEST_PrintAnno("**publicationWrapper[0]")
    -> pubEncap
    -> HRC_TEST_PrintPacket("**pubEncap[0]")
    -> HRC_TEST_PrintAnno("**pubEncap[0]")
    -> HRC_RP
    -> HRC_TEST_PrintPacket("**rp[0]")
    -> HRC_TEST_PrintAnno("**rp[0]")
    -> Discard;

pubEncap[1]
    -> HRC_TEST_PrintPacket("**pubEncap[1]")
    -> HRC_TEST_PrintAnno("**pubEncap[1]")
    -> Discard;
*/

gnrs::HRC_GNRSCache(FILENAME $gnrs);
fib::HRC_FIB(FILENAME $fib, NA $na);

TimedSource(1, $content, LIMIT 1, STOP true, HEADROOM 100)
    -> HRC_TEST_DataWrapper(SUBSCRIPTION, NAME $name)
    -> HRC_TEST_PrintPacket("**subscriptionWrapper[0]")
    -> HRC_TEST_PrintAnno("**subscriptionWrapper[0]")
    -> gnrs
    -> HRC_TEST_PrintPacket("**gnrs[0]")
    -> HRC_TEST_PrintAnno("**gnrs[0]")
    -> [1]fib[0]
    -> HRC_TEST_PrintPacket("**fib[0]")
    -> HRC_TEST_PrintAnno("**fib[0]")
    -> Discard;

gnrs[1]
    -> HRC_TEST_PrintPacket("**gnrs[1]")
    -> HRC_TEST_PrintAnno("**gnrs[1]")
    -> Discard;


TimedSource(1, $content, LIMIT 0, STOP true, HEADROOM 100)
    -> [0]fib;

fib[1]
    -> HRC_TEST_PrintPacket("**fib[1]")
    -> HRC_TEST_PrintAnno("**fib[1]")
    -> HRC_RP
    -> HRC_TEST_PrintPacket("**rp[0]")
    -> HRC_TEST_PrintAnno("**rp[0]")
    -> Discard;

fib[2]
    -> HRC_TEST_PrintPacket("**fib[2]")
    -> HRC_TEST_PrintAnno("**fib[2]")
    -> Discard;
