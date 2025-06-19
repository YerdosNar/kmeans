#include <stdlib.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "kmeans.h"

#define WIDTH 800
#define HEIGHT 800

int reverse_int(int i) {
    unsigned char c1, c2, c3, c4;
    c1 = i & 255;
    c2 = (i >> 8) & 255;
    c3 = (i >> 16) & 255;
    c4 = (i >> 24) & 255;

    return ((int)c1 << 24) + ((int)c2 << 16) + ((int)c3 << 8) + c4;
}

float *read_mnist_images(const char* filename, int* count) {
    FILE *fp = fopen(filename, "rb");
    if(!fp) {
        fprintf(stderr, "Could NOT open \"%s\" file.\n", filename);
        exit(1);
    }

    int mn = 0;
    if(fread(&mn, sizeof(int), 1, fp) != 1) {
        fprintf(stderr, "Failed to read magic number\n");
        fclose(fp);
        exit(1);
    }
    mn = reverse_int(mn);
    if(mn != 2051) {
        fprintf(stderr, "Invalid MNIST file: magic number %d (expected 2051)\n", mn);
        fclose(fp);
        exit(1);
    }

    if(fread(count, sizeof(int), 1, fp) != 1) {
        fprintf(stderr, "Failed to read image count\n");
        fclose(fp);
        exit(1);
    }
    *count = reverse_int(*count);

    int rows, cols;
    if(fread(&rows, sizeof(int), 1, fp) != 1 ||
       fread(&cols, sizeof(int), 1, fp) != 1) {
        fprintf(stderr, "Failed to read dimensions\n");
        fclose(fp);
        exit(1);
    }
    rows = reverse_int(rows);
    cols = reverse_int(cols);

    size_t img_size = *count * rows * cols;
    unsigned char *buff = malloc(img_size);
    if(!buff) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(fp);
        exit(1);
    }

    if(fread(buff, sizeof(unsigned char), img_size, fp) != img_size) {
        fprintf(stderr, "Failed to read image data\n");
        free(buff);
        fclose(fp);
        exit(1);
    }
    fclose(fp);

    float *imgs = malloc(img_size * sizeof(float));
    if(!imgs) {
        fprintf(stderr, "Memory allocation failed\n");
        free(buff);
        exit(1);
    }

    for(size_t i = 0; i < img_size; i++) {
        imgs[i] = buff[i] / 255.0f;
    }

    free(buff);
    return imgs;
}

void save_centroids_as_images(Point *centroids, int k) {
    if(SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return;
    }

    for(int i = 0; i < k; i++) {
        // Create surface with standard RGBA format
        SDL_Surface *surface = SDL_CreateRGBSurfaceWithFormat(0, 28, 28, 32, SDL_PIXELFORMAT_RGBA32);
        if(!surface) {
            fprintf(stderr, "Surface creation failed: %s\n", SDL_GetError());
            continue;
        }

        // Direct pixel access without locking (SDL2 handles this automatically)
        Uint32 *pixels = (Uint32 *)surface->pixels;
        SDL_PixelFormat *format = surface->format;

        for(int y = 0; y < 28; y++) {
            for(int x = 0; x < 28; x++) {
                int idx = y * 28 + x;
                Uint8 gray = (Uint8)(centroids[i].coordinates[idx] * 255.0f);
                Uint32 pixel = SDL_MapRGBA(format, gray, gray, gray, 255);
                pixels[y * 28 + x] = pixel;
            }
        }

        char filename[32];
        snprintf(filename, sizeof(filename), "centroid_%d.bmp", i);
        if(SDL_SaveBMP(surface, filename)) {
            fprintf(stderr, "Failed to save %s: %s\n", filename, SDL_GetError());
        }

        SDL_FreeSurface(surface);
    }

    SDL_Quit();
}

int main() {
    const char *filename = "train-images-idx3-ubyte";
    int num_imgs;
    float *imgs = read_mnist_images(filename, &num_imgs);

    Point *points = malloc(num_imgs * sizeof(Point));
    if(!points) {
        fprintf(stderr, "Memory allocation failed\n");
        free(imgs);
        return 1;
    }

    for(int i = 0; i < num_imgs; i++) {
        points[i].dimension = 784;
        points[i].coordinates = &imgs[i * 784];
    }

    init_kmeans(10, num_imgs, points);
    run_kmeans(200);

    Point *centroids = get_centroids();
    save_centroids_as_images(centroids, 10);

    // Cleanup memory
    for(int i = 0; i < 10; i++) {
        free(centroids[i].coordinates);
    }
    free(centroids);

    free_kmeans();
    free(points);
    free(imgs);

    return 0;
}
