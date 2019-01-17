CC=gcc
CFLAGS=-O3
OBJS=gen1 gen2 sink
LDFLAGS=-lpcap -lpthread

all: $(OBJS)


$(OBJS): %: %.o
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $< -c

clean:
	rm -f *.o $(OBJS)

