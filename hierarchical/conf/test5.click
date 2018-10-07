// $arp arpFile
// $sub subscription file
// $gnrs gnrs file
// $fib fib file
// $name named to be added for publication
// $content packet payload
// $na my NA
// $ether my ether

require(package hierarchical)

pubEncap::HRC_PublicationEncapsulator(FILENAME $gnrs);
st::HRC_SubscriptionTable(FILENAME $sub);
arp::HRC_ARPTable(FILENAME $arp);

TimedSource(1, $content, LIMIT 1, STOP true, HEADROOM 100)
    -> HRC_TEST_DataWrapper(PUBLICATION, NAME $name)
    -> HRC_TEST_PrintPacket("**publicationWrapper[0]")
    -> HRC_TEST_PrintAnno("**publicationWrapper[0]")
    -> pubEncap
    -> HRC_TEST_PrintPacket("**pubEncap[0]")
    -> HRC_TEST_PrintAnno("**pubEncap[0]")
// set src na with the following lines
    -> EtherEncap(0x27c1, D5:DC:CA:66:92:D2, $ether)
    -> HRC_EtherAnnotator(6)
    -> HRC_TEST_PrintAnno("**etherAnnotator[0]")
    -> Strip(14)
    -> [1]arp[1]
    -> HRC_TEST_PrintPacket("**arp[1]")
    -> HRC_TEST_PrintAnno("**arp[1]")
// finished setting src na
    -> HRC_RP
    -> HRC_TEST_PrintPacket("**rp[0]")
    -> HRC_TEST_PrintAnno("**rp[0]")
    -> st
    -> HRC_TEST_PrintPacket("**st[0]")
    -> HRC_TEST_PrintAnno("**st[0]")
    -> [0]arp[0]
    -> HRC_TEST_PrintPacket("**arp[0]")
    -> HRC_TEST_PrintAnno("**arp[0]")
    -> Discard;

pubEncap[1]
    -> HRC_TEST_PrintPacket("**pubEncap[1]")
    -> HRC_TEST_PrintAnno("**pubEncap[1]")
    -> Discard;

arp[2]
    -> HRC_TEST_PrintPacket("**arp[2]")
    -> HRC_TEST_PrintAnno("**arp[2]")
    -> Discard;

st[1]
    -> HRC_TEST_PrintPacket("**st[1]")
    -> HRC_TEST_PrintAnno("**st[1]")
    -> Discard;

st[2]
    -> HRC_TEST_PrintPacket("**st[2]")
    -> HRC_TEST_PrintAnno("**st[2]")
    -> Discard;
