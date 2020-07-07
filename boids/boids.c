#include "SDL.h"
#include "SDL_image.h"
#include "SDL_timer.h"
#include<stdio.h>
#include<stdint.h>
#include<math.h>

#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)
//pixels per second:
#define SPEED (300)
#define SCROLL_SPEED (300)

int initialise() {
	//Initialize SDL. Throw error and quit if there is a problem
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
		printf("Error initializing SDL: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}
}

int main(int argc, char* argv[]){
	
	if (initialise == 1) { return 1; }

	//Initialize SDL_image. Throw error and quit if there is a problem
	if (IMG_Init(IMG_INIT_JPG) == 0) {
		printf("Error initializing Image library: %s\n", SDL_GetError());
		//IMG_Quit();
		SDL_Quit();
		return 1;
	}


	//Define a window
	SDL_Window* win = SDL_CreateWindow(
		"CUSTOM WINDOW TITLE", // window's title
		10, 25, // coordinates on the screen, in pixels, of the window's upper left corner
		640, 480, // window's length and height in pixels  
		SDL_WINDOW_OPENGL);
	if (!win) { //Test if a window was defined. Throw error and quit if there is a problem
		printf("Error creating the window: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}


	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Renderer* rend = SDL_CreateRenderer(win, -1, render_flags);
	if (!rend) { //Test if a texture was defined. Throw error and quit if there is a problem
		printf("Error creating the window: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}
	//Load the image into memory
	SDL_Surface* surface = IMG_Load("Resources/redcircle.png");
	if (!surface) {
		printf("Error creating the surface: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}

	//Load the image data into the graphics hardware memory
	SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);
	SDL_FreeSurface(surface);
	if (!tex) {
		printf("Error creating the surface: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}

	//Get, and then SCALE, the dimensions of the texture
	SDL_Rect dest;
	SDL_QueryTexture(tex, NULL, NULL, &dest.w, &dest.h);
	dest.w = 5;
	dest.h = 5;

	//Start the sprite in the center of the screen
	float x_pos = (WINDOW_WIDTH - dest.w) / 2;
	float y_pos = (WINDOW_HEIGHT - dest.h) / 2;
	//Give sprite some initial velocity
	float x_vel = SPEED;
	float y_vel = SPEED;

	//Keep track of the inputs that are given
	int up = 0;
	int down = 0;
	int left = 0;
	int right = 0;

	int close_requested = 0;
	


	//animation loop
	while (!close_requested) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_QUIT:
					close_requested = 1;
					break;
				case SDL_KEYDOWN:
					switch (event.key.keysym.scancode) {
						case SDL_SCANCODE_W:
						case SDL_SCANCODE_UP:
							up = 1;
							break;
						case SDL_SCANCODE_S:
						case SDL_SCANCODE_DOWN:
							down = 1;
							break;
						case SDL_SCANCODE_A:
						case SDL_SCANCODE_LEFT:
							left = 1;
							break;
						case SDL_SCANCODE_D:
						case SDL_SCANCODE_RIGHT:
							right = 1;
							break;
					}
					break;
				case SDL_KEYUP:
					switch (event.key.keysym.scancode) {
					case SDL_SCANCODE_W:
					case SDL_SCANCODE_UP:
						up = 0;
						break;
					case SDL_SCANCODE_S:
					case SDL_SCANCODE_DOWN:
						down = 0;
						break;
					case SDL_SCANCODE_A:
					case SDL_SCANCODE_LEFT:
						left = 0;
						break;
					case SDL_SCANCODE_D:
					case SDL_SCANCODE_RIGHT:
						right = 0;
						break;
				}

			}
		}

		//Get cursor position relative to window
		int mouse_x, mouse_y;
		int buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

		//Determine velocity toward mouse
		int target_x = mouse_x - (dest.w / 2);
		int target_y = mouse_y - (dest.h / 2);
		int delta_x = target_x - x_pos;
		int delta_y = target_y - y_pos;
		float distance = sqrt((delta_x*delta_x) + (delta_y*delta_y));

		//prevent jitter
		if (distance < 5) {
			x_vel = y_vel = 0;
		}
		else {
			x_vel = delta_x * SPEED / distance;
			y_vel = delta_y * SPEED / distance;
		}

		x_vel = -x_vel;
		y_vel = -y_vel;
	

		//determine the new velocity.
		//x_vel = y_vel = 0;
		if (up && !down) { y_vel = -SPEED; }
		if (!up && down) { y_vel = SPEED; }
		if (left && !right) { x_vel = -SPEED; }
		if (!left && right) { x_vel = SPEED; }


		//Bounds-collision detection and reflection
		if (x_pos <= 0) {
			x_pos = 0;
			x_vel = SPEED;
		}
		if (y_pos <= 0) {
			y_pos = 0;
			y_vel = SPEED;
		}
		if (x_pos >= WINDOW_WIDTH - dest.w) {
			x_pos = WINDOW_WIDTH - dest.w;
			x_vel = -SPEED;
		}
		if (y_pos >= WINDOW_HEIGHT - dest.h) {
			y_pos = WINDOW_HEIGHT - dest.h;
			y_vel = -SPEED;
		}

		//Update the sprite position
		y_pos += y_vel / 60; //Speed-per-second, divided by frame-time
		x_pos += x_vel / 60;
		//set the positions in the struct
		dest.y = (int)y_pos; //Take note of the cast from float to int, here
		dest.x = (int)x_pos;

		//Clear the window
		SDL_RenderClear(rend);
		//draw the image to the window
		SDL_RenderCopy(rend, tex, NULL, &dest);
		SDL_RenderPresent(rend);

		//Delay the renderer. This does not take into account the time through the animation loop.
		SDL_Delay(1000 / 60);
	}


//SDL_RenderClear(rend);
//SDL_RenderCopy(rend, tex, NULL, NULL);
//SDL_RenderPresent(rend);

//printf();//"Everything went successfully!");


//SDL_Delay(200); // window lasts 3 seconds

//We are done with our resources, lets free them up.
SDL_DestroyTexture(tex);
SDL_DestroyRenderer(rend);
SDL_DestroyWindow(win);

//IMG_Quit();
SDL_Quit();
return 0;
}