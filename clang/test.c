#include <stdio.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <time.h>
#include "kmeans.h"

#define WIDTH 800
#define HEIGHT 800
#define MAX_POINTS 10000

int k;
SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
void init_sdl(char *label) {
    if(SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow(label, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    if(!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if(!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }
}

void draw(Cluster *clusters) {
    // Predefined safe, visually distinct colors (up to 10)
    SDL_Color color_list[10] = {
        {255, 0, 0, 255},      // red
        {0, 255, 0, 255},      // green
        {0, 0, 255, 255},      // blue
        {255, 255, 0, 255},    // yellow
        {255, 0, 255, 255},    // magenta
        {0, 255, 255, 255},    // cyan
        {255, 128, 0, 255},    // orange
        {128, 0, 255, 255},    // purple
        {0, 128, 255, 255},    // sky blue
        {128, 255, 0, 255}     // lime
    };

    for(int i = 0 ; i < k; i++) {
        SDL_Color color;
        if (i < 10) {
            color = color_list[i];
        } else {
            // Fallback to procedural colors using HSV to RGB
            float hue = (float)(i * 360 / k);
            float r, g, b;
            int h_i = ((int)(hue / 60)) % 6;
            float f = (hue / 60) - h_i;
            float v = 1.0f;
            float s = 1.0f;
            float p = v * (1 - s);
            float q = v * (1 - f * s);
            float t = v * (1 - (1 - f) * s);

            switch (h_i) {
                case 0: r = v; g = t; b = p; break;
                case 1: r = q; g = v; b = p; break;
                case 2: r = p; g = v; b = t; break;
                case 3: r = p; g = q; b = v; break;
                case 4: r = t; g = p; b = v; break;
                case 5: r = v; g = p; b = q; break;
                default: r = g = b = 1.0f; break;
            }

            color.r = (Uint8)(r * 255);
            color.g = (Uint8)(g * 255);
            color.b = (Uint8)(b * 255);
            color.a = 255;
        }

        SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
        for(int j = 0; j < clusters[i].size; j++) {
            int x = (int)clusters[i].points[j].coordinates[0];
            int y = (int)clusters[i].points[j].coordinates[1];
            SDL_RenderDrawPoint(renderer, x, y);
            SDL_Rect rect = { x-1, y-1, 2, 2 };
            SDL_RenderDrawRect(renderer, &rect);
        }

        // Draw centroid
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        int cent_x = (int) clusters[i].centroid.coordinates[0];
        int cent_y = (int) clusters[i].centroid.coordinates[1];
        SDL_Rect rect = { cent_x-5, cent_y-5, 10, 10 };
        SDL_RenderDrawRect(renderer, &rect);
    }
}

void draw_points(int interactive) {
    init_sdl("K-Means");

    int running = 1;
    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = 0;
            } else if(interactive && event.type == SDL_KEYDOWN) {
                if(event.key.keysym.sym == SDLK_SPACE) {
                    one_iter();
                } else if(event.key.keysym.sym == SDLK_ESCAPE) {
                    running = 0;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        Cluster *clusters = get_clusters();
        draw(clusters);

        SDL_RenderPresent(renderer);

        if(!interactive) {
            SDL_Delay(2000);
            running = 0;
        }

        SDL_Delay(50);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void draw_with_mouse() {
    init_sdl("Draw mode");

    Point *drawn_points = malloc(MAX_POINTS * sizeof(Point));
    int num_points = 0;
    int running = 1;
    int started = 0;
    int k_value = 0;

    for(int i = 0; i < MAX_POINTS; i++) {
        drawn_points[i].dimension = 2;
        drawn_points[i].coordinates = malloc(2 * sizeof(float));
    }

    while(running) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            if(event.type == SDL_QUIT) {
                running = 0;
            } else if(event.type == SDL_MOUSEBUTTONDOWN && !started && num_points < MAX_POINTS) {
                drawn_points[num_points].coordinates[0] = event.button.x;
                drawn_points[num_points].coordinates[1] = event.button.y;
                num_points++;
            } else if(event.type == SDL_KEYDOWN) {
                SDL_Keycode key = event.key.keysym.sym;
                if(key == SDLK_RETURN && !started && num_points >= 1) {
                    printf("Enter number of clusters (k): ");
                    if(scanf("%d", &k_value) != 1 || k_value <= 0 || k_value > num_points) {
                        printf("Invalid k. Must be between 1 and %d\n", num_points);
                        while(getchar() != '\n');
                        continue;
                    }
                    k = k_value;
                    init_kmeans(k, num_points, drawn_points);
                    started = 1;
                } else if(key == SDLK_SPACE && started) {
                    one_iter();
                } else if(key == SDLK_ESCAPE) {
                    if(started) free_kmeans();
                    started = 0;
                } else if(key == SDLK_q) {
                    running = 0;
                }
            }
        }

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);

        SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
        for(int i = 0; i < num_points; i++) {
            int x = (int)drawn_points[i].coordinates[0];
            int y = (int)drawn_points[i].coordinates[1];
            SDL_RenderDrawPoint(renderer, x, y);
        }

        if(started) {
            Cluster *clusters = get_clusters();
            draw(clusters);
        }

        SDL_RenderPresent(renderer);
        SDL_Delay(16);
    }

    if(started) free_kmeans();

    for(int i = 0; i < MAX_POINTS; i++) {
        free(drawn_points[i].coordinates);
    }
    free(drawn_points);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

Point *generate(int ps) {
    Point *points = malloc(ps * sizeof(Point));
    for(int i = 0; i < ps; i++) {
        points[i].coordinates = malloc(2 * sizeof(float));
        points[i].dimension = 2;
        points[i].coordinates[0] = rand() % WIDTH;
        points[i].coordinates[1] = rand() % HEIGHT;
    }
    return points;
}

int main() {
    srand(time(NULL));

    printf("(D)raw points with mouse\n(G)enerate randomly\n(Q)uit\nChoice: ");
    char choice = getchar();
    while(getchar() != '\n');

    if(choice == 'G' || choice == 'g') {
        int num_points;

        printf("Number of points: ");
        if(scanf("%d", &num_points) != 1 || num_points < 1) {
            printf("Invalid input\n");
            return 1;
        }
        while(getchar() != '\n');

        printf("Number of clusters (k): ");
        if(scanf("%d", &k) != 1 || k < 1 || k > num_points) {
            printf("Invalid k. Must be between 1 and %d\n", num_points);
            return 1;
        }
        while(getchar() != '\n');

        Point *points = generate(num_points);
        init_kmeans(k, num_points, points);

        printf("Step-by-step? (y/n): ");
        char step_mode = getchar();
        while(getchar() != '\n');

        if(step_mode == 'y' || step_mode == 'Y') {
            draw_points(1);
        } else {
            run_kmeans(100);
            draw_points(0);
        }

        free_kmeans();
        for(int i = 0; i < num_points; i++) {
            free(points[i].coordinates);
        }
        free(points);
    }
    else if(choice == 'D' || choice == 'd') {
        draw_with_mouse();
    }

    return 0;
}
