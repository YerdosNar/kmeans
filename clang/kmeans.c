#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "kmeans.h"

#define MINUS_SQUARE(x2, x1) (((x2) - (x1)) * ((x2) - (x1)))

static int k, num_points;
static Point *points = NULL;
static Cluster *clusters = NULL;

static float distance(Point p1, Point p2) {
    float sum = 0.0f;
    for(int i = 0; i < p1.dimension; i++) {
        sum += MINUS_SQUARE(p2.coordinates[i], p1.coordinates[i]);
    }
    return sqrtf(sum);
}

static void assign_points() {
    for(int i = 0; i < k; i++) {
        clusters[i].size = 0;
    }

    for(int i = 0; i < num_points; i++) {
        int closest = 0;
        float min_dis = distance(points[i], clusters[0].centroid);

        for(int j = 1; j < k; j++) {
            float dis = distance(points[i], clusters[j].centroid);
            if(dis < min_dis) {
                min_dis = dis;
                closest = j;
            }
        }
        clusters[closest].points[clusters[closest].size++] = points[i];
    }
}

static void update_centroids() {
    for(int i = 0; i < k; i++) {
        if(clusters[i].size == 0) continue;

        for(int d = 0; d < clusters[i].centroid.dimension; d++) {
            clusters[i].centroid.coordinates[d] = 0.0f;
        }

        for(int j = 0; j < clusters[i].size; j++) {
            for(int d = 0; d < clusters[i].centroid.dimension; d++) {
                clusters[i].centroid.coordinates[d] += clusters[i].points[j].coordinates[d];
            }
        }

        for(int d = 0; d < clusters[i].centroid.dimension; d++) {
            clusters[i].centroid.coordinates[d] /= clusters[i].size;
        }
    }
}

void init_kmeans_wait(int num_clusters, int np, Point *data_points) {
    k = num_clusters;
    num_points = np;
    points = data_points;

    clusters = malloc(k * sizeof(Cluster));
    for(int i = 0; i < k; i++) {
        clusters[i].points = malloc(num_points * sizeof(Point));
        clusters[i].size = 0;

        int idx = rand() % num_points;
        clusters[i].centroid.dimension = points[idx].dimension;
        clusters[i].centroid.coordinates = malloc(points[idx].dimension * sizeof(float));
        memcpy(clusters[i].centroid.coordinates, points[idx].coordinates, points[idx].dimension * sizeof(float));
    }
}

void init_kmeans(int num_clusters, int np, Point *data_points) {
    k = num_clusters;
    num_points = np;
    points = data_points;

    clusters = malloc(k * sizeof(Cluster));
    for (int i = 0; i < k; i++) {
        clusters[i].points = malloc(num_points * sizeof(Point));
        clusters[i].size = 0;
        clusters[i].centroid.dimension = points[0].dimension;
        clusters[i].centroid.coordinates = malloc(points[0].dimension * sizeof(float));
    }

    // 1. Choose the first centroid randomly
    int first_idx = rand() % num_points;
    memcpy(clusters[0].centroid.coordinates, points[first_idx].coordinates,
           points[first_idx].dimension * sizeof(float));

    // 2. Choose the remaining centroids
    for (int c = 1; c < k; c++) {
        float *distances = malloc(num_points * sizeof(float));
        float total = 0.0f;

        for (int i = 0; i < num_points; i++) {
            float min_dist = distance(points[i], clusters[0].centroid);
            for (int j = 1; j < c; j++) {
                float d = distance(points[i], clusters[j].centroid);
                if (d < min_dist) {
                    min_dist = d;
                }
            }
            distances[i] = min_dist * min_dist; // square it
            total += distances[i];
        }

        // Choose a point based on weighted probability
        float r = ((float)rand() / RAND_MAX) * total;
        float sum = 0.0f;
        int next_idx = 0;
        for (int i = 0; i < num_points; i++) {
            sum += distances[i];
            if (sum >= r) {
                next_idx = i;
                break;
            }
        }

        memcpy(clusters[c].centroid.coordinates, points[next_idx].coordinates,
               points[next_idx].dimension * sizeof(float));

        free(distances);
    }
}

void one_iter() {
    assign_points();
    update_centroids();
}

void run_kmeans(int max_iteration) {
    for(int iter = 0; iter < max_iteration; iter++) {
        one_iter();
    }
}

Point *get_centroids() {
    Point *centroids = malloc(k * sizeof(Point));
    for(int i = 0; i < k; i++) {
        centroids[i].dimension = clusters[i].centroid.dimension;
        centroids[i].coordinates = malloc(centroids[i].dimension * sizeof(float));
        memcpy(centroids[i].coordinates, clusters[i].centroid.coordinates,
               centroids[i].dimension * sizeof(float));
    }
    return centroids;
}

Cluster *get_clusters() {
    return clusters;
}

void free_kmeans() {
    if(!clusters) return;

    for(int i = 0; i < k; i++) {
        free(clusters[i].centroid.coordinates);
        free(clusters[i].points);
    }
    free(clusters);
    clusters = NULL;
}
