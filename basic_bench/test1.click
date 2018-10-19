// $dev1 $dev1Mac $dev1DstMac
// $dev2 $dev2Mac $dev2DstMac
inQueue::ThreadSafeQueue(65536);
outQueue1::ThreadSafeQueue(65536);
outQueue2::ThreadSafeQueue(65536);


FromDevice($dev1, PROMISC false, SNIFFER true)
	-> classifier1::Classifier(12/27c0, -)
	-> Paint(2)
	-> Strip(14)
	-> inQueue;

FromDevice($dev2, PROMISC false, SNIFFER true)
	-> classifier2::Classifier(12/27c0, -)
	-> Paint(1)
	-> Strip(14)
	-> inQueue;

inQueue
	-> Unqueue
	-> ps::PaintSwitch;

ps[1]
	-> outQueue1
	-> EtherEncap(0x27c0, $dev1Mac, $dev1DstMac)
	-> ToDevice($dev1);

ps[2]
	-> outQueue2
	-> EtherEncap(0x27c0, $dev2Mac, $dev2DstMac)
	-> ToDevice($dev2);


classifier1[1] -> Discard;
classifier2[1] -> Discard;
ps[0] -> Discard;
