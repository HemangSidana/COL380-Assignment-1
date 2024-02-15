all: pthreads openmp

pthreads: pthreads.cpp
	g++-13 -lpthread -o pthreads pthreads.cpp

openmp: openmp.cpp
	g++-13 -fopenmp -o openmp openmp.cpp

clean:
	rm -f pthreads openmp