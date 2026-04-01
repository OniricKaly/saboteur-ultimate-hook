#pragma once

namespace renderer
{
	class Font;

	__declspec(selectany) LPDIRECT3DDEVICE9	internal_device = nullptr;
	__declspec(selectany) LPD3DXSPRITE		internal_sprite = nullptr;
	__declspec(selectany) D3DXVECTOR2		internal_resolution;
	__declspec(selectany) std::vector<std::function<void()>> internal_on_draw;
	
	__declspec(selectany) float global_alpha = 1.0f;

	inline D3DCOLOR apply_alpha(D3DCOLOR col, float alphaMod)
	{
		int a = (col >> 24) & 0xFF;
		int r = (col >> 16) & 0xFF;
		int g = (col >> 8) & 0xFF;
		int b = col & 0xFF;
		a = (int)(a * alphaMod);
		if (a < 0) a = 0;
		if (a > 255) a = 255;
		return D3DCOLOR_ARGB(a, r, g, b);
	}

	class Font {
	public:
		Font() :
			m_font_height(15),
			m_font_name("Arial"),
			m_font_color(0xFFFFFFFF),
			m_font_sprite(internal_sprite)
		{
		}

		~Font()
		{
			if (m_font != nullptr) {
				m_font->Release();
			}
		}

		auto get_height() { return m_font_height; }
		int get_width(const char* text) {
			if (!m_font) return 0;
			RECT rc = { 0, 0, 0, 0 };
			m_font->DrawTextA(NULL, text, -1, &rc, DT_CALCRECT, 0);
			return rc.right - rc.left;
		}
		void set_height(int font_size) { m_font_height = font_size; }
		void set_name(std::string name) { m_font_name = name; }
		void set_color(unsigned long color) { m_font_color = color; }
		void set_sprite(LPD3DXSPRITE sprite) { m_font_sprite = sprite; }

		void set_pos(int x, int y)
		{
			m_font_transform.left	= x;
			m_font_transform.top	= y;
			m_font_transform.right	= (LONG)internal_resolution.x;
			m_font_transform.bottom = (LONG)internal_resolution.y;
		}

		void draw(const char* text)
		{
			if (m_font && global_alpha > 0.01f) {
				D3DCOLOR col = apply_alpha(m_font_color, global_alpha);
				m_font->DrawTextA(m_font_sprite, text, -1, &m_font_transform, DT_NOCLIP, col);
			}
		}

		void draw(const wchar_t* text)
		{
			if (m_font && global_alpha > 0.01f) {
				D3DCOLOR col = apply_alpha(m_font_color, global_alpha);
				m_font->DrawTextW(m_font_sprite, text, -1, &m_font_transform, DT_NOCLIP, col);
			}
		}

		void create(void)
		{
			
			if (FAILED(D3DXCreateFontA(internal_device, m_font_height, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, ANTIALIASED_QUALITY, DEFAULT_PITCH | FF_DONTCARE, m_font_name.c_str(), &m_font))) {
				MessageBoxA(NULL, "Unable to create directx font !", "ERROR", MB_OK);
			}
		}
	private:
		LPD3DXFONT		m_font;
		LPD3DXSPRITE	m_font_sprite;
		int				m_font_height;
		std::string		m_font_name;
		RECT			m_font_transform;
		unsigned long	m_font_color;
	};
	
	struct Vertex_t
	{
		FLOAT x, y, z, rhw; 
		D3DCOLOR color;

		enum
		{
			FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE
		};
	};

	inline void buid_vertex(FLOAT x, FLOAT y, FLOAT z, FLOAT rhw, D3DCOLOR color, Vertex_t* vertexList, int index)
	{
		vertexList[index].x = x;
		vertexList[index].y = y;
		vertexList[index].z = z;
		vertexList[index].rhw = rhw;
		vertexList[index].color = apply_alpha(color, global_alpha);
	}

	inline void draw_rect_gradient(float x, float y, float w, float h, D3DCOLOR colTL, D3DCOLOR colTR, D3DCOLOR colBL, D3DCOLOR colBR)
	{
		if (global_alpha < 0.01f) return;

		Vertex_t vertex_list[4];
		buid_vertex(x, y + h, 0.0f, 1.0f, colBL, vertex_list, 0);
		buid_vertex(x, y, 0.0f, 1.0f, colTL, vertex_list, 1);
		buid_vertex(x + w, y + h, 0.0f, 1.0f, colBR, vertex_list, 2);
		buid_vertex(x + w, y, 0.0f, 1.0f, colTR, vertex_list, 3);

		internal_device->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		internal_device->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
		internal_device->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
		internal_device->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
		internal_device->SetRenderState(D3DRS_ZENABLE, D3DZB_FALSE);

		internal_device->SetFVF(Vertex_t::FVF);
		internal_device->SetTexture(0, NULL);
		internal_device->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, vertex_list, sizeof(Vertex_t));
	}

	inline void draw_rect(float x, float y, float w, float h, D3DCOLOR color)
	{
		draw_rect_gradient(x, y, w, h, color, color, color, color);
	}

	inline D3DXVECTOR2 get_resolution()
	{
		IDirect3DSurface9* back_buffer;
		internal_device->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &back_buffer);

		D3DSURFACE_DESC surface_description;
		back_buffer->GetDesc(&surface_description);

		D3DXVECTOR2 returnVec = D3DXVECTOR2((FLOAT)surface_description.Width, (FLOAT)surface_description.Height);
		back_buffer->Release();

		return returnVec;
	}

	inline void init(IDirect3DDevice9* device)
	{
		if (device != nullptr) {
			internal_device = device;
			if (FAILED(D3DXCreateSprite(device, &internal_sprite))) {
				MessageBoxA(NULL, "Unable to create sprite !", "ERROR", MB_OK);
			}
			internal_resolution = get_resolution();
		}
	}

	inline void on_draw(std::function<void()> callback)
	{
		if(callback != nullptr)
			internal_on_draw.push_back(callback);
	}

	inline void render()
	{
		if (internal_sprite != nullptr) {
			
			
			internal_sprite->Begin(D3DXSPRITE_ALPHABLEND);

			for (auto event : internal_on_draw) {
				event();
			}

			internal_sprite->End();
		}
	}
};
