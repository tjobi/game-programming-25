#include <SDL3/SDL.h>

enum {
	UP 		= 0,
	DOWN 	= 1,
	LEFT 	= 2,
	RIGHT 	= 3
};

int main(void)
{
	SDL_Log("hello sdl");
	
	float window_w = 800;
	float window_h = 600;
	int target_framerate_ms = 1000 / 60;       // 16 milliseconds
	int target_framerate_ns = 1000000000 / 60; // 16666666 nanoseconds

	SDL_Window* window = SDL_CreateWindow("E00 - introduction", window_w, window_h, 0);
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

	// increase the zoom to make debug text more legible
	// (ie, on the class projector, we will usually use 2)
	{
		float zoom = 1;
		window_w /= zoom;
		window_h /= zoom;
		SDL_SetRenderScale(renderer, zoom, zoom);
	}

	bool quit = false;

	SDL_Time walltime_frame_beg;
	SDL_Time walltime_work_end;
	SDL_Time walltime_frame_end = 0;
	SDL_Time time_elapsed_frame;
	SDL_Time time_elapsed_work;

	int delay_type = 0;

	const float MOVE_SPEED = 5;
	const float player_size = 40;

	SDL_FRect player_rect1;
	player_rect1.w = player_size;
	player_rect1.h = player_size;
	player_rect1.x = window_w / 2 - player_size / 2;
	player_rect1.y = window_h / 2 - player_size / 2;

	SDL_FRect player_rect2;
	player_rect2.w = player_size;
	player_rect2.h = player_size;
	player_rect2.x = window_w / 3 - player_size / 2;
	player_rect2.y = window_h / 2 - player_size / 2;

	bool btns_player1[4];
	bool btns_player2[4];
	
	SDL_GetCurrentTime(&walltime_frame_beg);
	while(!quit)
	{
		// input
		SDL_Event event;
		//NOTE: may want to limit the amount of events that we process - to avoid stuttering
		while(SDL_PollEvent(&event))
		{
			switch(event.type)
			{
				case SDL_EVENT_QUIT:
					quit = true;
					break;
				case SDL_EVENT_KEY_DOWN:
					if(event.key.key >= SDLK_0 && event.key.key < SDLK_5)
						delay_type = event.key.key - SDLK_0;
					switch(event.key.key)
					{
						case SDLK_W: btns_player1[UP] 	 = true; break;
						case SDLK_S: btns_player1[DOWN]  = true; break;
						case SDLK_A: btns_player1[LEFT]  = true; break;
						case SDLK_D: btns_player1[RIGHT] = true; break;
						case SDLK_UP: btns_player2[UP] 	 	 = true; break;
						case SDLK_DOWN: btns_player2[DOWN]   = true; break;
						case SDLK_LEFT: btns_player2[LEFT]   = true; break;
						case SDLK_RIGHT: btns_player2[RIGHT] = true; break;
					}
					break;
				case SDL_EVENT_KEY_UP:
					switch(event.key.key)
					{
						case SDLK_W: btns_player1[UP] 	 = false; break;
						case SDLK_S: btns_player1[DOWN]  = false; break;
						case SDLK_A: btns_player1[LEFT]  = false; break;
						case SDLK_D: btns_player1[RIGHT] = false; break;
						case SDLK_UP: btns_player2[UP] 	 = false; break;
						case SDLK_DOWN: btns_player2[DOWN]  = false; break;
						case SDLK_LEFT: btns_player2[LEFT]  = false; break;
						case SDLK_RIGHT: btns_player2[RIGHT] = false; break;
					}
					break;
				case SDL_EVENT_MOUSE_BUTTON_DOWN:
					player_rect1.x = event.button.x - player_size/2;
					player_rect1.y = event.button.y - player_size/2;
					break;
			}
		}
		//update
		{
			//------ PLAYER 1 ------
			if(btns_player1[UP] && 0 <= player_rect1.y) 
				player_rect1.y = SDL_max(0, player_rect1.y - 1 * MOVE_SPEED);

			if(btns_player1[DOWN] && window_h-player_size >= player_rect1.y) 	
				player_rect1.y = SDL_min(window_h-player_size, player_rect1.y + 1*MOVE_SPEED);
			
			if(btns_player1[LEFT] && 0 <= player_rect1.x) 
				player_rect1.x = SDL_max(0, player_rect1.x - 1*MOVE_SPEED);
			
			if(btns_player1[RIGHT] && window_w - player_size >= player_rect1.x) 
				player_rect1.x = SDL_min(window_w-player_size, player_rect1.x + 1*MOVE_SPEED);

			//------ PLAYER 2 --------
			if(btns_player2[UP] && 0 <= player_rect2.y) 
				player_rect2.y = SDL_max(0, player_rect2.y - 1 * MOVE_SPEED);

			if(btns_player2[DOWN] && window_h-player_size >= player_rect2.y) 	
				player_rect2.y = SDL_min(window_h-player_size, player_rect2.y + 1*MOVE_SPEED);
			
			if(btns_player2[LEFT] && 0 <= player_rect2.x) 
				player_rect2.x = SDL_max(0, player_rect2.x - 1*MOVE_SPEED);
			
			if(btns_player2[RIGHT] && window_w - player_size >= player_rect2.x) 
				player_rect2.x = SDL_min(window_w-player_size, player_rect2.x + 1*MOVE_SPEED);
		}

		// clear screen
		// NOTE: `0x` prefix means we are expressing the number in hexadecimal (base 16)
		//       `0b` is another useful prefix, expresses the number in binary
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(renderer);
		
		SDL_SetRenderDrawColor(renderer, 0x3C, 0x63, 0xFF, 0XFF);
		SDL_RenderFillRect(renderer, &player_rect1);

		SDL_SetRenderDrawColor(renderer, 0x3C, 0xFF, 0x63, 0XFF);
		SDL_RenderFillRect(renderer, &player_rect2);

		if(SDL_HasRectIntersectionFloat(&player_rect1, &player_rect2)){
			SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x00, 0xFF);
			SDL_RenderDebugText(renderer, window_w/2, window_h/2, "ouch");
		}

		SDL_GetCurrentTime(&walltime_work_end);
		time_elapsed_work = walltime_work_end - walltime_frame_beg;

		if(target_framerate_ns > time_elapsed_work)
		{
			switch(delay_type)
			{
				case 0:
				{
					// busy wait - very precise, but costly
					SDL_Time walltime_busywait = walltime_work_end;
					while(walltime_busywait - walltime_frame_beg < target_framerate_ns)
						SDL_GetCurrentTime(&walltime_busywait);
					break;
				}
				case 1:
				{
					// simple delay - too imprecise
					// NOTE: `SDL_Delay` gets milliseconds, but our timer gives us nanoseconds! We need to covert it manually
					SDL_Delay((target_framerate_ns - time_elapsed_work) / 1000000);
					break;
				}
				case 2:
				{
					// delay ns - also too imprecise
					SDL_DelayNS(target_framerate_ns - time_elapsed_work);
					break;
				}
				case 3:
				{
					// delay precise
					SDL_DelayPrecise(target_framerate_ns - time_elapsed_work);
					break;
				}
				case 4:
				{
					// custom delay - we use the sleeping delay with an arbitrary margin, then we busywait what's left
					SDL_DelayNS(target_framerate_ns - time_elapsed_work - 1000000);
					SDL_Time walltime_busywait = walltime_work_end;

					while(walltime_busywait - walltime_frame_beg < target_framerate_ns)
						SDL_GetCurrentTime(&walltime_busywait);
					break;
				}
			}
		}

		SDL_GetCurrentTime(&walltime_frame_end);
		time_elapsed_frame = walltime_frame_end - walltime_frame_beg;

		SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderDebugTextFormat(renderer, 10.0f, 10.0f, "elapsed (frame): %9.6f ms", (float)time_elapsed_frame/(float)1000000);
		SDL_RenderDebugTextFormat(renderer, 10.0f, 20.0f, "elapsed(work   : %9.6f ms", (float)time_elapsed_work/(float)1000000);
		SDL_RenderDebugTextFormat(renderer, 10.0f, 30.0f, "delay type: %d (change with 0-4)", delay_type);


		// render
		SDL_RenderPresent(renderer);
		
		walltime_frame_beg = walltime_frame_end;
	}

	return 0;
};