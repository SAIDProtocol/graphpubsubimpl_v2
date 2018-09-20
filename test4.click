require(package "graphpubsub");

rp::GPS_RP(FILENAME test.rp, MY_NA 0x123);

print::GPS_TEST_PrintPacket(LABEL "[0]rp.data")
	-> GPS_TEST_PrintAnno("[0]rp.anno")
	-> rp;

TimedSource(1, "aaaaaa", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x10006)
	-> print;

TimedSource(2, "bbbbb", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x10008)
	-> print;

TimedSource(3, "bbbbb", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x10005)
	-> print;

TimedSource(4, "bbbbb", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x10004)
	-> print;

rp[0]
	-> GPS_TEST_PrintPacket(LABEL "rp[0].data")
	-> GPS_TEST_PrintAnno("rp[0].anno")
	-> Discard;


rp[1]
	-> GPS_TEST_PrintPacket(LABEL "rp[1].data")
	-> GPS_TEST_PrintAnno("rp[1].anno")
	-> Discard;


rp[2]
	-> GPS_TEST_PrintPacket(LABEL "rp[2].data")
	-> GPS_TEST_PrintAnno("rp[2].anno")
	-> Discard;




