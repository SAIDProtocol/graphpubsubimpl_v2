require(package graphpubsub)

FromDevice($dev)
	-> Classifier(12/27c0)
	-> cnt::Counter
	-> Discard;

GPS_TEST_PrintCounter(ELEM cnt);
