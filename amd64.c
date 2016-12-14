/*
 * This is the code which is compiled to run on a desktop machine.
 */

#include <stdio.h>
#include <stdint.h>
#include <SDL2/SDL.h>

#include "amd64.h"

struct stris stris = {MAX_WALLS, {
    {160,120, 480,240, 160,360, 1, 3}, 
    {140,130, 470,250, 150,370, 2, 5}, 
    {120,140, 460,260, 140,380, 3, 7} 
}};
struct crit_points crit_points;
struct changes changes;

void draw_scanline(SDL_Renderer *renderer, uint16_t line) {
    uint16_t p = 0;
    for (int i = 0; i < changes.num; i++) {
        const struct change change = *(changes.ob + i);
	printf("%d, %d\n", change.start, change.colour);
        SDL_SetRenderDrawColor(renderer, 255 * !!(change.colour & 0b100), 255 * !!(change.colour & 0b010), 255 * !!(change.colour & 0b001), 255);
        SDL_RenderDrawLine(renderer, change.start, line, SCREEN_WIDTH - 1, line);
    }
}


void draw_frame(SDL_Renderer *renderer) {
    sort_stris();
    for (uint16_t line = 0; line < SCREEN_HEIGHT; line += 1) {
	printf("line %d\n", line);
	stris_to_crit_points(line);
        sort_crit_points();
        //printf("crit_points->num == %d\n", crit_points->num);
        crit_points_to_changes();
        //printf("changes->num == %d\n", changes->num);
        draw_scanline(renderer, line);
    }

    SDL_RenderPresent(renderer);
}

int main() {
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_RenderSetLogicalSize(renderer, SCREEN_WIDTH, SCREEN_HEIGHT);

    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) return -1;
    window = SDL_CreateWindow( "Server", 100, 100, SCREEN_WIDTH, SCREEN_HEIGHT, 0 );
    if (window == NULL) return -2;
    renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );
    if (renderer == NULL) return -3;

    for (int ang = 0; ang < 256; ang += 16) {
        printf("ang: %d, sine: %d, cos: %d\n", ang, mulsine(1000, ang), mulcos(1000, ang));
    }

    int loop = 1;
    while(loop) {
        SDL_Delay(16);
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                loop = 0;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_RIGHT:
                        printf("right\n");
                        break;
                    case SDLK_LEFT:
                        printf("left\n");
                        break;
                    case SDLK_DOWN:
                        printf("down\n");
                        break;
                    case SDLK_UP:
                        printf("up\n");
                    default :
                        break;
                }
            }
            printf("\n\n");
            draw_frame(renderer);
        }
    }

    return 0;
}
