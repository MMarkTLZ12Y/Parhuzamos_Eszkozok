#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/stb/stb_image.h"
#include "include/stb/stb_image_write.h"
#include "include/openmp/grayscale_omp.h"
#include "include/opencl/grayscale_ocl.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Hasznalat: %s <bemeneti_kep> <kimeneti_kep>\n", argv[0]);
        return 1;
    }

    const char* input_file = argv[1];
    const char* output_file = argv[2];

    int width, height, channels;
    unsigned char *image = stbi_load(input_file, &width, &height, &channels, 0);

    if (image == NULL) {
        printf("Hiba: %s\n", input_file);
        return 1;
    }

    // --- 1. OpenMP ---
    double omp_time = run_grayscale_openmp(image, width, height, channels);

    // --- 2. OpenCL (1D) ---
    stbi_image_free(image);
    image = stbi_load(input_file, &width, &height, &channels, 0);
    double ocl_1d_time = run_grayscale_opencl(image, width, height, channels, 0); // 0 = 1D mód

    // --- 3. OpenCL (2D) ---
    stbi_image_free(image);
    image = stbi_load(input_file, &width, &height, &channels, 0);
    double ocl_2d_time = run_grayscale_opencl(image, width, height, channels, 1); // 1 = 2D mód

    // Kimentés
    if (strcmp(output_file, "none") != 0) {
        stbi_write_png(output_file, width, height, channels, image, width * channels);
    }
    stbi_image_free(image);
    
    // CSV MENTÉS (4 oszlop)
    FILE *csv_file = fopen("data/results/performance.csv", "a");
    if (csv_file != NULL) {
        fprintf(csv_file, "%d, %f, %f, %f\n", width * height, omp_time, ocl_1d_time, ocl_2d_time);
        fclose(csv_file);
    }
    
    return 0;
}