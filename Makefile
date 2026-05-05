CC = gcc
CFLAGS = -fopenmp -Wall
LDFLAGS = -lOpenCL

# A forrásfájljaink listája
SRC = main.c src/openmp/grayscale_omp.c src/opencl/grayscale_ocl.c src/common/stb_impl.c
OUT = szurke_program.exe

# Alapértelmezett parancs
all: $(OUT)

$(OUT): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(OUT) $(LDFLAGS)

clean:
	rm -f $(OUT)