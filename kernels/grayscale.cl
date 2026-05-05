// EREDETI: 1 Dimenziós feldolgozás (Szalagként látja a képet)
__kernel void grayscale_1d(__global const unsigned char* input,
                           __global unsigned char* output,
                           const int channels) {
    int gid = get_global_id(0);
    int idx = gid * channels;
    
    unsigned char r = input[idx];
    unsigned char g = input[idx + 1];
    unsigned char b = input[idx + 2];
    unsigned char gray = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);
    
    output[idx] = gray;
    output[idx + 1] = gray;
    output[idx + 2] = gray;
    if (channels == 4) output[idx + 3] = input[idx + 3];
}

// ÚJ: 2 Dimenziós feldolgozás (Mátrixként / Csempékként látja a képet)
__kernel void grayscale_2d(__global const unsigned char* input,
                           __global unsigned char* output,
                           const int width,
                           const int channels) {
    int x = get_global_id(0); // X koordináta (Oszlop)
    int y = get_global_id(1); // Y koordináta (Sor)
    
    int idx = (y * width + x) * channels;
    
    unsigned char r = input[idx];
    unsigned char g = input[idx + 1];
    unsigned char b = input[idx + 2];
    unsigned char gray = (unsigned char)(0.299f * r + 0.587f * g + 0.114f * b);
    
    output[idx] = gray;
    output[idx + 1] = gray;
    output[idx + 2] = gray;
    if (channels == 4) output[idx + 3] = input[idx + 3];
}