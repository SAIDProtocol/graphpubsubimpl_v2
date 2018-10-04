require(package hierarchical)

TimedSource(1, "AAAAAA", LIMIT 1, STOP true)
    -> EtherEncap(0x27c0, 1:2:3:4:5:6, 7:8:9:a:b:c)
    -> Print
    -> HRC_TEST_PrintAnno("Before EtherAnno")
    -> HRC_EtherAnnotator(PORT 4)
    -> HRC_TEST_PrintAnno("After EtherAnno")
    -> Discard;