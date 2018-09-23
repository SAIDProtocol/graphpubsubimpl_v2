require(package graphpubsub)

outQueue1::ThreadSafeQueue(65536);


TimedSource(1, "", LIMIT 1, HEADROOM 70, STOP true)
	-> GPS_TEST_GNRSPacketCreator(TYPE 0xc2, GUID $guid, DST_NA $naDst, REQUESTED_GUID_NA $naReq, VERSION $ver)
	-> GPS_TEST_PrintPacket(LABEL "pubWrapper[0].data")
	-> outQueue1;

outQueue1
	-> EtherEncap(ETHERTYPE 0x27c0, SRC $etherSrc, DST $etherDst)
	-> ToDevice($dev);
