CC=gcc
CFLAGS=-I ./
LDFLAGS=-static
OBJS=motion_angle.o
BIN=motion_angle

%.o:%.c
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDFLAGS) -pthread -lm

clean:
	rm -f ./*.o $(BIN)

