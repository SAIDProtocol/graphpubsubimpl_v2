require(package hierarchical)

TimedSource(2, "aaa", HEADROOM 100)
    -> Print("Before Wrapper")
    -> HRC_TEST_InterestWrapper(NAME "/a/bb/ccc/dddd")
    -> HRC_TEST_PrintPacket("After Wrapper")
    -> Discard;
