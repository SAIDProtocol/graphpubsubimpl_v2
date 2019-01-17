// $dev: input device


require(package "basictest")

fd::FromDevice($dev)
    -> BASIC_CountedSink;