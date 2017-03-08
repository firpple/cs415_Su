# set flags and compiler, as well as file var
CC=mpicc
CXXFLAGS=-Wall
LIBS=-lpmi
# feel free to either add files here (for more compilcated src's), or create more vars, or just manually type them in. Up to you.
FILEPINGPONG= src/mpi_pingpong.c
FILETIMING= src/mpi_packetCheck.c
# uncomment below if/when you have a folder called include that holds potential header files
#INCLUDES=-I../include
OUTPUTPINGPONG=mpi_pingpong
OUTPUTTIMING=mpi_packetCheck

OUTMANDELSEQ = mpi_mandelSeq
FILEMANDELSEQ = src/mpi_mandelSeq.c

OUTMANDELUTL = mpi_mandelUtility.o
FILEMANDELUTL = src/mpi_mandelUtility.c
all: $(OUTMANDELSEQ)

$(OUTMANDELSEQ): $(FILEMANDELSEQ) $(OUTMANDELUTL)
	$(CC) $(CXXFLAGS) -o $(OUTMANDELSEQ) $(FILEMANDELSEQ) $(OUTMANDELUTL) $(LIBS)

$(OUTMANDELUTL): $(FILEMANDELUTL)
	$(CC) $(CXXFLAGS) -c $(FILEMANDELUTL)

# use this to run
#run: $(OUTPUTPINGPONG) $(OUTPUTTIMING)
#	sbatch One_box.sh
#	sbatch Two_box.sh
#	sbatch Timing.sh

clean:
	-@if rm $(OUTMANDELUTL) $(OUTMANDELSEQ) 2>/dev/null || true; then echo "Cleaned"; else echo "Nothing to clean"; fi
