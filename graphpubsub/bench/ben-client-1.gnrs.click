require(package graphpubsub)

outQueue1::ThreadSafeQueue(65536);



TimedSource(1, "bbbbbbbbbbbbbbbbbbbbbb", LIMIT $lim, HEADROOM 70, STOP true)
	-> GPS_TEST_PubWrapper(DST_GUID $guid)
	-> GPS_TEST_PrintPacket(LABEL "pubWrapper[0].data")
	-> outQueue1;

outQueue1
	-> EtherEncap(ETHERTYPE 0x27c0, SRC $etherSrc, DST $etherDst)
	-> ToDevice($dev);
