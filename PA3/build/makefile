# set flags and compiler, as well as file var
CC=mpicc
CXXFLAGS=-Wall
LIBS=-lpmi
# feel free to either add files here (for more compilcated src's), or create more vars, or just manually type them in. Up to you.
# uncomment below if/when you have a folder called include that holds potential header files
#INCLUDES=-I../include

OUTBUCKETSEQ = mpi_bucketSequential
FILEBUCKETSEQ = ../src/mpi_bucketSeq.c

OUTBUCKETUTL = mpi_bucketUtility.o
FILEBUCKETUTL = ../src/mpi_bucketUtility.c

OUTBUCKETPAR = mpi_bucketParallel
FILEBUCKETPAR = ../src/mpi_bucketPar.c

all: $(OUTBUCKETSEQ) $(OUTBUCKETPAR)

$(OUTBUCKETSEQ): $(FILEBUCKETSEQ) $(OUTBUCKETUTL)
	$(CC) $(CXXFLAGS) -o $(OUTBUCKETSEQ) $(FILEBUCKETSEQ) $(OUTBUCKETUTL) $(LIBS)

$(OUTBUCKETPAR): $(FILEBUCKETPAR) $(OUTBUCKETUTL)
	$(CC) $(CXXFLAGS) -o $(OUTBUCKETPAR) $(FILEBUCKETPAR) $(OUTBUCKETUTL) $(LIBS)

$(OUTBUCKETUTL): $(FILEBUCKETUTL)
	$(CC) $(CXXFLAGS) -c $(FILEBUCKETUTL)








# use this to run
#run: $(OUTPUTPINGPONG) $(OUTPUTTIMING)
#	sbatch One_box.sh
#	sbatch Two_box.sh
#	sbatch Timing.sh

clean:
	-@if rm $(OUTBUCKETUTL) $(OUTBUCKETSEQ) $(OUTBUCKETPAR) 2>/dev/null || true; then echo "Cleaned"; else echo "Nothing to clean"; fi
