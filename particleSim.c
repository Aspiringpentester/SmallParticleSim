#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <stdbool.h>


typedef struct{
	double xPos;
	double yPos;
	double xVelocity;
	double yVelocity;
	double xAcceleration;
	double yAcceleration;
	double mass;
	double radius;
} Circle;


typedef struct{
	int numParticles;
	Circle particles[2000];
} Simulation;

bool is_touching(Circle* particle1, Circle* particle2){
	double dx = particle2->xPos - particle1->xPos;
	double dy = particle2->yPos - particle1->yPos;
	double radical = dx * dx + dy * dy;
	return sqrt(radical) < particle1->radius + particle2->radius;
}


void draw_circle(SDL_Renderer* renderer, int centerX, int centerY, int radius) {
	SDL_Point points[radius * 8 * 35 / 49];
	int x = radius;
    	int y = 0;
    	int radiusError = 1 - x;

    	while (x >= y) {
        	points[0] = (SDL_Point){centerX + x, centerY + y};
	        points[1] = (SDL_Point){centerX + y, centerY + x};
	        points[2] = (SDL_Point){centerX - y, centerY + x};
	        points[3] = (SDL_Point){centerX - x, centerY + y};
	        points[4] = (SDL_Point){centerX - x, centerY - y};
	        points[5] = (SDL_Point){centerX - y, centerY - x};
	        points[6] = (SDL_Point){centerX + y, centerY - x};
	        points[7] = (SDL_Point){centerX + x, centerY - y};

	       	y++;
        	if (radiusError < 0) {
	            radiusError += 2 * y + 1;
	        } else {
	            x--;
	            radiusError += 2 * (y - x + 1);
		}
	}
	SDL_RenderDrawPoints(renderer, points, 8);
}

void init_particles(Simulation* sim){
	for(int i = 0; i < sim->numParticles; i++){
		sim->particles[i].xPos = rand() % 640;
		sim->particles[i].yPos = rand() % 480;
		sim->particles[i].xVelocity = 8.0 * (((double)rand() / RAND_MAX) - 0.5);
		sim->particles[i].yVelocity = 8.0 * (((double)rand() / RAND_MAX) - 0.5);
		sim->particles[i].xAcceleration = 0;
		sim->particles[i].yAcceleration = 0;
		sim->particles[i].mass = 1;
		sim->particles[i].radius = 2;
	}
}

void render_particles(Simulation* sim, SDL_Renderer* renderer, SDL_Window* window){
	int windowWidth;
	int windowHeight;
	SDL_GL_GetDrawableSize(window, &windowWidth, &windowHeight);

	//Update particle values
	for(int i = 0; i < sim->numParticles; i++){
		Circle* particle = &sim->particles[i];
		particle->xPos += particle->xVelocity;
		particle->yPos += particle->yVelocity;
		//particle->xVelocity += particle->xAcceleration;
		//particle->yVelocity += particle->yAcceleration;

		if(particle->xPos > windowWidth || particle->xPos < 0){
			particle->xVelocity = -particle->xVelocity;
		}
		if(particle->yPos > windowHeight || particle->yPos < 0){
			particle->yVelocity = -particle->yVelocity;
		}

		for(int j = 0; j < sim->numParticles; j++){
			if(j != i){
				Circle* tempParticle = &sim->particles[j];
				if(is_touching(tempParticle, particle)){
					double m1 = particle->mass;
					double m2 = tempParticle->mass;
					double v1X = particle->xVelocity;
					double v1Y = particle->yVelocity;
					double v2X = tempParticle->xVelocity;
					double v2Y = tempParticle->yVelocity;

					double v1XFinal = ((m1 - m2) * v1X + 2 * m2 * v2X) / (m1 + m2);
					double v1YFinal = ((m1 - m2) * v1Y + 2 * m2 * v2Y) / (m1 + m2);

					double v2XFinal = ((m2 - m1) * v2X + 2 * m1 * v1X) / (m1 + m2);
					double v2YFinal = ((m2 - m1) * v2Y + 2 * m1 * v1Y) / (m1 + m2);

					particle->xVelocity = v1XFinal;
					particle->yVelocity = v1YFinal;

					tempParticle->xVelocity = v2XFinal;
					tempParticle->yVelocity = v2YFinal;
				}
				//sim->particles[j] = tempParticle;
			}
		}
		//sim->particles[i] = particle;
	}


	//Draw particles
	for(int i = 0; i < sim->numParticles; i++){
		draw_circle(renderer, sim->particles[i].xPos, sim->particles[i].yPos, sim->particles[i].radius);
	}

}


int main(){
	srand(time(NULL));

	if(SDL_Init(SDL_INIT_VIDEO) < 0){
		printf("SDL could not initialize");
		return 1;
	}

	SDL_Window* window = SDL_CreateWindow("Testing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_SHOWN);
	if(!window){
		printf("Window failed to create");
		SDL_Quit();
		return 1;
	}

	SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if(!renderer){
		SDL_DestroyWindow(window);
		SDL_Quit();
		return 1;
	}

	bool running = true;
	SDL_Event event;

	Simulation sim;
	sim.numParticles = 750;
	init_particles(&sim);

	clock_t lastTime = clock();
	clock_t fpsTime = clock();
	int frameCount = 0;
	int fps = 0;

	while(running){
		while(SDL_PollEvent(&event)){
			if(event.type == SDL_QUIT){
				running = false;
			}
		}

		clock_t currentTime = clock();
		double deltaTime = (double)(currentTime - lastTime) / CLOCKS_PER_SEC;
		lastTime = currentTime;

		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
		SDL_RenderClear(renderer);

		SDL_SetRenderDrawColor(renderer, 100, 200, 200, 255);
		render_particles(&sim, renderer, window);
		SDL_RenderPresent(renderer);

		frameCount++;
		double elapsed = (double)(currentTime - fpsTime) / CLOCKS_PER_SEC;
		if (elapsed >= 1.0) {
			fps = frameCount;
			frameCount = 0;
			fpsTime = currentTime;
			printf("FPS:%d\n", fps);
		}
	}


	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();



	return 0;
}
