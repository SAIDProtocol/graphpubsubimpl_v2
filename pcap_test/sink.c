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
#define DEV "eth0"

static void (*got_packet_handler)(uint8_t *, const struct pcap_pkthdr *, const uint8_t *);

static uint32_t counts[DURATION + 1];
static struct timeval start_time;

//static void got_later_packet(uint8_t *user, const struct pcap_pkthdr *header, const uint8_t *packet) {
//    time_t diff = header->ts.tv_sec - start_tv_sec;
//    counts[diff]++;
//    if (diff == DURATION) {
//        printf("s.u: %ld, e.u: %ld\n", start_tv_usec, header->ts.tv_usec);
//        for (int i = 0; i <= DURATION; i++) {
//            printf("%" PRIu32 "\t", counts[i]);
//        }
//        printf("\n");
//
//        exit(0);
//    }
//}
//
//static void got_first_packet(uint8_t *user, const struct pcap_pkthdr *header, const uint8_t *packet) {
//    start_tv_sec = header->ts.tv_sec;
//    start_tv_usec = header->ts.tv_usec;
//    counts[0] = 1;
//    got_packet_handler = &got_later_packet;
//}


static void got_packet(uint8_t *user, const struct pcap_pkthdr *header, const uint8_t *packet) {
//    got_packet_handler(user, header, packet);
    counts[header->ts.tv_sec - start_time.tv_sec]++;
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
    pcap_t *pcap = pcap_open_live(dev, 96, 0, 0, pcap_errbuf);
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

