// $gnrs gnrs file
// $fib fib file
// $name named to be added for publication
// $content packet payload
// $na my NA

require(package hierarchical)

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