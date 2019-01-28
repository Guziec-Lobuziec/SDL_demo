#ifndef DRAWABLE_HPP
#define DRAWABLE_HPP

#include <SDL2/SDL.h>
#include <memory>
#include <vector>
#include <tuple>
#include <algorithm>
#include <string>
#include "alg.hpp"

class drawable {
public:

	drawable(int init_width, int init_height, Uint32 fill = 0x00000000);
	drawable(const std::string& file);

	drawable& draw(std::vector<std::tuple<point<int>,Uint32>> v);

	drawable& resize(int new_left, int new_right, int new_top, int new_bottom, Uint32 fill  = 0x00000000);

	void paste_to_renderer(
		std::shared_ptr<SDL_Renderer> renderer,
		const point<int> at,
		const double angle = 0
	);

	drawable clone();

	drawable& createTexture(std::shared_ptr<SDL_Renderer> renderer);

	int height() const;

	int width() const;

private:
	std::shared_ptr<SDL_Surface> apperance;
	std::shared_ptr<SDL_Texture> texture;

};

#endif
