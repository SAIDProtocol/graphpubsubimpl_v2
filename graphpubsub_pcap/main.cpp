#include <iostream>
#include <pcap/pcap.h>
#include <cerrno>
#include <cstdlib>

using namespace std;

int main() {

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *alldevsp;

    int ret = pcap_findalldevs(&alldevsp, errbuf);
    cout << "ret=" << ret << endl;

    for (auto dev = alldevsp; dev != nullptr; dev = dev->next) {
        cout << "name = " << dev->name << endl;
        if (dev->description) cout << "  description = " << dev->description << endl;
        cout << "  flags =";
        if (dev->flags & PCAP_IF_LOOPBACK) cout << " PCAP_IF_LOOPBACK";
        if (dev->flags & PCAP_IF_UP) cout << " PCAP_IF_UP";
        if (dev->flags & PCAP_IF_RUNNING) cout << " PCAP_IF_RUNNING";
        cout << endl;
        cout << endl;
    }

    pcap_freealldevs(alldevsp);


    return 0;

}