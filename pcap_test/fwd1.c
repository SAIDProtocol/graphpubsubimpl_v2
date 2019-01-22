#include <assert.h>
#include <inttypes.h>
#include <pcap/pcap.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>

#define DURATION 5000
#define DEV "eth1"
#define ETH_HEADER_SIZE (14)

static void (*got_packet_handler)(uint8_t *, const struct pcap_pkthdr *, const uint8_t *);

static uint32_t counts[DURATION + 1];
static struct timeval start_time;

pcap_t *pcap;

char src[] = { 0xec, 0x0d, 0x9a, 0x7e, 0x91, 0x52 };
char dst[] = { 0xec, 0x0d, 0x9a, 0x7e, 0x91, 0x97 };

static void got_packet(uint8_t *user, const struct pcap_pkthdr *header, const uint8_t *packet) {
//    uint8_t buf[6];
//    if (packet[12] == 0x27 && packet[13] == 0xc1) {
//        memcpy(buf, packet, 6);
        memcpy((uint8_t *)packet, dst, 6);
        memcpy((uint8_t *)packet + 6, src, 6);
        pcap_inject(pcap, packet, header->caplen);
        counts[header->ts.tv_sec - start_time.tv_sec]++;
//    }
}

static void my_int_handler(int s) {
    struct timeval now;
    gettimeofday(&now, NULL);
    time_t i;

    printf("Caught signal %d, usec=%lu\n", s, now.tv_usec);

    for (int i = 0; i <= now.tv_sec - start_time.tv_sec; i++) {
        printf("%" PRIu32 "\t", counts[i]);
    }
    printf("\n");

    exit(1);
}

int main() {
    const char *dev = DEV;
    int i;

    char pcap_errbuf[PCAP_ERRBUF_SIZE];
    memset(pcap_errbuf, 0, PCAP_ERRBUF_SIZE);
    pcap = pcap_open_live(dev, 96, 0, 0, pcap_errbuf);
    if (pcap_errbuf[0] != '\0') fprintf(stderr, "%s", pcap_errbuf);
    if (!pcap) return 1;


    memset(counts, 0, DURATION * sizeof(uint32_t));

    struct sigaction act;
    act.sa_handler = my_int_handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;

    sigaction(SIGINT, &act, NULL);

    gettimeofday(&start_time, NULL);

    pcap_loop(pcap, 0, got_packet, (uint8_t *) pcap);

    printf("here!\n");

    return 0;
}