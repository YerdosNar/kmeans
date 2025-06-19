#ifndef KMEANS_H
#define KMEANS_H

#include <stddef.h>

typedef struct {
    size_t dimension;
    float *coordinates;
} Point;

typedef struct {
    Point *points;
    int size;
    Point centroid;
} Cluster;

void init_kmeans(int num_clusters, int num_points, Point *data_points);
void one_iter();
void run_kmeans(int max_iterations);
Point *get_centroids();
Cluster *get_clusters();
void free_kmeans();

#endif
