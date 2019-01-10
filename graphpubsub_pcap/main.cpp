//#include <iostream>
//#include <pcap/pcap.h>
//#include <cerrno>
//#include <cstdlib>
//#include <netinet/in.h>
//#include <arpa/inet.h>

#include "pcap_help.hpp"
#include <locale.h>

int main() {
    setlocale(LC_NUMERIC, "");
//    print_all_devices();
//    print_dev_net("eno1");
//    capture_one_packet("vlan259");
    capture_dev("vlan259", -1, "");
//    relay_dev("eno2", -1, "", "eno1");
    return 0;

}

