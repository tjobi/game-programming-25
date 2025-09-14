#ifndef ITU_LIB_SPRITE_HPP
#define ITU_LIB_SPRITE_HPP

#include <itu_lib_engine.hpp>

struct Sprite
{
	SDL_Texture* texture;
	SDL_FRect    rect;
	vec2f        pivot;
	color        tint;
};

void itu_lib_sprite_init(Sprite* sprite, SDL_Texture* texture, SDL_FRect rect);
SDL_FRect itu_lib_sprite_get_rect(int x, int y, int tile_w, int tile_h);
void itu_lib_sprite_render(SDLContext* context, Sprite* sprite, Transform* transform);

#endif // ITU_LIB_SPRITE_HPP

#if (defined ITU_LIB_SPRITE_IMPLEMENTATION) || (defined ITU_UNITY_BUILD)

#include <SDL3/SDL.h>


// inits sprite with reasonable defaults
void itu_lib_sprite_init(Sprite* sprite, SDL_Texture* texture, SDL_FRect rect)
{
	sprite->texture = texture;
	sprite->rect = rect;
	sprite->pivot = vec2f{ 0.5f, 0.5f };
	sprite->tint = COLOR_WHITE;
}

SDL_FRect itu_lib_sprite_get_rect(int x, int y, int tile_w, int tile_h)
{
	SDL_FRect ret;
	ret.x = x * tile_w;
	ret.y = y * tile_h;
	ret.w = tile_w;
	ret.h = tile_h;
	return ret;
}

void itu_lib_sprite_render(SDLContext* context, Sprite* sprite, Transform* transform)
{
	SDL_FRect rect_src = sprite->rect;
	SDL_FRect rect_dst;
	rect_dst.w = rect_src.w;
	rect_dst.h = rect_src.h;
	rect_dst.x = transform->position.x + sprite->pivot.x * rect_dst.w;
	rect_dst.y = transform->position.y + sprite->pivot.y * rect_dst.h;
	rect_dst = rect_global_to_screen(&context->camera, rect_dst);

	sdl_set_texture_tint(sprite->texture, sprite->tint);
	SDL_RenderTexture(context->renderer, sprite->texture, &rect_src, &rect_dst);
}

#endif // ITU_LIB_SPRITE_IMPLEMENTATION