CFLAGS= -O3 -mfpmath=sse -fomit-frame-pointer -funroll-loops -pipe -I.
#CFLAGS= -O0 -march=native -mtune=native -mfpmath=sse -fomit-frame-pointer -funroll-loops -pipe -Wall -I.
SAFE_CFLAGS= -O2 -fomit-frame-pointer -pipe -I.

ifeq ($(MPI),1)
CC= mpicc $(CFLAGS) -DWITH_MPI
else
CC= gcc $(CFLAGS)
endif

all: rmc utils

utils: RM_collision M_RM_diff M_RM_visualize u_calculator

tests: test_Mpi_in_Mp

safe: CC= gcc $(SAFE_CFLAGS)
safe: all

test_Mpi_in_Mp: log.o common.o ideals.o test_Mpi_in_Mp.o
	$(CC) -o tests/test_Mpi_in_Mp log.o common.o ideals.o test_Mpi_in_Mp.o -lgmp

u_calculator: log.o common.o ideals.o u_calculator.o
	$(CC) -o u_calculator log.o common.o ideals.o u_calculator.o -lgmp

M_RM_visualize: log.o common.o ideals.o M_RM_visualize.o
	$(CC) -o M_RM_visualize log.o common.o ideals.o M_RM_visualize.o -lgmp

M_RM_diff: log.o common.o ideals.o M_RM_diff.o
	$(CC) -o M_RM_diff log.o common.o ideals.o M_RM_diff.o -lgmp

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

M_RM_diff.o: M_RM_diff.c ideals.h common.h log.h
	$(CC) -c M_RM_diff.c

M_RM_visualize.o: M_RM_visualize.c ideals.h common.h log.h
	$(CC) -c M_RM_visualize.c

u_calculator.o: u_calculator.c ideals.h common.h log.h
	$(CC) -c u_calculator.c

test_Mpi_in_Mp.o: tests/test_Mpi_in_Mp.c ideals.h common.h log.h
	$(CC) -c tests/test_Mpi_in_Mp.c

clean:
	rm -f *.o rmc RM_collision M_RM_diff M_RM_visualize u_calculator *\~
	rm -f tests/test_Mpi_in_Mp

distclean: clean
	rm -f *.gv
