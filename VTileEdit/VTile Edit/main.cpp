#include "main.h"

class colour
{
public:
	float col[4];
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
	ImVec2 m_Size;
	std::vector<bitplane> m_Bitplanes;
	std::vector<pixel> pixels;
	tile(ImVec2 size, int bpp)
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
	palette.resize(256);
	SDL_Init(SDL_INIT_EVERYTHING);
	SDL_Window* controls_window = SDL_CreateWindow("VTileEdit - Controls", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_RESIZABLE);
#ifdef _WIN64
	SDL_Renderer* controls_renderer = SDL_CreateRenderer(controls_window, -1, SDL_RENDERER_SOFTWARE);
#else
	SDL_Renderer* controls_renderer = SDL_CreateRenderer(controls_window, -1, SDL_RENDERER_ACCELERATED);
#endif
	SDL_Renderer* canvas_renderer = SDL_CreateRenderer(nullptr, -1, SDL_RENDERER_TARGETTEXTURE);
	ImGui::CreateContext();
	ImGuiSDL::Initialize(controls_renderer, 800, 600);

	bool run = true;
	int palInd = 0;
	int width = 16;
	int height = 16;
	int bpp = 8;
	tile curTile(ImVec2(width, height), bpp);
	while (run)
	{
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
		ImGui::Render();
		ImGuiSDL::Render(ImGui::GetDrawData());

		SDL_RenderPresent(controls_renderer);

		SDL_SetRenderDrawColor(controls_renderer, 114, 144, 154, 255);
		SDL_RenderClear(controls_renderer);
	}

	ImGuiSDL::Deinitialize();

	SDL_DestroyRenderer(controls_renderer);
	SDL_DestroyWindow(controls_window);
	ImGui::DestroyContext();
	return 0;
}
