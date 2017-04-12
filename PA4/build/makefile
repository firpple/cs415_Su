# set flags and compiler, as well as file var
CC=mpicc
CXXFLAGS=-Wall
LIBS=-lpmi
# feel free to either add files here (for more compilcated src's), or create more vars, or just manually type them in. Up to you.
# uncomment below if/when you have a folder called include that holds potential header files
#INCLUDES=-I../include

OUTMATRIXSEQ = mpi_matrixSequential
FILEMATRIXSEQ = ../src/mpi_matrixSeq.c

OUTMATRIXPAR = mpi_matrixParallel
FILEMATRIXPAR = ../src/mpi_matrixPar.c

all: $(OUTMATRIXSEQ)

$(OUTMATRIXSEQ): $(FILEMATRIXSEQ)
	$(CC) $(CXXFLAGS) -o $(OUTMATRIXSEQ) $(FILEMATRIXSEQ) $(LIBS)

$(OUTMATRIXPAR): $(FILEMATRIXPAR)
	$(CC) $(CXXFLAGS) -o $(OUTMATRIXPAR) $(FILEMATRIXPAR) $(LIBS)









# use this to run
#run: $(OUTPUTPINGPONG) $(OUTPUTTIMING)
#	sbatch One_box.sh
#	sbatch Two_box.sh
#	sbatch Timing.sh

clean:
	-@if rm $(OUTMATRIXSEQ) $(OUTMATRIXPAR) 2>/dev/null || true; then echo "Cleaned"; else echo "Nothing to clean"; fi
