require(package "graphpubsub");

subTable::GPS_SubscriptionTable(FILENAME $subFile);
arpTable::GPS_ARPTable(FILENAME $arpFile)


next::Null
	-> GPS_TEST_PrintPacket(LABEL "GPS_EtherAnnotator[0].data")
	-> GPS_TEST_PrintAnno("GPS_EtherAnnotator[0].anno")
	-> subTable
	-> GPS_TEST_PrintPacket(LABEL "subTable[0].data")
	-> GPS_TEST_PrintAnno("subTable[0].anno")
	-> arpTable
	-> GPS_TEST_PrintPacket(LABEL "arpTable[0].data")
	-> GPS_TEST_PrintAnno("arpTable[0].anno")
	-> Discard;

TimedSource(1, "bbbbb", LIMIT 10, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x223, DST_NA_FILE $rtgFile)
	-> EtherEncap(0x0800, 70:8b:cd:bc:84:47, 70:8b:cd:bc:84:46)
	-> GPS_EtherAnnotator(PORT 1)
	-> Strip(14)
	-> next;


TimedSource(2, "bbbbb", LIMIT 1, HEADROOM 96)
	-> GPS_TEST_PubWrapper(DST_GUID 0x223)
	-> Print("Before IP")
	-> IPEncap(4, 192.168.1.1, 192.168.1.2)
	-> IPPrint("After IP")
	-> EtherEncap(0x0800, 70:8b:cd:bc:84:46, 70:8b:cd:bc:84:47)
	-> Print("Before Ether")
	-> GPS_EtherAnnotator(PORT 2)
	-> Print
	-> StripIPHeader
	-> next;

