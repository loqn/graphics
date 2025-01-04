#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

#define WIDTH 900
#define HEIGHT 600

#define COLOR_WHITE 0xffffffff
#define COLOR_BLACK 0x00000000
#define COLOR_GRAY 0xefefefef
#define COLOR_RED  SDL_MapRGBA(surface->format, 255, 0, 0, 255)
#define COLOR_BLUE SDL_MapRGBA(surface->format, 0, 0, 255, 255)

#define RAYS_NB 100

struct Circle {
	double x, y;
	double r;
};

struct Ray {
	double x,y;
	double angle;
};

static struct SDL_Rect eraser = {0,0, WIDTH, HEIGHT};

double euclidian_dist(double x1, double y1, double x2, double y2) {
	return sqrt(pow(x1-x2,2) + pow(y1-y2,2));
}

void gen_rays(struct Circle start, struct Ray rays[RAYS_NB]) {
	for (int i=0; i < RAYS_NB; i++) {
		double angle = ((double) i / RAYS_NB) * 2 * M_PI;
		struct Ray ray = {start.x, start.y, angle};
		rays[i] = ray;
	}
}

void draw_rays(SDL_Surface* surface, struct Circle start, struct Ray rays[RAYS_NB], struct Circle obstacle, Uint32 color) {
	
	int step = 1;	
	for (int i = 0; i < RAYS_NB; i++) {
		struct Ray ray = rays[i]; 
		double draw_x = start.x;
		double draw_y = start.y;

		//flags	
		int wall_hit = 0;
		int obstacle_hit = 0;

		while (!wall_hit && !obstacle_hit) {
			draw_x += step*cos(ray.angle);
			draw_y += step*sin(ray.angle);
			
			if (draw_x < 0 || draw_y < 0 || draw_x > WIDTH || draw_y > HEIGHT) {
				wall_hit = 1;
			} else if(euclidian_dist(obstacle.x, obstacle.y, draw_x, draw_y) < obstacle.r) {
				obstacle_hit = 1;
			} else {
				SDL_Rect pixel = {draw_x,draw_y,1,1};
				SDL_FillRect(surface, &pixel, color);
			}
		}
	}
}

void fill_circle(SDL_Surface* surface, struct Circle circle, Uint32 color) {	
	for (int x=circle.x - circle.r; x < circle.r + circle.x; x++) {
		for (int y=circle.y - circle.r; y < circle.r + circle.y; y++) {
			double dist = euclidian_dist(x,y, circle.x, circle.y);
			if (dist < circle.r) {
				SDL_Rect pixel = {x,y,1,1};
				SDL_FillRect(surface, &pixel, color);
			}
		}
	}
}

int main() {
	if (SDL_Init(SDL_INIT_VIDEO) != 0) {
		printf("SDL couldn't initialize: %s\n", SDL_GetError());
		return 1;
	}
	SDL_Window* window = SDL_CreateWindow("Raytracing", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
	
	if (window == NULL) {
		printf("Error creating SDL Window: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
	
	SDL_Surface* surface = SDL_GetWindowSurface(window);
	
	//SDL_FillRect(surface, &rect, COLOR_WHITE);
	
	SDL_Event event;

	struct Circle circle = {200,200,50};
	struct Circle shadow = {600,300,70};
	struct Ray rays[RAYS_NB];
	struct Ray shadows[RAYS_NB];
	gen_rays(circle, rays);
	gen_rays(shadow, shadows);
	int running = 1; 
	while(running) {
		
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				running = 0;
			}

			if (event.type == SDL_MOUSEMOTION && event.motion.state != 0) {
				circle.x = event.motion.x;
				circle.y = event.motion.y;	
			}
		}
		SDL_FillRect(surface, &eraser, COLOR_BLACK);		
		draw_rays(surface, circle, rays, shadow, COLOR_RED);
		fill_circle(surface, circle, COLOR_WHITE);
		draw_rays(surface, shadow, shadows, circle, COLOR_BLUE);
		fill_circle(surface, shadow, COLOR_WHITE);
		SDL_UpdateWindowSurface(window);
		SDL_Delay(10);
	}
}
