// $dev1
// $dev2


outQueue1::ThreadSafeQueue(65536);
outQueue2::ThreadSafeQueue(65536);


FromDevice($dev1, PROMISC false, SNIFFER true)
	-> outQueue2
	-> ToDevice($dev2);

FromDevice($dev2, PROMISC false, SNIFFER true)
	-> outQueue1
	-> ToDevice($dev1);
