#include <SDL3/SDL.h>

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

	float player_size = 40;
	SDL_FRect player_rect;
	player_rect.w = player_size;
	player_rect.h = player_size;
	player_rect.x = window_w / 2 - player_size / 2;
	player_rect.y = window_h / 2 - player_size / 2;


	bool btn_pressed_up = false;

	SDL_GetCurrentTime(&walltime_frame_beg);
	while(!quit)
	{
		// input
		SDL_Event event;
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
					break;
			}
		}

		// clear screen
		// NOTE: `0x` prefix means we are expressing the number in hexadecimal (base 16)
		//       `0b` is another useful prefix, expresses the number in binary
		SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(renderer);
		
		SDL_SetRenderDrawColor(renderer, 0x3C, 0x63, 0xFF, 0XFF);
		SDL_RenderFillRect(renderer, &player_rect);

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