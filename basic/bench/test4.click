// $dev1
// $dev2

inQueue1::ThreadSafeQueue(65536);
inQueue2::ThreadSafeQueue(65536);
outQueue1::ThreadSafeQueue(65536);
outQueue2::ThreadSafeQueue(65536);


FromDevice($dev1, PROMISC false, SNIFFER true)
	-> inQueue1
	-> Unqueue
	-> outQueue2
	-> ToDevice($dev2);

FromDevice($dev2, PROMISC false, SNIFFER true)
	-> inQueue2
	-> Unqueue
	-> outQueue1
	-> ToDevice($dev1);
