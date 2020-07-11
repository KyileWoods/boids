#include "SDL.h"
#include "SDL_image.h"
#include "SDL_timer.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>

#define WINDOW_WIDTH (640)
#define WINDOW_HEIGHT (480)
//pixels per second:
#define SPEED (300)
#define SCROLL_SPEED (300)

int initialise() {
	printf("INITIALIZATION ENTERED\n");
	//Initialize SDL. Throw error and quit if there is a problem
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {									 //SDL_Init() throws 0 on success
		printf("Error initializing SDL: %s\n", SDL_GetError());
		SDL_Quit();
		return 1;
	}

	//Initialize SDL_image. Throw error and quit if there is a problem
	if (IMG_Init(IMG_INIT_JPG) == 0) {														//IMG_INIT() throws 0 on ERROR
		printf("Error initializing Image library: %s\n", SDL_GetError());
		//IMG_Quit();
		SDL_Quit();
		return 1;
	}
	return 0;
}

int CreateWindow(SDL_Window** WhereWindow){ // Initiates the main game screen. Returns a 1 on failure
	printf("Entered window creation\n");
	SDL_Window* PointerToWindow = SDL_CreateWindow(
		"CUSTOM WINDOW TITLE", // window's title
		10, 25, // coordinates on the screen, in pixels, of the window's upper left corner
		640, 480, // window's length and height in pixels  
		SDL_WINDOW_OPENGL);
	*WhereWindow = PointerToWindow;

	if (!PointerToWindow) { //Test if a window was defined. Throw error and quit if there is a problem
		printf("Error creating the window: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}
	return 0;
}

int InitialiseRenderer(SDL_Renderer** WhereRend, SDL_Window* win) { // Initiates the main game screen. Returns a 1 on failure
	printf("Entered Renderer initialisation\n");
	Uint32 render_flags = SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC;
	SDL_Window* PointerToRenderer = SDL_CreateRenderer(win, -1, render_flags);
	*WhereRend = PointerToRenderer;
	if (!PointerToRenderer) { //Test if a texture was defined. Throw error and quit if there is a problem
		printf("Error creating the renderer: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}
	return 0;
}

int main(int argc, char* argv[]) {

	if (initialise() == 1) { return 1; }


	//Define a window
	SDL_Window* win;
	if (CreateWindow(&win) == 1) { return 1; }

	SDL_Renderer* rend;
	if (InitialiseRenderer(&rend, win) == 1) { return 1; }

	//A single image/surface is loaded into memory, and then applied to N-many textures/boids
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

	int boids_count = 5 ;
	//Get, and then SCALE, the dimensions of the texture
	//SDL_Rect dest[boids_count];
	SDL_Rect* pdest = (SDL_Rect*)malloc(boids_count * sizeof(SDL_Rect));
	//ptr = (int*)malloc(100 * sizeof(int)); //Prototype
	float* x_pos = (float*)malloc(boids_count * sizeof(float));
	float* y_pos = (float*)malloc(boids_count * sizeof(float));
	float* x_vel = (float*)malloc(boids_count * sizeof(float));
	float* y_vel = (float*)malloc(boids_count * sizeof(float));
	printf("position and velocity established!\n");

	for (int i = 0; i <= boids_count; i++) {
		printf("%d Established: ... ", i); // Debug line
		printf("%d", pdest[i].w); // Debug line
		SDL_QueryTexture(tex, NULL, NULL, &pdest[i].w, &pdest[i].h);
		pdest[i].w = 5;
		pdest[i].h = 5; //Absolute scale, in pixels
		printf("texture..."); // Debug line



	/*
		SDL_Rect dest2; //Second dest, as a prototype for extending this to be multiple boids
		SDL_QueryTexture(tex, NULL, NULL, &dest2.w, &dest2.h);
		dest2.w = 5;
		dest2.h = 5; //Absolute scale, in pixels
	*/
		//Start the sprite in the center of the screen
		x_pos[i] = (WINDOW_WIDTH - pdest[i].w) / (rand()%10);
		y_pos[i] = (WINDOW_HEIGHT - pdest[i].h) / (rand()%10);
		//Give sprite some initial velocity
		x_vel[i] = 0.1*SPEED*(rand()%10);
		y_vel[i] = 0.1*SPEED * (rand() % 10);
		printf("Position and velocity\n"); // Debug line
	}
	printf("Boids created successfully!"); // Debug line
	//Keep track of the inputs that are given
	int up = 0;
	int down = 0;
	int left = 0;
	int right = 0;

	int close_requested = 0;
	


	//animation loop
	while (!close_requested) {
		printf("Loop Entere!\n");
		SDL_Event event;
		while (SDL_PollEvent(&event)) //Keyboard input capturing
		{
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
		} //Keyboard input information

		/*Get cursor position relative to window
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
		

		//determine the new velocity.
		//x_vel = y_vel = 0;
		if (up && !down) { y_vel = -SPEED; }
		if (!up && down) { y_vel = SPEED; }
		if (left && !right) { x_vel = -SPEED; }
		if (!left && right) { x_vel = SPEED; }*/
		for (int i = 0; i <= boids_count; i++) {
			//Bounds-collision detection and reflection
			if (x_pos[i] <= 0) {
				x_pos[i] = 0;
				x_vel[i] = SPEED;
			}
			if (y_pos[i] <= 0) {
				y_pos[i] = 0;
				y_vel[i] = SPEED;
			}
			if (x_pos[i] >= WINDOW_WIDTH - pdest[i].w) {
				x_pos[i] = WINDOW_WIDTH - pdest[i].w;
				x_vel[i] = -SPEED;
			}
			if (y_pos[i] >= WINDOW_HEIGHT - pdest[i].h) {
				y_pos[i] = WINDOW_HEIGHT - pdest[i].h;
				y_vel[i] = -SPEED;
			}

			//Update the sprite position
			y_pos[i] += y_vel[i] / 60; //Speed-per-second, divided by frame-time
			x_pos[i] += x_vel[i] / 60;
			//set the positions in the struct
			pdest[i].y = (int)y_pos[i]; //Take note of the cast from float to int, here
			pdest[i].x = (int)x_pos[i];


			//Clear the window
			SDL_RenderClear(rend);
			//draw the image to the window
			SDL_RenderCopy(rend, tex, NULL, &pdest[i]);
			SDL_RenderPresent(rend);
		}
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