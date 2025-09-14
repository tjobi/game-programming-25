#define STB_IMAGE_IMPLEMENTATION
#define ITU_UNITY_BUILD
#include <SDL3/SDL.h>
#include <itu_lib_engine.hpp>
#include <itu_sys_sprite_renderer.hpp>
#include <itu_lib_render.hpp>

#define MEMORY_GAME_SIZE MB(512)
#define TARGET_FRAMERATE SECONDS(1) / 60
#define WINDOW_W         800
#define WINDOW_H         600

#define ENTITY_COUNT 4096

struct Entity
{
	Sprite sprite;
	
	vec2f position;
	vec2f size;
};

struct GameState
{
	// shotcut references
	Entity* player;

	// game-allocated memory
	Entity* entities;
	int entities_alive_count;

	// SDL-allocated structures
	SDL_Texture* atlas;

	// timing info
	float delta;
	float elapsed;
};

static Entity* entity_create(GameState* state)
{
	if(!(state->entities_alive_count < ENTITY_COUNT))
		// NOTE: this might as well be an assert, if we don't have a way to recover/handle it
		return NULL;

	// // concise version
	//return &state->entities[state->entities_alive_count++];

	Entity* ret = &state->entities[state->entities_alive_count];
	++state->entities_alive_count;
	return ret;
}

// NOTE: this only works if nobody holds references to other entities!
//       if that were the case, we couldn't swap them around.
//       We will see in later lectures how to handle this kind of problems
static void entity_destroy(GameState* state, Entity* entity)
{
	// NOTE: here we want to fail hard, nobody should pass us a pointer not gotten from `entity_create()`
	SDL_assert(entity < state->entities ||entity > state->entities + ENTITY_COUNT);

	--state->entities_alive_count;
	*entity = state->entities[state->entities_alive_count];
}

static void game_init(SDLContext* context, GameState* state)
{
	// allocate memory
	state->entities = (Entity*)SDL_calloc(ENTITY_COUNT, sizeof(Entity));
	SDL_assert(state->entities);

	// texture atlases
	state->atlas = texture_create(context, "data/kenney/simpleSpace_tilesheet_2.png", SDL_SCALEMODE_LINEAR);

}

static void game_reset(SDLContext* context, GameState* state)
{
	state->entities_alive_count = 0;

	// entities
	state->player = entity_create(state);
	itu_lib_sprite_init(
		&state->player->sprite,
		state->atlas,
		itu_lib_sprite_get_rect(0, 1, 128, 128)
	);
	state->player->sprite.pivot.x = 1;
}

static void game_update(SDLContext* context, GameState* state)
{
	vec2f mov = { 0 };
	if(context->btn_isdown_up)
		mov.y -= 1;
	if(context->btn_isdown_down)
		mov.y += 1;
	if(context->btn_isdown_left)
		mov.x -= 1;
	if(context->btn_isdown_right)
		mov.x += 1;
	
	state->player->position = state->player->position + mov * (128 * state->delta);
}

static void game_render(SDLContext* context, GameState* state)
{
	for(int i = 0; i < state->entities_alive_count; ++i)
	{
		Entity* entity = &state->entities[i];

		// render texture
		SDL_FRect rect_src = entity->sprite.rect;
		SDL_FRect rect_dst;
		rect_dst.w = rect_src.w;
		rect_dst.h = rect_src.h;
		rect_dst.x = entity->position.x;
		rect_dst.y = entity->position.y;
		SDL_RenderTexture(context->renderer, entity->sprite.texture, &rect_src, &rect_dst);

		// debug texture rect
		SDL_SetRenderDrawColor(context->renderer, 0xFF, 0xFF, 0xFF,0xFF);
		SDL_RenderRect(context->renderer, &rect_dst);
		itu_lib_render_draw_point(context->renderer, entity->position, 5, COLOR_YELLOW);
	}

	// debug window
	SDL_SetRenderDrawColor(context->renderer, 0xFF, 0x00, 0xFF, 0xff);
	SDL_RenderRect(context->renderer, NULL);
}

int main(void)
{
	bool quit = false;
	SDL_Window* window;
	SDLContext context = { 0 };
	GameState  state   = { 0 };

	
	float renderer_zoom_factor = 1;

	SDL_CreateWindowAndRenderer("L03 - Coordinate Systems", WINDOW_W, WINDOW_H, 0, &window, &context.renderer);
	SDL_SetRenderVSync(context.renderer, 1);

	game_init(&context, &state);
	game_reset(&context, &state);

	SDL_Time walltime_frame_beg;
	SDL_Time walltime_frame_end;
	SDL_Time walltime_work_end;
	SDL_Time elapsed_work;
	SDL_Time elapsed_frame;

	SDL_GetCurrentTime(&walltime_frame_beg);
	walltime_frame_end = walltime_frame_beg;

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
				case SDL_EVENT_KEY_UP:
					switch(event.key.key)
					{
						case SDLK_W: sdl_input_key_process(&context, BTN_TYPE_UP, &event);        break;
						case SDLK_A: sdl_input_key_process(&context, BTN_TYPE_LEFT, &event);      break;
						case SDLK_S: sdl_input_key_process(&context, BTN_TYPE_DOWN, &event);      break;
						case SDLK_D: sdl_input_key_process(&context, BTN_TYPE_RIGHT, &event);     break;
						case SDLK_Q: sdl_input_key_process(&context, BTN_TYPE_ACTION_0, &event);  break;
						case SDLK_E: sdl_input_key_process(&context, BTN_TYPE_ACTION_1, &event);  break;
						case SDLK_SPACE: sdl_input_key_process(&context, BTN_TYPE_SPACE, &event); break;
					}
					break;
			}
		}

		SDL_SetRenderDrawColor(context.renderer, 0x00, 0x00, 0x00, 0x00);
		SDL_RenderClear(context.renderer);

		// update
		game_update(&context, &state);
		game_render(&context, &state);

		SDL_GetCurrentTime(&walltime_work_end);
		elapsed_work = walltime_work_end - walltime_frame_beg;

		if(elapsed_work < TARGET_FRAMERATE)
			SDL_DelayNS(TARGET_FRAMERATE - elapsed_work);
		SDL_GetCurrentTime(&walltime_frame_end);
		elapsed_frame = walltime_frame_end - walltime_frame_beg;
		
		SDL_SetRenderDrawColor(context.renderer, 0xFF, 0xFF, 0xFF, 0xFF);
		SDL_RenderDebugTextFormat(context.renderer, 10, 10, "work: %9.6f ms/f", (float)elapsed_work  / (float)MILLIS(1));
		SDL_RenderDebugTextFormat(context.renderer, 10, 20, "tot : %9.6f ms/f", (float)elapsed_frame / (float)MILLIS(1));

		// render
		SDL_RenderPresent(context.renderer);

		state.delta = (float)elapsed_frame / (float)SECONDS(1);
		state.elapsed += state.delta;
		walltime_frame_beg = walltime_frame_end;
	}
}
