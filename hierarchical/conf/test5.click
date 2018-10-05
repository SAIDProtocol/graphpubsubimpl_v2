// $sub subscription file
// $name named to be added for publication
// $sub packet payload

require(package hierarchical)

st::HRC_SubscriptionTable(FILENAME $sub);

TimedSource(1, $content, LIMIT 1, STOP true, HEADROOM 100)
    -> HRC_TEST_DataWrapper(PUBLICATION, NAME $name)
    -> HRC_TEST_PrintPacket("**publicationWrapper[0]")
    -> HRC_TEST_PrintAnno("**publicationWrapper[0]")
    -> Discard;
