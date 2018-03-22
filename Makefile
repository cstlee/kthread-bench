CC=g++
INCLUDES=/usr/include/x86_64-linux-gnu
LDLIBS=-lrt
all: dispatch worker

%.o: %.cc 
	$(CC) -c $< -I $(INCLUDES)

dispatch: dispatch.o
	$(CC) -o $@ $^ $(LDLIBS)

worker: worker.o
	$(CC) -o $@ $^ $(LDLIBS)

.PHONY: clean

clean:
	rm -f dispatch dispatch.o worker worker.o
