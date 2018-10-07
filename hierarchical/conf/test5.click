// $sub subscription file
// $gnrs gnrs file
// $name named to be added for publication
// $content packet payload

require(package hierarchical)


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
