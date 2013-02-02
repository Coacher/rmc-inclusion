CFLAGS= -O3 -mfpmath=sse -fomit-frame-pointer -funroll-loops -pipe -I.
#CFLAGS= -O0 -march=native -mtune=native -mfpmath=sse -fomit-frame-pointer -funroll-loops -pipe -Wall -I.
SAFE_CFLAGS= -O2 -fomit-frame-pointer -pipe -I.

ifeq ($(MPI),1)
CC= mpicc $(CFLAGS) -DWITH_MPI
else
CC= gcc $(CFLAGS)
endif

all: codes_info

safe: CC= gcc $(SAFE_CFLAGS)
safe: all

codes_info: log.o common.o ideals.o codes_info.o
	$(CC) -o codes_info common.o ideals.o log.o codes_info.o -lgmp

log.o: log.c log.h
	$(CC) -c log.c

common.o: common.c common.h
	$(CC) -c common.c

ideals.o: ideals.c ideals.h common.h
	$(CC) -c ideals.c

codes_info.o: codes_info.c common.h ideals.h log.h
	$(CC) -c codes_info.c

clean:
	rm -f *.o codes_info *\~
