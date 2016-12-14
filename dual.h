#ifndef __DUAL_H
#define __DUAL_H

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

#define THOU 1024

#define NORTH 0
#define EAST  90
#define SOUTH 180
#define WEST  270

#define SCREEN_WIDTH       640
#define HALF_SCREEN_WIDTH  (SCREEN_WIDTH / 2)
#define SCREEN_HEIGHT      480
#define HALF_SCREEN_HEIGHT (SCREEN_HEIGHT / 2)

#define MAX_WALLS          3
#define MAX_WALLS_PLUS_ONE (MAX_WALLS + 1)
#define MAX_COLOUR_CHANGES 10

#define BLACK   0b000
#define BLUE    0b001
#define GREEN   0b010
#define CYAN    0b011
#define RED     0b100
#define MAGENTA 0b101
#define YELLOW  0b110
#define WHITE   0b111

#define COLOUR_END 0xFF

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define ABS(X) (((X) < 0) ? -(X) : (X))


struct stri { // screen triangle
    int16_t tx, ty, sx, sy, bx, by;
    int8_t z;
    int8_t colour;
};

struct stris {
    int8_t num;
    struct stri ob[MAX_WALLS];
};

struct crit_point { 
    int16_t p; // horizontal position
    int8_t stri_idx; // index into sorted panels->ob
    // each panel will start and stop exactly once, and the start will not be right of the stop
    //int8_t is_start; // 1 - is start, 0 - is end
};

struct crit_points {
    int8_t num;
    struct crit_point ob[MAX_WALLS * 2];
};

struct change {
    int16_t start;
    int8_t colour;
};

struct changes {
    int8_t num;
    struct change ob[MAX_WALLS * 2 + 1];
};

int16_t mulsine(int16_t num, uint8_t ang);
int16_t mulcos(int16_t num, uint8_t ang);

// run once per frame
void sort_stris();

// run once per scanline - this is time critical
void stris_to_crit_points(const uint16_t line);
void sort_crit_points();               // sort
void crit_points_to_changes();

#endif
