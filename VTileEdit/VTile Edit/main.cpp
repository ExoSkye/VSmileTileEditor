#include "main.h"

struct colour
{
	int r = 0;
	int b = 0;
	int g = 0;
	int a = 0;
};

typedef std::vector<colour> palette_t;

class bitplane
{
	std::vector<bool> bits;
};


class pixel
{
public:
	int index;
	std::vector<bitplane>* m_Bitplanes;
	pixel(std::vector<bitplane>* bitplanes, int _index)
	{
		index = _index;
		m_Bitplanes = bitplanes;
	}

};

struct vec2
{
	int x, y;
};

class tile
{
public:
	vec2 m_Size;
	std::vector<bitplane> m_Bitplanes;
	std::vector<pixel> pixels;
	tile(vec2 size, int bpp)
	{
		m_Size = size;
		m_Bitplanes.resize(bpp);
		for (int x = 0; x < m_Size.x; x++)
		{
			for (int y = 0; y < m_Size.x; y++)
			{
				pixel temp(&m_Bitplanes, x * y);
				pixels.push_back(temp);
			}
		}
	}
};

typedef std::vector<tile> tileMap;

int main(int argc, char* argv[])
{
	palette_t palette;
	palette.resize(256);

	SDL_Window* imgui_w = SDL_CreateWindow("Controls - VTileEdit", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_VULKAN);
	SDL_Renderer* imgui_r = SDL_CreateRenderer(imgui_w, -1, SDL_RENDERER_ACCELERATED);
	ImGuiSDL::Initialize(imgui_r, 800, 600);
	
	return 0;
}
