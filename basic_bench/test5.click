require(package graphpubsub)

FromDevice($dev1)
	-> Classifier(12/27c0)
	-> cntIn1::Counter
	-> outQueue2::Queue
	-> Strip(14)
	-> EtherEncap(0x27c0, $dev2Mac, $dev2DstMac)
	-> cntOut2::Counter
	-> ToDevice($dev2);

GPS_TEST_PrintCounter(ELEM cntIn1, ELEM cntOut2);
