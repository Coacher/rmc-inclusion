CFLAGS= -O3 -mfpmath=sse -fomit-frame-pointer -funroll-loops -pipe -I.
#CFLAGS= -O0 -march=native -mtune=native -mfpmath=sse -fomit-frame-pointer -funroll-loops -pipe -Wall -I.
SAFE_CFLAGS= -O2 -fomit-frame-pointer -pipe -I.

ifeq ($(MPI),1)
CC= mpicc $(CFLAGS) -DWITH_MPI
else
CC= gcc $(CFLAGS)
endif

all: rmc

safe: CC= gcc $(SAFE_CFLAGS)
safe: all

rmc: log.o common.o ideals.o info.o graph.o rmc.o
	$(CC) -o rmc log.o common.o ideals.o info.o graph.o rmc.o -lgmp

log.o: log.c log.h
	$(CC) -c log.c

common.o: common.c common.h
	$(CC) -c common.c

ideals.o: ideals.c ideals.h common.h
	$(CC) -c ideals.c

info.o: info.c info.h ideals.h common.h
	$(CC) -c info.c

graph.o: graph.c graph.h ideals.h common.h
	$(CC) -c graph.c

rmc.o: rmc.c graph.h info.h ideals.h common.h log.h
	$(CC) -c rmc.c

clean:
	rm -f *.o rmc *\~
