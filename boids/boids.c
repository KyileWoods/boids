#include "SDL.h"
#include "SDL_image.h"
#include "SDL_timer.h"
#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<math.h>

#define WINDOW_WIDTH (900)
#define WINDOW_HEIGHT (300)
//pixels per second:
#define SPEED (100)
#define SCROLL_SPEED (300)
#define DEBUG (0)
#define SPEED_CONSTANT (0.8)
#define ALIGN_FORCE (5)
#define BOIDS_COUNT (15)
#define SHOWGREENDOT (1)

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
	float avoid_force = SPEED_CONSTANT;
	float centripetal_force = SPEED_CONSTANT;


	if (initialise() == 1) { return 1; }

	//Define a window
	SDL_Window* win;
	if (CreateWindow(&win) == 1) { return 1; }

	SDL_Renderer* rend;
	if (InitialiseRenderer(&rend, win) == 1) { return 1; }

	//A single image/surface is loaded into memory, and then applied to N-many textures/boids
	//Load the image into memory
	SDL_Surface* redsurface = IMG_Load("Resources/redboid.png");
	if (!redsurface) {
		printf("Error creating the red surface: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}
	SDL_Surface* bluesurface = IMG_Load("Resources/blueboid.png");
	if (!bluesurface) {
		printf("Error creating the blue surface: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}
	SDL_Surface* greensurface = IMG_Load("Resources/greenboid.png");
	if (!bluesurface) {
		printf("Error creating the green surface: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}

	SDL_Rect greendot; //Creating a rect structure for the green dot

	//Load the image data into the graphics hardware memory
	SDL_Texture* redtex = SDL_CreateTextureFromSurface(rend, redsurface);
	SDL_SetTextureBlendMode(redtex, SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(redsurface);
	if (!redtex) {
		printf("Error creating the red texture: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}
	SDL_Texture* bluetex = SDL_CreateTextureFromSurface(rend, bluesurface);
	SDL_SetTextureBlendMode(bluetex, SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(bluesurface);
	if (!bluetex) {
		printf("Error creating the blue texture: %s\n", SDL_GetError());
		IMG_Quit();
		SDL_Quit();
		return 1;
	}
	SDL_Texture* greentex = SDL_CreateTextureFromSurface(rend, greensurface);
	SDL_SetTextureBlendMode(greentex, SDL_BLENDMODE_BLEND);
	SDL_FreeSurface(greensurface);
	if (!greentex) {
		printf("Error creating the green texture: %s\n", SDL_GetError());
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

		/*Create the boids*/
		if (DEBUG) { printf("boid #%d Established ... ", i);  // Debug line
					 printf("at %d |", pdest[i].w); } // Debug line
		if(i==0){
			SDL_QueryTexture(bluetex, NULL, NULL, &pdest[i].w, &pdest[i].h);}
		else {
			SDL_QueryTexture(redtex, NULL, NULL, &pdest[i].w, &pdest[i].h);
		}
		pdest[i].w = 12;
		pdest[i].h = 12; //Absolute scale, in pixels
		if (DEBUG) { printf("texture..."); } // Debug line


		//Sprite position on the screen
		flock[i].x_pos = (WINDOW_WIDTH - pdest[i].w) / ((rand() % 10)+1); //TODO make this able to select any pixel on screen
		flock[i].y_pos = (WINDOW_HEIGHT - pdest[i].h) / ((rand() % 10)+1); //(rand()%10)
		if (DEBUG) { printf("X&Y done, angle: "); }// Debug line
		//Give sprite some initial angle
		flock[i].angle = 2*M_PI*sin(rand());   //radians //Not sure hwy this is not uniformly distributed
		if (DEBUG) { printf("%f\n", flock[i].angle); }// Debug line
	}
	if (DEBUG) { printf("All Boids created successfully!"); }// Debug line
	//Keep track of the inputs that are given
	int up = 0;
	int down = 0;
	int left = 0;
	int right = 0;

	int close_requested = 0; int looper = 0;
	//animation loop
	while (!close_requested) {
		looper += 1;
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
		*/

		// Keyboard Controls
		//x_vel = y_vel = 0;
		if (up && !down) { avoid_force += 0.1; printf("%f\n", avoid_force); }
		if (!up && down) { avoid_force -= 0.1; printf("%f\n", centripetal_force); }
		if (left && !right) { centripetal_force += 0.1; }
		if (!left && right) { centripetal_force -= 0.1; printf("%f\n", centripetal_force); }
		
		
		for (int i = 0; i < BOIDS_COUNT; i++) {
			if(DEBUG){printf("\nBoid #%d ", i); }

			// TODO check that these transformations are not going to break out of [0, 2pi]
			//Bounds-collision detection and reflection
			if (flock[i].x_pos <= 0) {
				flock[i].x_pos = 1;
				if (flock[i].angle >= M_PI) {
					flock[i].angle = flock[i].angle + (M_PI/2);
				}
				else { flock[i].angle = M_PI - flock[i].angle; }
			}
			if (flock[i].x_pos+pdest[i].w >= WINDOW_WIDTH) {
				flock[i].x_pos = WINDOW_WIDTH-1-pdest[i].w;
				if (flock[i].angle <= M_PI) { //Assuming  pi<theta<2*pi or 0<theta<pi
					flock[i].angle = M_PI - flock[i].angle;
				}
				else { flock[i].angle = (3*M_PI/4) - flock[i].angle; }
			}
				
			if (flock[i].y_pos <= 0){
				flock[i].y_pos = 1;
				if (flock[i].angle <= (3 * M_PI / 4)) {
					flock[i].angle = flock[i].angle - (M_PI/2);
				}
				else{flock[i].angle = 2*M_PI - flock[i].angle;}
			}
			if (flock[i].y_pos + pdest[i].h >= WINDOW_HEIGHT) {
				flock[i].y_pos = WINDOW_HEIGHT-1-pdest[i].h;
				if (flock[i].angle <= M_PI/2) {
					flock[i].angle = (2*M_PI) - flock[i].angle;
				}
				else { flock[i].angle = (2 * M_PI) - flock[i].angle; }
			}
			if (DEBUG) { printf("Bounds-checking complete ..."); }//Debug line

			/*------------update the sprite velocity------------*/
			float distance_x = 0, distance_y = 0, distance = 0, phi=0;

			/***Alignment algorithm ****/
			float mean_theta_dev = 0;
			for (int j = 0; j < BOIDS_COUNT; j++) {
				mean_theta_dev = 0;
				if (j != i) {
					// Distance = other_boid - this_boid
					distance_x = flock[j].x_pos - flock[i].x_pos;
					distance_y = flock[j].y_pos - flock[i].y_pos;
					distance = sqrt((distance_x*distance_x) + (distance_y*distance_y));
					mean_theta_dev += (fmod(flock[j].angle,360) - fmod(flock[i].angle,360))*(pow(0.7, pow((distance / 30), 1.3)));

					// Force = K*1/Distance
					// (1 / pow(2, (distance / 50)))
				}
			}
			mean_theta_dev /= BOIDS_COUNT - 1;
			
			int align_rot_direction = 1;
			if (flock[i].angle>=mean_theta_dev){ align_rot_direction = -1; }

			float align_rotation = ALIGN_FORCE * align_rot_direction;

			/***Simply centripetal algorithm ****/
			int boids_in_range = 0; float centripetal_rotation = 0; float x_mean = 0; float y_mean = 0;
			greendot.x = -50; greendot.y = -50; //make it invisible when not needed
			distance = 0;
			for (int j = 0; j < BOIDS_COUNT; j++) {
				if (j != i) {
					// Distance = other_boid - this_boid
					distance_x = flock[j].x_pos - flock[i].x_pos;
					distance_y = flock[j].y_pos - flock[i].y_pos;
					distance = sqrt((distance_x*distance_x) + (distance_y*distance_y));
					if (distance < 200) { x_mean += flock[j].x_pos; y_mean += flock[j].y_pos; boids_in_range++;}
				}
			}
			if (boids_in_range) {
				x_mean /= boids_in_range;
				y_mean /= boids_in_range;
				if (x_mean - flock[i].x_pos) {
					phi = atan((y_mean - flock[i].y_pos) / (x_mean - flock[i].x_pos)); // phi set: (-pi,pi)
				}
				if (phi > 0) {
					phi = fmod(phi, 2 * M_PI);
				}
				else{
					phi = 2 * M_PI - fmod(phi, 2 * M_PI);
				}
				if (flock[i].angle > 0) {
					flock[i].angle = fmod(flock[i].angle, 2 * M_PI);
				}
				else {
					flock[i].angle = 2 * M_PI - fmod(flock[i].angle, 2 * M_PI);
				}
				centripetal_rotation = sin(phi - flock[i].angle)/10;
				
				if (i == 0) { //Blue-boid informations
					//float a = 0.2, t, x = 0, y = 0;
					//t = x * cos(a) - y * sin(a);
					//y = x * sin(a) + y * cos(a);
					//x = t;
					//y=x*sin(a)+y*cos(a)
					greendot.x = x_mean;
					greendot.y = y_mean;
					greendot.w = pdest[0].w;
					greendot.h = pdest[0].h;
					printf("angle to green: %f*pi\n", phi/M_PI);

				}
			}


			/*/***distance weighted centripetal algorithm ****
			float centripetal_rotation, baryocenter_weighted;  float x_centre = 0; float y_centre = 0;
			for (int j = 0; j < BOIDS_COUNT; j++) {
				if (j != i) {
					// Distance = other_boid - this_boid
					distance_x = flock[j].x_pos - flock[i].x_pos;
					distance_y = flock[j].y_pos - flock[i].y_pos;
					distance = sqrt((distance_x*distance_x) + (distance_y*distance_y));
					x_centre += flock[j].x_pos * (pow(0.7, pow((distance / 30), 1.3)));
					y_centre += flock[j].y_pos * (pow(0.7, pow((distance / 30), 1.3)));
					// Force = K*1/Distance
					// (1 / pow(2, (distance / 50)))
				}
			}
			x_centre = x_centre / (BOIDS_COUNT - 1);
			y_centre = y_centre / (BOIDS_COUNT - 1); //DISTANCE-WEIGHTED mean baryocenter
			baryocenter_weighted = sqrt((x_centre*x_centre) + (y_centre*y_centre));
			
			int centripetal_rot_direction = 1;
			if (tan(flock[i].angle) >= y_centre / x_centre) { centripetal_rot_direction = -1; }

			centripetal_rotation = centripetal_force * baryocenter_weighted;
			*/

			//****Seperation Algorithm*********//
			float  seperation_rotation = 0;
			for (int j = 0; j < BOIDS_COUNT; j++) {
				if (j != i) {
					// Distance = other_boid - this_boid
					distance_x = flock[j].x_pos - flock[i].x_pos;
					distance_y = flock[j].y_pos - flock[i].y_pos;
					distance = sqrt((distance_x*distance_x) + (distance_y*distance_y));
					phi = atan(distance_y / distance_x);

					seperation_rotation += cos(phi)/BOIDS_COUNT;
					//seperation_rotation += cos(phi)*(pow(0.5, pow((distance / 50), 10)) + (20 * pow(0.07, pow((distance / 50), 0.2))));

					int seperation_rot_direction = -1; // Sep_rot is opposite centrip_rot defaults, (seeking -V- avoiding)
					if (flock[i].angle >= atan(distance_y / distance_x)) { seperation_rot_direction = 1; }
					

					//TODO: Insert a term to this equation which strengthens the steering response based on the viewing-angle to the other boid, so as to steer more strongly from boids which are directly)
					//seperation_rotation += avoid_force * (pow(0.5, pow((distance / 50), 10)) + (10 * pow(0.07, pow((distance / 50), 0.2))))*seperation_rot_direction;
					/*
					*/
					/* The following equation will give a very strong response when in CLOSE proximity, as documented in "Design Explanation" Word document
					(pow(0.5, pow((distance / 50), 10)) + (10 * pow(0.07, pow((distance / 50), 0.2))))
				      */
					
				}
			}
			

			//****Rotation calculations completed*****//
			if (DEBUG) { printf("flock-forces applied ..."); }//Debug line
			
			//flock[i].angle += seperation_rotation;
			flock[i].angle += centripetal_rotation;
			//flock[i].angle += align_rotation;

			int v_cohesion;
			int v_seperation;
			int v_alignment;

			//Update the sprite position
			flock[i].y_pos += SPEED * (sin(flock[i].angle)) / 60; //Speed-per-second, divided by frame-time
			flock[i].x_pos += SPEED * (cos(flock[i].angle)) / 60;
			//set the positions in the struct
			pdest[i].y = (int)flock[i].y_pos; //Take note this is cast from float to int
			pdest[i].x = (int)flock[i].x_pos;
			if (DEBUG) { printf("Sprite positions updated ..."); } //Debug line

			if(isnan(flock[i].angle)){
				1 + 1;
			}

			//draw the image to the window
			//int facing_angle = 360 / (i+1);
			if(i!=0){
				if (SDL_RenderCopyEx(rend, redtex, NULL, &pdest[i], flock[i].angle * 180 / M_PI, NULL, SDL_FLIP_NONE)) {//Takes angle as degrees only
					printf("RenderCopy RED FAILED FAILED FAILED");
				}
				else {
					if (DEBUG) { printf("RenderCopy RED succeeded"); } //Debug line
				}
			}
			else {
				if (SDL_RenderCopyEx(rend, bluetex, NULL, &pdest[i], flock[i].angle * 180 / M_PI, NULL, SDL_FLIP_NONE)) {//Takes angle as degrees only
					printf("RenderCopy BLUE FAILED FAILED FAILED");
				}
				else {
					if (DEBUG) { printf("RenderCopy BLUE succeeded"); } //Debug line
				}
			}
			if (SHOWGREENDOT) {
				if (SDL_RenderCopyEx(rend, greentex, NULL, &greendot, flock[0].angle * 180 / M_PI, NULL, SDL_FLIP_NONE)) {//Takes angle as degrees only
					printf("RenderCopy BARYGREEN FAILED FAILED FAILED");
				}
				else {
					if (DEBUG) { printf("RenderCopy BARYOGREEN succeeded"); } //Debug line
				}
			}
		}

		if (DEBUG) { ("Attempting RenderPresent..."); }
		SDL_RenderPresent(rend);
		if (DEBUG) { printf("\n-----------------RenderPresent succeeded------------------------"); } //Debug line
		
		//Delay the renderer. This does not take into account the time through the animation loop.
		//SDL_Delay(100/60); //TODO make this framerate consistent, despite loop-computation time.
	}


//SDL_RenderClear(rend);
//SDL_RenderCopy(rend, tex, NULL, NULL);
//SDL_RenderPresent(rend);

//printf();//"Everything went successfully!");


//SDL_Delay(200); // window lasts 3 seconds

//We are done with our resources, lets free them up.
SDL_DestroyTexture(redtex);
SDL_DestroyTexture(bluetex);
SDL_DestroyRenderer(rend);
SDL_DestroyWindow(win);

//IMG_Quit();
SDL_Quit();
return 0;
}