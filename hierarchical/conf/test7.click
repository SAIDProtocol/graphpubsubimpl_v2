// $type
// $file
// $size
// $duration
// $rate
require(package hierarchical)

HRC_TEST_DataGenerator(TYPE $type, NAME_FILE $file, SIZE $size, DURATION $duration)
    -> BandwidthRatedUnqueue(RATE $rate)
    -> HRC_TEST_PrintPacket("Unqueue[0]")
    -> Discard;