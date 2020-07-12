#include "SDL.h"
#include "SDL_image.h"
#include "SDL_timer.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>

#define WINDOW_WIDTH (1200)
#define WINDOW_HEIGHT (600)
//pixels per second:
#define SPEED (100)
#define SCROLL_SPEED (300)
#define DEBUG (0)
#define FORCE_CONSTANT (300)
#define BOIDS_COUNT (60)

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
		"BOIDS", // window's title
		10, 25, // coordinates on the screen, in pixels, of the window's upper left corner
		WINDOW_WIDTH, WINDOW_HEIGHT, // window's length and height in pixels  
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
	if (!PointerToRenderer) { //Test if a renderer was defined. Throw error and quit if there is a problem
		printf("Error creating the renderer: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}
	return 0;
}

struct Boid { int ID; float x_pos; float y_pos; double angle; };

int main(int argc, char* argv[]) {

	if (initialise() == 1) { return 1; }

	//Define a window
	SDL_Window* win;
	if (CreateWindow(&win) == 1) { return 1; }

	SDL_Renderer* rend;
	if (InitialiseRenderer(&rend, win) == 1) { return 1; }

	//A single image/surface is loaded into memory, and then applied to N-many textures/boids
	//Load the image into memory
	SDL_Surface* surface = IMG_Load("Resources/redboid.png");
	if (!surface) {
		printf("Error creating the surface: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}



	//Load the image data into the graphics hardware memory
	SDL_Texture* tex = SDL_CreateTextureFromSurface(rend, surface);
	SDL_SetTextureBlendMode(tex, SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(surface);
	if (!tex) {
		printf("Error creating the surface: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}

	//Create an array of textures representing each boid
	SDL_Rect* pdest = (SDL_Rect*)malloc(BOIDS_COUNT * sizeof(SDL_Rect));
	struct Boid* flock = (struct Boid*)malloc(BOIDS_COUNT * sizeof(struct Boid)); //Creating space for a flock of boids
	//ptr = (int*)malloc(100 * sizeof(int)); //Prototype of the malloc function
	printf("position and velocity structs established!\n");

	/**** BOIDS CREATION ITERATION ****/
	for (int i = 0; i < BOIDS_COUNT; i++) { 

		if (DEBUG) { printf("boid #%d Established ... ", i);  // Debug line
					 printf("at %d |", pdest[i].w); } // Debug line
		SDL_QueryTexture(tex, NULL, NULL, &pdest[i].w, &pdest[i].h);
		pdest[i].w = 12;
		pdest[i].h = 12; //Absolute scale, in pixels
		if (DEBUG) { printf("texture..."); } // Debug line



	/*
		SDL_Rect dest2; //Second dest, as a prototype for extending this to be multiple boids
		SDL_QueryTexture(tex, NULL, NULL, &dest2.w, &dest2.h);
		dest2.w = 5;
		dest2.h = 5; //Absolute scale, in pixels
	*/
		//Sprite position on the screen
		flock[i].x_pos = (WINDOW_WIDTH - pdest[i].w) / ((rand() % 10)+1); //TODO make this able to select any pixel on screen
		flock[i].y_pos = (WINDOW_HEIGHT - pdest[i].h) / ((rand() % 10)+1); //(rand()%10)
		if (DEBUG) { printf("X&Y done, angle: "); }// Debug line
		//Give sprite some initial angle
		flock[i].angle = (rand() % 3600)/10;   //Degrees
		if (DEBUG) { printf("%f\n", flock[i].angle); }// Debug line
	}
	if (DEBUG) { printf("All Boids created successfully!"); }// Debug line
	//Keep track of the inputs that are given
	int up = 0;
	int down = 0;
	int left = 0;
	int right = 0;

	int close_requested = 0;
	//animation loop
	while (!close_requested) {
		if (DEBUG) { printf("\nMain loop:   "); }

		//Clear the window in the buffer
		SDL_RenderClear(rend);

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
		for (int i = 0; i < BOIDS_COUNT; i++) {
			if(DEBUG){printf("\nBoid #%d ", i); }

			flock[i].angle = fmod(flock[i].angle, 360); //This fmod reassignment is used to stop angles winding up arbitrarily large
			//Bounds-collision detection and reflection
			if (flock[i].x_pos <= 0) {
				flock[i].x_pos = 1;
				if (flock[i].angle >= 180) {
					flock[i].angle = flock[i].angle + 90;
				}
				else { flock[i].angle = 180 - flock[i].angle; }
			}
			if (flock[i].x_pos >= WINDOW_WIDTH) {
				flock[i].x_pos = WINDOW_WIDTH-1;
				if (flock[i].angle <= 180) { //Assuming  180<theta<360 or 0<theta<180
					flock[i].angle = 180 - flock[i].angle;
				}
				else { flock[i].angle = 270 - flock[i].angle; }
			}
				
			if (flock[i].y_pos <= 0){
				flock[i].y_pos = 1;
				if (flock[i].angle <= 270) {
					flock[i].angle = flock[i].angle - 90;
				}
				else{flock[i].angle = 360 - flock[i].angle;}
			}
			if (flock[i].y_pos >= WINDOW_HEIGHT) {
				flock[i].y_pos = WINDOW_HEIGHT-1;
				if (flock[i].angle <= 90) {
					flock[i].angle = 360 - flock[i].angle;
				}
				else { flock[i].angle = 360 - flock[i].angle; }
				//270-(90-theta)
			}
			if (DEBUG) { printf("Bounds-checking complete ..."); }//Debug line

			/*------------update the sprite velocity------------*/

			//****Cohesion algorithm (Not yet implemented)
			int x_centre = 0;
			int y_centre = 0;
			for (int j = 0; j < BOIDS_COUNT; j++) {
				if (j != i) {
					x_centre = x_centre + flock[j].x_pos;
					y_centre = y_centre + flock[j].y_pos;
				}
			}
			x_centre = x_centre / (BOIDS_COUNT-1);
			y_centre = y_centre / (BOIDS_COUNT-1); //Average baryocenter
			if (DEBUG) { printf("flock-centre found..."); } //Debug line
			int centripetal_force = 10; //TODO this should be made into a global variable (OR boid attribute), probably.

			int centripetal_rot_direction = 1;
			if (tan(flock[i].angle) >= y_centre / x_centre) { centripetal_rot_direction = -1; }

			// TODO Centripetal force is not applied yet.

			//****Seperation Algorithm
			float distance_x, distance_y, distance;
			float  seperation_rotation = 0;
			for (int j = 0; j < BOIDS_COUNT; j++) {
				if (j != i) {
					// Distance = other_boid - this_boid
					distance_x = flock[j].x_pos - flock[i].x_pos;
					distance_y = flock[j].y_pos - flock[i].y_pos;
					distance = sqrt((distance_x*distance_x) + (distance_y*distance_y));
					
					int seperation_rot_direction = 1;
					if (tan(flock[i].angle) >= distance_y / distance_x) { seperation_rot_direction = -1; }
					
					// Force = K*1/Distance
					seperation_rotation = (FORCE_CONSTANT * 1 / distance)*seperation_rot_direction;
				}
			}
			if (DEBUG) { printf("flock-forces applied ..."); }//Debug line

			flock[i].angle += seperation_rotation;

			int v_cohesion;
			int v_seperation;
			int v_alignment;

			//Update the sprite position
			flock[i].y_pos += SPEED*(sin(M_PI*flock[i].angle/180)) / 60; //Speed-per-second, divided by frame-time
			flock[i].x_pos += SPEED * (cos(M_PI*flock[i].angle/180)) / 60;
			//set the positions in the struct
			pdest[i].y = (int)flock[i].y_pos; //Take note this is cast from float to int
			pdest[i].x = (int)flock[i].x_pos;
			if (DEBUG) { printf("Sprite positions updated ..."); } //Debug line

			//draw the image to the window
			//int facing_angle = 360 / (i+1);
			SDL_RenderCopyEx(rend, tex, NULL, &pdest[i], flock[i].angle, NULL, SDL_FLIP_NONE);
			if (DEBUG) { printf("RenderCopy succeeded"); } //Debug line
		}

		if (DEBUG) { ("Attempting RenderPresent..."); }
		SDL_RenderPresent(rend);
		if (DEBUG) { printf("\n-----------------RenderPresent succeeded------------------------"); } //Debug line
		
		//Delay the renderer. This does not take into account the time through the animation loop.
		SDL_Delay(100/60); //TODO make this framerate consistent, despite loop-cmputation time.
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