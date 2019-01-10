// $dev1
// $dev2


outQueue1::Queue(10);
outQueue2::Queue(10);


FromDevice($dev1, PROMISC false, SNIFFER true)
	-> outQueue2
	-> ToDevice($dev2);

FromDevice($dev2, PROMISC false, SNIFFER true)
	-> outQueue1
	-> ToDevice($dev1);
