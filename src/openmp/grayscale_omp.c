#include <stdio.h>
#include <omp.h>
#include "../../include/openmp/grayscale_omp.h"

// Ez a függvény visszatérési értékként megadja a futási időt (double)
double run_grayscale_openmp(unsigned char* image, int width, int height, int channels) {
    printf("\n--- OpenMP Konverzio Indul ---\n");
    int num_pixels = width * height;
    
    double start_time = omp_get_wtime();

    #pragma omp parallel for
    for (int i = 0; i < num_pixels; i++) {
        int pixel_index = i * channels;
        unsigned char r = image[pixel_index];
        unsigned char g = image[pixel_index + 1];
        unsigned char b = image[pixel_index + 2];
        unsigned char gray = (unsigned char)(0.299 * r + 0.587 * g + 0.114 * b);
        image[pixel_index] = gray;
        image[pixel_index + 1] = gray;
        image[pixel_index + 2] = gray;
    }

    double end_time = omp_get_wtime();
    double elapsed = end_time - start_time;
    
    printf("OpenMP konverzio kesz! Ido: %f masodperc.\n", elapsed);
    return elapsed;
}