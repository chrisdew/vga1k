/*
 * This is the code which is compiled for both amd64 and lpc810 targets.
 */

#include "dual.h"

int16_t mulsine(int16_t num, uint8_t ang) {
    // https://en.wikipedia.org/wiki/Bhaskara_I's_sine_approximation_formula
    int sign = 1;
    if (ang > 128) { 
	ang -= 128;
        sign = -1;
    }
    return (int16_t) (sign * ((int32_t)num * 4 * ang * (128 - ang) / (20480 - ang * (128 - ang))));
}

int16_t mulcos(int16_t num, uint8_t ang) {
    return mulsine(num, ang + 64);
}

extern struct stris stris;
extern struct crit_points crit_points;
extern struct changes changes;

// bubble sort as we're only dealing with 5-10 items and do this only once per frame
void sort_stris() {
    struct stri tmp;
    int8_t sorted = 0;
    while (!sorted) {
        sorted = 1; // hypothesise that the panels are sorted
        for (int i = 0; i < stris.num - 1; i++) {
            struct stri a = *(stris.ob + i);
            struct stri b = *(stris.ob + i + 1);
            if (a.z > b.z) {
                tmp = a;
                a = b;
                b = tmp;
                sorted = 0;
                //printf("X");
            } else {
                //printf(".");
            }
        }
        //printf("\n");
    }
}

void stris_to_crit_points(const uint16_t line) {
    uint16_t offset = ABS(line - SCREEN_HEIGHT / 2); // offset from centre line
    struct crit_point *crit_point = crit_points.ob;
    crit_points.num = 0;
    for (int i = 0; i < stris.num; i++) {
        const struct stri *stri = stris.ob + i;

        const int16_t tx = stri->tx; 
        const int16_t ty = stri->ty; 
        const int16_t sx = stri->sx; 
        const int16_t sy = stri->sy; 
        const int16_t bx = stri->bx; 
        const int16_t by = stri->by; 

	if (line < ty) {
            printf("too high\n");
	    continue;
	}

	if (line >= by) {
	    printf("too low\n");
	    continue;
	}

	const int16_t th = sy - ty;
        const int16_t bh = by - sy;
        const int16_t h = by - ty;

        const int16_t lw = tx - bx;	
        const int16_t rw = sx - tx;	
        const int16_t w = sx - bx;	

	const int16_t lm = lw / h;
	const int16_t tm = -rw / th;
	const int16_t bm = w / bh;

	if (line >= ty && line < sy) {
	    printf("top half\n");
            const int16_t tline = line - ty;
            crit_point->p = tx - tline * lm;
            crit_point++;
            crit_points.num++;
            crit_point->p = tx + tline * tm;
            crit_point++;
            crit_points.num++;
	}

	if (line >= sy && line < by) {
	    printf("bottom half\n");
            const int16_t bline = by - line;	
            crit_point->p = bx + bline * lm;
            crit_point++;
            crit_points.num++;
            crit_point->p = bx + bline * bm;
            crit_point++;
            crit_points.num++;
	}
    }
}

// TODO: faster algorithm needed - this is called on each scan line for 10-20 items
void sort_crit_points(struct crit_points *crit_points_io) {
    struct crit_point tmp;
    int8_t sorted = 0;
    while (!sorted) {
        sorted = 1; // hypothesise that the panels are sorted
        for (int i = 0; i < crit_points_io->num - 1; i++) {
            struct crit_point a = *(crit_points_io->ob + i);
            struct crit_point b = *(crit_points_io->ob + i + 1);
            if (b.p < a.p) {
                tmp = a;
                a = b;
                b = tmp;
                sorted = 0;
                //printf("X");
            } else {
                //printf(".");
            }
        }
        //printf("\n");
    }
}

void crit_points_to_changes() {
    uint32_t tracker = 0x00000000;
    struct change *change = changes.ob;
    changes.num = 0;
    uint8_t last_colour = 0;

    change->start = 0;
    change->colour = 0; // background
    change++;
    changes.num++;

    for (int i = 0; i < crit_points.num; i++) {
        //printf("i == %d\n", i);
        // loop through all crit_points at p
        const struct crit_point *crit_point = crit_points.ob + i;
        const int16_t p = crit_point->p;
        for (; crit_point->p == p && i < crit_points.num; crit_point++, i++) {
            tracker ^= 1 << crit_point->stri_idx; // each panel will start and stop exactly once, and the start will not be right of the stop
            //printf("  i == %d, tracker == %8.8x\n", i, tracker);
        }
        i--; // reverse overshoot
        uint8_t colour = 0;
        for (int b = 0; b < 32; b++) {
            //printf("  b == %d\n", b);
            if (tracker & (1 << b)) {
                colour = (stris.ob + b)->colour;
                //printf("  colour == %d\n", colour);
                break;
            }
            if (b == 31) {
                colour = 0; // background
                //printf("  colour == %d (background)\n", colour);
            }
        }
        if (colour != last_colour) {
            change->start = p;
            change->colour = colour;
            change++;
            changes.num++;
            last_colour = colour;
        }
    }
}

