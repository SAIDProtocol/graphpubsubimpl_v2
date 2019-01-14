#include <assert.h>
#include <pcap/pcap.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define MIN_ETH_SIZE 14
#define BUF_SIZE 125
#define COUNT 32000000
#define DEV "eth0"


int main() {
    const char *dev = DEV;
    int i;

    assert(BUF_SIZE >= MIN_ETH_SIZE);

    char buf[BUF_SIZE] = {0xec, 0x0d, 0x9a, 0x7e, 0x91, 0x96, 0xec, 0x0d, 0x9a, 0x7e, 0x90, 0xc6, 0x27, 0xc1};

    for (i = 0; i < BUF_SIZE; i++) {
        printf("%02x ", buf[i] & 0xFF);
    }
    printf("\n");


    char pcap_errbuf[PCAP_ERRBUF_SIZE];
    memset(pcap_errbuf, 0, PCAP_ERRBUF_SIZE);
    pcap_t *pcap = pcap_open_live(dev, 96, 0, 0, pcap_errbuf);
    if (pcap_errbuf[0] != '\0') fprintf(stderr, "%s", pcap_errbuf);
    if (!pcap) return 1;

    struct timespec start, end;

    clock_gettime(CLOCK_MONOTONIC_RAW, &start);
    for (i = 0; i < COUNT; i++) {
        pcap_inject(pcap, buf, BUF_SIZE);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    uint64_t delta_ns = (end.tv_sec - start.tv_sec) * 1000000000 + (end.tv_nsec - start.tv_nsec);
    printf("took: %luns\n", delta_ns);



    pcap_close(pcap);
    return 0;
}

