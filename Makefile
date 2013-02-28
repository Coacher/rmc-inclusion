CFLAGS= -O3 -mfpmath=sse -fomit-frame-pointer -funroll-loops -pipe -I.
#CFLAGS= -O0 -march=native -mtune=native -mfpmath=sse -fomit-frame-pointer -funroll-loops -pipe -Wall -I.
SAFE_CFLAGS= -O2 -fomit-frame-pointer -pipe -I.

ifeq ($(MPI),1)
CC= mpicc $(CFLAGS) -DWITH_MPI
else
CC= gcc $(CFLAGS)
endif

all: rmc RM_collision M_RM_visualize

safe: CC= gcc $(SAFE_CFLAGS)
safe: all RM_collision M_RM_visualize

M_RM_visualize: log.o common.o ideals.o M_RM_visualize.o
	$(CC) -o M_RM_visualize log.o common.o ideals.o M_RM_visualize.o -lgmp

RM_collision: log.o common.o ideals.o RM_collision.o
	$(CC) -o RM_collision log.o common.o ideals.o RM_collision.o -lgmp

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

RM_collision.o: RM_collision.c ideals.h common.h log.h
	$(CC) -c RM_collision.c

M_RM_visualize.o: M_RM_visualize.c ideals.h common.h log.h
	$(CC) -c M_RM_visualize.c

clean:
	rm -f *.o rmc RM_collision M_RM_visualize *\~

distclean: clean
	rm -f *.gv
