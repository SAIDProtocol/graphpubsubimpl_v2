require(package graphpubsub)

FromDevice($dev1)
	-> cnt1::Counter
	-> Discard;

FromDevice($dev2)
	-> cnt2::Counter
	-> Discard;


GPS_TEST_PrintCounter(ELEM cnt1, ELEM cnt2);
