#include "main.h"

class colour
{
public:
	float col[4];
	colour(float _col[4])
	{
		for (int i = 0; i < 4; ++i)
		{
			col[i] = _col[i];
		}
	}
	colour(float _col)
	{
		for (int i = 0; i < 3; ++i)
		{
			col[i] = _col;
		}
		col[3] = 1.0f;
	}
};

typedef std::vector<colour> palette_t;

class bitplane
{
public:
	std::vector<bool> bits;
	bitplane(int length)
	{
		bits.resize(length);
	}
	bitplane() = default;
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
	pixel& operator=(uint8_t newcol)
	{
		for (int i = 0; i < m_Bitplanes->size(); i++)
		{
			bool bit = newcol >> i & 1;
			m_Bitplanes->at(i).bits.at(index) = bit;
		}
	}
	uint8_t operator()()
	{
		int ret = 0;
		for (int i = 0; i < m_Bitplanes->size(); i++)
		{
			ret += m_Bitplanes->at(i).bits[index] << i;
		}
		return ret;
	}

};

class tile
{
public:
	ImVec2 m_Size;
	std::vector<pixel> pixels;
	tile(ImVec2 size, int bpp, std::vector<bitplane>* bitplanes)
	{
		m_Size = size;
		bitplanes->resize(bpp);
		for (int x = 0; x < m_Size.x; x++)
		{
			for (int y = 0; y < m_Size.x; y++)
			{
				pixel temp(bitplanes, x * y);
				pixels.push_back(temp);
			}
		}
	}
};

typedef std::vector<tile> tileMap_t;

void clamp(int& v, int min, int max)
{
	if (v < min)
	{
		v = min;
	}
	else if (v > max)
	{
		v = max;
	}
}


int main(int argc, char* argv[])
{
	palette_t palette;
	palette.resize(256, colour(0.1f));
	
	int width = 16;
	int height = 16;
	int zoom = 1;
	int bpp = 8;
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* canvas_window = SDL_CreateWindow("VTileEdit - Canvas", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 1024, 1024, SDL_WINDOW_SHOWN);
	SDL_Window* controls_window = SDL_CreateWindow("VTileEdit - Controls", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
#ifdef _WIN64
	SDL_Renderer* controls_renderer = SDL_CreateRenderer(controls_window, -1, SDL_RENDERER_SOFTWARE);
#else
	SDL_Renderer* controls_renderer = SDL_CreateRenderer(controls_window, -1, SDL_RENDERER_ACCELERATED);
#endif
	SDL_Renderer* canvas_window_render = SDL_CreateRenderer(canvas_window, -1, SDL_RENDERER_ACCELERATED);
    if (controls_renderer == nullptr || controls_window == nullptr || canvas_window == nullptr || canvas_window_render == nullptr) {
		std::cout << SDL_GetError() << std::endl;
		return 1;
	}
	ImGui::CreateContext();
	ImGuiSDL::Initialize(controls_renderer, 800, 600);

	tileMap_t tileMap;
	std::vector<bitplane> bitplanes;
	bitplanes.resize(bpp);
	for (int i = 0; i < bpp; i++)
	{
		bitplanes[i].bits.resize((width * height) * 256);
	}
	tileMap.resize(256, tile(ImVec2(width, height), bpp, &bitplanes));
	
	bool run = true;
	int palInd = 0;
	while (run)
	{
#ifdef _DEBUG
		using namespace std::chrono;

		high_resolution_clock::time_point t1 = high_resolution_clock::now();
#endif
		ImGuiIO& io = ImGui::GetIO();

		int wheel = 0;

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			if (e.type == SDL_QUIT) run = false;
			else if (e.type == SDL_WINDOWEVENT)
			{
				if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED)
				{
					io.DisplaySize.x = static_cast<float>(e.window.data1);
					io.DisplaySize.y = static_cast<float>(e.window.data2);
				}
			}
			else if (e.type == SDL_MOUSEWHEEL)
			{
				wheel = e.wheel.y;
			}
		}

		int mouseX, mouseY;
		const int buttons = SDL_GetMouseState(&mouseX, &mouseY);

		// Setup low-level inputs (e.g. on Win32, GetKeyboardState(), or write to those fields from your Windows message loop handlers, etc.)

		io.DeltaTime = 1.0f / 60.0f;
		io.MousePos = ImVec2(static_cast<float>(mouseX), static_cast<float>(mouseY));
		io.MouseDown[0] = buttons & SDL_BUTTON(SDL_BUTTON_LEFT);
		io.MouseDown[1] = buttons & SDL_BUTTON(SDL_BUTTON_RIGHT);
		io.MouseWheel = static_cast<float>(wheel);

		ImGui::NewFrame();

		ImGui::Begin("Palette");
		ImGui::ColorPicker4("Colour", palette[palInd].col);
		ImGui::Text("Palette Index");
		ImGui::SliderInt("", &palInd, 0, 255);
		if (ImGui::Button("+"))
		{
			palInd++;
		}
		if (ImGui::Button("-"))
		{
			palInd--;
		}
		clamp(palInd, 0, 255);
		ImGui::End();
		ImGui::Begin("Canvas Controls");
		ImGui::SliderInt("Zoom", &zoom, 1, 4);
		ImGui::End();
		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());

		SDL_RenderPresent(controls_renderer);

		SDL_SetRenderDrawColor(controls_renderer, 114, 144, 154, 255);
		SDL_RenderClear(controls_renderer);
		SDL_SetRenderDrawColor(canvas_window_render, 128, 128, 128, 255);
		SDL_RenderClear(canvas_window_render);
		for (int tile_y = 0; tile_y < 16; tile_y += 1)
		{
			for (int tile_x = 0; tile_x < 16; tile_x += 1) {
				tile curTile = tileMap[tile_x * tile_y];
				for (int x = 0; x < width; ++x)
				{
					for (int y = 0; y < height; ++y)
					{
						colour temp = palette[curTile.pixels[x * y]()];
						SDL_SetRenderDrawColor(canvas_window_render, temp.col[0] * 256, temp.col[1] * 256, temp.col[2] * 256, temp.col[3] * 256);
						SDL_RenderDrawPoint(canvas_window_render, tile_x*width + x, tile_y*height + y);
					}
				}
			}
		}
		
		SDL_RenderPresent(canvas_window_render);
#ifdef _DEBUG
		high_resolution_clock::time_point t2 = high_resolution_clock::now();

		std::chrono::duration<float, std::milli> fp_ms = t2 - t1;

		printf("Frametime = %f\n", fp_ms.count());
		printf("Framerate = %f\n", 1000/fp_ms.count());
#endif
	}

	ImGuiSDL::Deinitialize();

	SDL_DestroyRenderer(controls_renderer);
	SDL_DestroyWindow(controls_window);
	SDL_DestroyRenderer(canvas_window_render);
	SDL_DestroyWindow(canvas_window);
	ImGui::DestroyContext();
	return 0;
}
