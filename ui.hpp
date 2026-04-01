#pragma once
#include "renderer.hpp"

namespace ui
{
	inline void draw_panel(float x, float y, float w, float h)
	{
		
		renderer::draw_rect_gradient(x, y, w, h,
			D3DCOLOR_ARGB(220, 25, 25, 25),
			D3DCOLOR_ARGB(220, 25, 25, 25),
			D3DCOLOR_ARGB(245, 5, 5, 5),
			D3DCOLOR_ARGB(245, 5, 5, 5)
		);

		
		renderer::draw_rect(x - 1, y - 1, w + 2, 1, D3DCOLOR_ARGB(150, 180, 20, 20)); 
		renderer::draw_rect(x - 1, y + h, w + 2, 1, D3DCOLOR_ARGB(150, 50, 5, 5));   
		renderer::draw_rect(x - 1, y, 1, h, D3DCOLOR_ARGB(100, 180, 20, 20));        
		renderer::draw_rect(x + w, y, 1, h, D3DCOLOR_ARGB(100, 180, 20, 20));        
	}

	inline void draw_header(renderer::Font* font, const char* text, float x, float y)
	{
		
		font->set_pos((int)x + 2, (int)y + 2);
		font->set_color(D3DCOLOR_ARGB(200, 0, 0, 0));
		font->draw(text);
		
		
		font->set_pos((int)x, (int)y);
		font->set_color(D3DCOLOR_ARGB(255, 255, 255, 255));
		font->draw(text);
	}

	inline void draw_separator(float x, float y, float w)
	{
		
		renderer::draw_rect_gradient(x, y, w, 2.0f, 
			D3DCOLOR_ARGB(0, 180, 20, 20),
			D3DCOLOR_ARGB(255, 180, 20, 20),
			D3DCOLOR_ARGB(0, 50, 5, 5),
			D3DCOLOR_ARGB(255, 50, 5, 5)
		);
	}

	inline void draw_highlight(float x, float y, float w, float h)
	{
		
		
		renderer::draw_rect(x, y - 2, w, h + 4, D3DCOLOR_ARGB(60, 255, 0, 0));
		
		renderer::draw_rect_gradient(x, y, w, h, 
			D3DCOLOR_ARGB(220, 200, 20, 20),
			D3DCOLOR_ARGB(150, 120, 10, 10),
			D3DCOLOR_ARGB(220, 200, 20, 20),
			D3DCOLOR_ARGB(150, 120, 10, 10)
		);
		
		renderer::draw_rect(x, y, 4, h, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	struct AnimFloat {
		float value = 0.0f;
		void update(float target, float speed) {
			value += (target - value) * speed;
		}
	};
};
