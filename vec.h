#include <math.h>
#include <stdio.h>

typedef struct {
    float x, y;
} vec2;

static inline vec2 vec2_add(vec2 v1, vec2 v2) {
    return (vec2){
        .x = v1.x + v2.x,
        .y = v1.y + v2.y,
    };
}

static inline vec2 vec2_sub(vec2 v1, vec2 v2) {
    return (vec2){
        .x = v1.x - v2.x,
        .y = v1.y - v2.y,
    };
}

static inline vec2 vec2_scalar_mult(vec2 v, float t) {
    return (vec2){
        .x = v.x * t,
        .y = v.y * t,
    };
}

static inline float vec2_length_squared(vec2 v) {
    return v.x * v.x + v.y * v.y;
}

static inline float vec2_length(vec2 v) {
    return sqrtf(vec2_length_squared(v));
}

static inline vec2 unit_direction(vec2 v1, vec2 v2) {
    vec2 dir = vec2_sub(v1, v2);
    return vec2_scalar_mult(dir, 1 / vec2_length(dir));
}

static inline void vec2_print(vec2 v) {
    printf("{x = %f, y = %f}\n", v.x, v.y);
}
