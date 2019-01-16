// $dev: input device


fd::FromDevice($dev)
    -> cnt1::Counter
    -> q::ThreadSafeQueue(65536)
    -> cnt2::Counter
    -> dis::Discard;

DriverManager(
    wait,
    read cnt1.count,
    read cnt2.count,
    stop
);

StaticThreadSched(fd 0, q 1);

