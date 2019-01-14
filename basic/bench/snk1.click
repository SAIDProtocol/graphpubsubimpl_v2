// $dev: input device


FromDevice($dev)
    -> cnt::Counter
    -> Discard;

DriverManager(
    wait,
    read cnt.count,
    stop
);
