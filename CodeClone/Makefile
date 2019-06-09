NVCC = nvcc

all: codeClone

%.o : %.cu
	$(NVCC) -c $< -o $@ -std=c++11

codeClone : codeClone.o
	$(NVCC) $^ -o $@ -std=c++11

clean:
	rm -rf *.o *.a codeClone
