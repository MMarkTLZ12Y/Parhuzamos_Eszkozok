#define CL_TARGET_OPENCL_VERSION 120 

#include "../../include/opencl/grayscale_ocl.h"
#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <omp.h> // A stopperórához

#define MAX_SOURCE_SIZE (0x100000)

double run_grayscale_opencl(unsigned char* image, int width, int height, int channels, int is_2d) {
    FILE *fp;
    char *source_str;
    size_t source_size;

    // Kernel beolvasása a fájlból
    fp = fopen("kernels/grayscale.cl", "r");
    if (!fp) {
        fprintf(stderr, "Hiba: Nem sikerult betolteni a grayscale.cl fajlt.\n");
        return -1.0;
    }
    source_str = (char*)malloc(MAX_SOURCE_SIZE);
    source_size = fread(source_str, 1, MAX_SOURCE_SIZE, fp);
    fclose(fp);

    // OpenCL inicializálása (Eszközök megkeresése)
    cl_platform_id platform_id = NULL;
    cl_device_id device_id = NULL;   
    cl_uint ret_num_devices;
    cl_uint ret_num_platforms;
    cl_int ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
    ret = clGetDeviceIDs(platform_id, CL_DEVICE_TYPE_DEFAULT, 1, &device_id, &ret_num_devices);

    cl_context context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &ret);
    cl_command_queue command_queue = clCreateCommandQueue(context, device_id, 0, &ret);

    // Memória lefoglalása a videókártyán
    cl_mem image_mem_obj = clCreateBuffer(context, CL_MEM_READ_ONLY, width * height * channels, NULL, &ret);
    cl_mem result_mem_obj = clCreateBuffer(context, CL_MEM_WRITE_ONLY, width * height * channels, NULL, &ret);

    // Kernel program fordítása (Ezt sem mérjük bele az időbe, mert ez csak egyszeri "betöltés")
    cl_program program = clCreateProgramWithSource(context, 1, (const char **)&source_str, (const size_t *)&source_size, &ret);
    ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);

    cl_kernel kernel;

    // =========================================================================
    //   STOPPERÓRA INDÍTÁSA - "FAIR" MÉRÉS (Aktív)
    //   (Ebbe belemérjük a RAM -> VRAM másolás idejét is. Ez adja a metszéspontot!)
    // =========================================================================
    double start_time = omp_get_wtime();

    // 1. Lépés: Adat áttolása a gép memóriájából (RAM) a videókártyára (VRAM)
    ret = clEnqueueWriteBuffer(command_queue, image_mem_obj, CL_TRUE, 0, width * height * channels, image, 0, NULL, NULL);

    // -------------------------------------------------------------------------
    // /* ALTERNATÍV STOPPER INDÍTÁS - NYERS SZÁMÍTÁSI ERŐ (Kikommentezve) */
    // /* Ha a metszéspont NÉLKÜLI grafikont akarod: kommentezd ki a fenti start_time-ot, 
    //    és töröld ki a kommentet az alábbi sor elől! */
    // // double start_time = omp_get_wtime(); 
    // -------------------------------------------------------------------------

    if (is_2d == 0) {
        // --- 1D KERNEL BEÁLLÍTÁSA ---
        kernel = clCreateKernel(program, "grayscale_1d", &ret);
        clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&image_mem_obj);
        clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&result_mem_obj);
        clSetKernelArg(kernel, 2, sizeof(int), (void *)&channels);

        size_t global_item_size = width * height; // 1 Dimenzió
        ret = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_item_size, NULL, 0, NULL, NULL);
    } else {
        // --- 2D KERNEL BEÁLLÍTÁSA ---
        kernel = clCreateKernel(program, "grayscale_2d", &ret);
        clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&image_mem_obj);
        clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&result_mem_obj);
        clSetKernelArg(kernel, 2, sizeof(int), (void *)&width);
        clSetKernelArg(kernel, 3, sizeof(int), (void *)&channels);

        size_t global_item_size[2] = {width, height}; // 2 Dimenzió (X és Y)
        ret = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global_item_size, NULL, 0, NULL, NULL);
    }

    // Megvárjuk, amíg a GPU befejezi a matekot
    ret = clFinish(command_queue);

    // -------------------------------------------------------------------------
    // /* ALTERNATÍV STOPPER LEÁLLÍTÁS - NYERS SZÁMÍTÁSI ERŐ (Kikommentezve) */
    // /* Ha a metszéspont NÉLKÜLI grafikont akarod: töröld ki a kommentet az alábbi sor elől, 
    //    és kommentezd ki a legalsó end_time-ot! */
    // // double end_time = omp_get_wtime(); 
    // -------------------------------------------------------------------------

    // 2. Lépés: Eredmény visszaolvasása a GPU-ról (VRAM) a gép memóriájába (RAM)
    ret = clEnqueueReadBuffer(command_queue, result_mem_obj, CL_TRUE, 0, width * height * channels, image, 0, NULL, NULL);

    // =========================================================================
    //   STOPPERÓRA LEÁLLÍTÁSA - "FAIR" MÉRÉS (Aktív)
    //   (Ebbe belemértük a VRAM -> RAM másolás idejét is!)
    // =========================================================================
    double end_time = omp_get_wtime();

    // Takarítás (Memória felszabadítása)
    ret = clReleaseKernel(kernel);
    ret = clReleaseProgram(program);
    ret = clReleaseMemObject(image_mem_obj);
    ret = clReleaseMemObject(result_mem_obj);
    ret = clReleaseCommandQueue(command_queue);
    ret = clReleaseContext(context);
    free(source_str);

    // Visszatérünk a mért idővel (Futás + Oda-vissza másolás)
    return end_time - start_time;
}