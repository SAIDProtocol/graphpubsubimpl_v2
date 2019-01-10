#include <assert.h>
#include <pcap/pcap.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define MIN_ETH_SIZE 14
#define BUF_SIZE 125
#define DURATION 5
#define DEV "eth0"

int v = 1;

void *wait_and_stop(void *pram) {
    usleep(DURATION * 1000 * 1000);
    v = 0;
    return NULL;
}


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

    pthread_t thread;

    if (pthread_create(&thread, NULL, wait_and_stop, NULL)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }

    int count = 0, ret;
    while (v) {
        for (i = 0; i < 8192; i++)
            ret = pcap_inject(pcap, buf, BUF_SIZE);
//			if (ret < 0) {
//				pcap_perror(pcap, "");
//				goto cleanup;
//			}
        count += i;
    }
    cleanup:
    printf("count=%d\n", count);
    pcap_close(pcap);
    return 0;
}

