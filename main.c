#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "vec.h"

#define ENTITIES_SIZE 4
#define G 25.f
#define EPS 100.f

#define SIMULATION_DELTA (1.0 / 60.0)

double get_current_time(void) { return (double)clock() / CLOCKS_PER_SEC; }

void scc(int code) {
    if (code < 0) {
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
        exit(1);
    }
}

void *scp(void *ptr) {
    if (ptr == NULL) {
        fprintf(stderr, "SDL ERROR: %s\n", SDL_GetError());
        exit(1);
    }
    return ptr;
}

typedef struct {
    vec2 pos, new_pos, d, dd;
    float radius, mass;
    SDL_Color color;
} entity;

entity entities[ENTITIES_SIZE];

const int window_width = 2000;
const int window_height = 1400;

static int SDL_RenderFillCircle(SDL_Renderer *renderer, entity *c);
static vec2 gravity_acc(entity *c1, entity *c2);
static void render_frame(SDL_Renderer *renderer, entity *entities);

int main(void) {
    scc(SDL_Init(SDL_INIT_VIDEO));

    SDL_Window *window =
        scp(SDL_CreateWindow("three body problem", 0, 0, window_width,
                             window_height, SDL_WINDOW_SHOWN));

    SDL_Renderer *renderer =
        scp(SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED));

    int quarter_width = window_width / 4;

    int mid_height = window_height / 2;

    /* struct timeval start, end; */

    entities[0] = (entity){
        .pos = (vec2){.x = 1 * quarter_width, .y = mid_height},
        .d = (vec2){.y = .3f},
        .radius = 20,
        .mass = 1,
        .color = (SDL_Color){255, 255, 255, 255},
    };
    entities[1] = (entity){
        .pos = (vec2){.x = 3 * quarter_width, .y = mid_height},
        .d = (vec2){.y = -.3f},
        .radius = 20,
        .mass = 1,
        .color = (SDL_Color){0, 0, 255, 255},
    };
    entities[2] = (entity){
        .pos = (vec2){.x = 2 * quarter_width, .y = mid_height - 100},
        .radius = 20,
        .mass = 5,
        .color = (SDL_Color){255, 255, 255, 255},
    };
    entities[3] = (entity){
        .pos = (vec2){.x = 2 * quarter_width, .y = mid_height + 300},
        .radius = 20,
        .mass = 1,
        .color = (SDL_Color){0, 255, 255, 255},
    };
    /* entities[4] = (entity){ */
    /*     .pos = (vec2){.x = 2 * quarter_width - 200, .y = mid_height + 300},
     */
    /*     .radius = 20, */
    /*     .mass = 1, */
    /*     .color = (SDL_Color){255, 0, 0, 255}, */
    /* }; */
    /* entities[5] = (entity){ */
    /*     .pos = (vec2){.x = 2 * quarter_width + 100, .y = mid_height - 300},
     */
    /*     .radius = 20, */
    /*     .mass = 1, */
    /*     .color = (SDL_Color){255, 255, 255, 255}, */
    /* }; */

    bool quit = false;
    double previous_time = get_current_time();
    double lag = 0.f;

    while (!quit) {
        double current_time = get_current_time();
        double elapsed = current_time - previous_time;
        previous_time = current_time;
        lag += elapsed;

        SDL_Event event = {-1};
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT: {
                    quit = true;
                } break;
            }
            break;
        }

        entity *c;
        size_t i, j;
        while (lag >= SIMULATION_DELTA) {
            for (i = 0; i < ENTITIES_SIZE; ++i) {
                c = entities + i;

                c->dd = (vec2){0};

                for (j = 0; j < ENTITIES_SIZE; ++j) {
                    if (i == j) continue;

                    c->dd = vec2_add(c->dd, gravity_acc(c, entities + j));
                }

                c->d = vec2_add(c->d, c->dd);
                c->new_pos = vec2_add(c->pos, c->d);
            }
            lag -= SIMULATION_DELTA;
        }

        render_frame(renderer, entities);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

// source: https://gist.github.com/Gumichan01/332c26f6197a432db91cc4327fcabb1c
static int SDL_RenderFillCircle(SDL_Renderer *renderer, entity *c) {
    SDL_Color color = c->color;

    int x = c->new_pos.x, y = c->new_pos.y, radius = c->radius;

    int offsetx, offsety, d;
    int status;

    offsetx = 0;
    offsety = radius;
    d = radius - 1;
    status = 0;

    scc(SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a));

    while (offsety >= offsetx) {
        status += SDL_RenderDrawLine(renderer, x - offsety, y + offsetx,
                                     x + offsety, y + offsetx);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y + offsety,
                                     x + offsetx, y + offsety);
        status += SDL_RenderDrawLine(renderer, x - offsetx, y - offsety,
                                     x + offsetx, y - offsety);
        status += SDL_RenderDrawLine(renderer, x - offsety, y - offsetx,
                                     x + offsety, y - offsetx);

        if (status < 0) {
            status = -1;
            break;
        }

        if (d >= 2 * offsetx) {
            d -= 2 * offsetx + 1;
            offsetx += 1;
        } else if (d < 2 * (radius - offsety)) {
            d += 2 * offsety - 1;
            offsety -= 1;
        } else {
            d += 2 * (offsety - offsetx - 1);
            offsety -= 1;
            offsetx += 1;
        }
    }

    return status;
}

static vec2 gravity_acc(entity *c1, entity *c2) {
    vec2 dir = vec2_sub(c1->pos, c2->pos);

    // source: https://arxiv.org/pdf/1205.2729
    float softened_distance =
        1 / vec2_length(dir) * pow(vec2_length_squared(dir) + EPS * EPS, 1.5);
    float force = -G * c2->mass / softened_distance;

    vec2 unit_dir = vec2_scalar_mult(dir, 1 / vec2_length(dir));
    return vec2_scalar_mult(unit_dir, force);
}

static inline void render_frame(SDL_Renderer *renderer, entity *entities) {
    scc(SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0));
    scc(SDL_RenderClear(renderer));

    entity *c;
    for (size_t i = 0; i < ENTITIES_SIZE; ++i) {
        c = entities + i;
        c->pos = c->new_pos;
        vec2_print(c->pos);
        scc(SDL_RenderFillCircle(renderer, c));
    }

    SDL_RenderPresent(renderer);
}
