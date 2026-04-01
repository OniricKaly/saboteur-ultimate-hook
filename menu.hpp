#pragma once
#include <windows.h>
#include <string>
#include <vector>
#include <sstream>

namespace menu
{
	struct MenuItem {
		std::string display_name;
		std::string filepath;
		std::string lua_code;
		std::string cheat_id;
		
		bool is_submenu = false;    
		bool is_expandable = false; 
		bool expanded = false;
		bool is_toggle = false;
		int depth = 0;
		
		std::vector<MenuItem> children;
	};

	std::vector<MenuItem> loaded_mods; 
	std::vector<MenuItem> submenu_tree; 
	std::vector<MenuItem*> visible_items; 

	std::string current_submenu_name = "";
	bool in_submenu = false;

	int target_selected_index = 0;
	bool menu_visible = false;
	
	
	ui::AnimFloat visual_selected_index;
	ui::AnimFloat menu_anim;

	auto menu_font = new renderer::Font;
	auto title_font = new renderer::Font;

	
	inline void flatten_tree(std::vector<MenuItem>& source, int depth) {
		for (auto& item : source) {
			item.depth = depth;
			visible_items.push_back(&item);
			if (item.is_expandable && item.expanded) {
				flatten_tree(item.children, depth + 1);
			}
		}
	}

	inline void rebuild_visible_list() {
		visible_items.clear();
		if (!in_submenu) {
			for (auto& item : loaded_mods) {
				item.depth = 0;
				visible_items.push_back(&item);
			}
		} else {
			flatten_tree(submenu_tree, 0);
		}
		
		if (target_selected_index >= visible_items.size()) {
			target_selected_index = visible_items.empty() ? 0 : (int)visible_items.size() - 1;
		}
	}

	
	inline void parse_file_recursive(const std::string& path, std::vector<MenuItem>& target_list) {
		std::FILE* fp = std::fopen(path.c_str(), "r");
		if (!fp) return;

		char line[1024];
		while (std::fgets(line, sizeof(line), fp)) {
			std::string s_line(line);
			size_t pos;

			if ((pos = s_line.find("-- @Item:")) != std::string::npos) {
				std::string content = s_line.substr(pos + 9);
				size_t sep = content.find(">>");
				if (sep != std::string::npos) {
					MenuItem item;
					item.display_name = content.substr(0, sep);
					item.lua_code = content.substr(sep + 2);
					
					item.display_name.erase(0, item.display_name.find_first_not_of(" \t"));
					item.display_name.erase(item.display_name.find_last_not_of(" \t\r\n") + 1);
					item.lua_code.erase(0, item.lua_code.find_first_not_of(" \t"));
					item.lua_code.erase(item.lua_code.find_last_not_of(" \t\r\n") + 1);
					target_list.push_back(item);
				}
			}
			else if ((pos = s_line.find("-- @Toggle:")) != std::string::npos) {
				std::string content = s_line.substr(pos + 11);
				size_t sep = content.find(">>");
				if (sep != std::string::npos) {
					MenuItem item;
					item.display_name = content.substr(0, sep);
					item.cheat_id = content.substr(sep + 2);
					item.display_name.erase(0, item.display_name.find_first_not_of(" \t"));
					item.display_name.erase(item.display_name.find_last_not_of(" \t\r\n") + 1);
					item.cheat_id.erase(0, item.cheat_id.find_first_not_of(" \t"));
					item.cheat_id.erase(item.cheat_id.find_last_not_of(" \t\r\n") + 1);
					item.is_toggle = true;
					target_list.push_back(item);
				}
			}
			else if ((pos = s_line.find("-- @Submenu:")) != std::string::npos) {
				std::string content = s_line.substr(pos + 12);
				size_t sep = content.find(">>");
				if (sep != std::string::npos) {
					MenuItem item;
					item.display_name = content.substr(0, sep);
					std::string rel_path = content.substr(sep + 2);
					rel_path.erase(0, rel_path.find_first_not_of(" \t"));
					rel_path.erase(rel_path.find_last_not_of(" \t\r\n") + 1);
					
					item.display_name.erase(0, item.display_name.find_first_not_of(" \t"));
					item.display_name.erase(item.display_name.find_last_not_of(" \t\r\n") + 1);
					
					item.filepath = "mods\\" + rel_path;
					item.is_submenu = true;
					target_list.push_back(item);
				}
			}
			else if ((pos = s_line.find("-- @Expand:")) != std::string::npos) {
				std::string content = s_line.substr(pos + 11);
				size_t sep = content.find(">>");
				if (sep != std::string::npos) {
					MenuItem item;
					item.display_name = content.substr(0, sep);
					std::string rel_path = content.substr(sep + 2);
					rel_path.erase(0, rel_path.find_first_not_of(" \t"));
					rel_path.erase(rel_path.find_last_not_of(" \t\r\n") + 1);
					
					item.display_name.erase(0, item.display_name.find_first_not_of(" \t"));
					item.display_name.erase(item.display_name.find_last_not_of(" \t\r\n") + 1);
					
					std::string sub_full = "mods\\" + rel_path;
					item.is_expandable = true;
					parse_file_recursive(sub_full, item.children);
					target_list.push_back(item);
				}
			}
		}
		std::fclose(fp);
	}

	inline void scan_mods_folder()
	{
		loaded_mods.clear();
		WIN32_FIND_DATAA findFileData;
		HANDLE hFind = FindFirstFileA("mods\\*.lua", &findFileData);

		if (hFind != INVALID_HANDLE_VALUE) {
			do {
				if (!(findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
					std::string fname = findFileData.cFileName;
					
					std::FILE* fp = std::fopen(("mods\\" + fname).c_str(), "r");
					if (fp) {
						char line[256];
						if (std::fgets(line, sizeof(line), fp)) {
							std::string s_line(line);
							size_t pos = s_line.find("-- @MenuName:");
							if (pos != std::string::npos) {
								MenuItem item;
								item.filepath = "mods\\" + fname;
								item.is_submenu = true;
								item.display_name = s_line.substr(pos + 13);
								item.display_name.erase(0, item.display_name.find_first_not_of(" \t"));
								item.display_name.erase(item.display_name.find_last_not_of(" \t\r\n") + 1);
								loaded_mods.push_back(item);
							}
						}
						std::fclose(fp);
					}
				}
			} while (FindNextFileA(hFind, &findFileData) != 0);
			FindClose(hFind);
		}
		rebuild_visible_list();
	}

	inline void load_submenu(const std::string& filepath, const std::string& name)
	{
		submenu_tree.clear();
		current_submenu_name = name;
		parse_file_recursive(filepath, submenu_tree);
		in_submenu = true;
		target_selected_index = 0;
		visual_selected_index.value = 0.0f;
		rebuild_visible_list();
	}

	inline void gui_on_draw()
	{
		menu_anim.update(menu_visible ? 1.0f : 0.0f, 0.15f);
		if (menu_anim.value < 0.01f) return;

		renderer::global_alpha = menu_anim.value;
		visual_selected_index.update((float)target_selected_index, 0.2f);

		int screen_h = renderer::internal_resolution.y;
		int menu_w = 400;
		int menu_h = 500;
		int start_x = 50; 
		int start_y = (screen_h - menu_h) / 2;

		ui::draw_panel(start_x, start_y, menu_w, menu_h);
		ui::draw_header(title_font, in_submenu ? current_submenu_name.c_str() : "RESISTANCE MOD MENU", start_x + 20, start_y + 15);
		ui::draw_separator(start_x + 10, start_y + 45, menu_w - 20);

		int list_start_y = start_y + 60;
		int item_height = 25;

		if (visible_items.empty()) {
			menu_font->set_pos(start_x + 20, list_start_y + 10);
			menu_font->set_color(D3DCOLOR_ARGB(255, 150, 150, 150));
			menu_font->draw("No visible items.");
		} else {
			int max_visible = (menu_h - 70) / item_height;
			int display_start = 0;
			if (target_selected_index >= max_visible) display_start = target_selected_index - max_visible + 1;
			
			float mapped_visual_idx = visual_selected_index.value - display_start;
			if (mapped_visual_idx < 0) mapped_visual_idx = 0;

			ui::draw_highlight(start_x + 10, list_start_y + (int)(mapped_visual_idx * item_height), menu_w - 20, item_height + 4);

			int drawn = 0;
			for (size_t i = display_start; i < visible_items.size() && drawn < max_visible; ++i) {
				MenuItem* item = visible_items[i];
				int x_offset = 30 + (item->depth * 20);
				menu_font->set_pos(start_x + x_offset, list_start_y + drawn * item_height + 2);
				
				bool focused = (i == target_selected_index);
				if (focused) {
					menu_font->set_color(D3DCOLOR_ARGB(255, 255, 255, 255));
					
					menu_font->set_pos(start_x + x_offset + 1, list_start_y + drawn * item_height + 3);
					menu_font->set_color(D3DCOLOR_ARGB(120, 0, 0, 0));
					menu_font->draw(item->display_name.c_str());
					menu_font->set_pos(start_x + x_offset, list_start_y + drawn * item_height + 2);
					menu_font->set_color(D3DCOLOR_ARGB(255, 255, 255, 255));
				} else {
					menu_font->set_color(D3DCOLOR_ARGB(255, 140, 140, 140));
				}

				std::string name = item->display_name;
				if (item->is_submenu) name = "> " + name;
				else if (item->is_expandable) name = (item->expanded ? "v " : "> ") + name;
				
				if (item->is_toggle) {
					bool act = cheats::is_enabled(item->cheat_id);
					name += act ? " [ON]" : " [OFF]";
					
					if (focused) {
						menu_font->draw(name.substr(0, name.length() - 5).c_str());
						int w = menu_font->get_width(name.substr(0, name.length() - 5).c_str());
						menu_font->set_pos(start_x + x_offset + w, list_start_y + drawn * item_height + 2);
						menu_font->set_color(act ? D3DCOLOR_ARGB(255, 220, 20, 20) : D3DCOLOR_ARGB(255, 120, 120, 120));
						menu_font->draw(name.substr(name.length() - 5).c_str());
					} else {
						menu_font->draw(name.c_str());
					}
				} else {
					menu_font->draw(name.c_str());
				}
				drawn++;
			}
		}
		renderer::global_alpha = 1.0f;
	}

	inline bool on_keyboard(RAWKEYBOARD keyboard)
	{
		ULONG message = keyboard.Message;
		USHORT VKey = keyboard.VKey;

		if (message == WM_KEYDOWN || message == WM_SYSKEYDOWN) {
			if (VKey == VK_NUMPAD7) {
				menu_visible = !menu_visible;
				if (menu_visible) { in_submenu = false; scan_mods_folder(); }
				return true;
			}
			if (!menu_visible) return false;

			if (VKey == VK_BACK || VKey == VK_NUMPAD4 || VKey == VK_LEFT) {
				if (in_submenu) { in_submenu = false; target_selected_index = 0; rebuild_visible_list(); }
				return true;
			}

			if (visible_items.empty()) return true;

			if (VKey == VK_NUMPAD2 || VKey == VK_DOWN) {
				target_selected_index++;
				if (target_selected_index >= visible_items.size()) target_selected_index = 0;
				return true;
			}
			if (VKey == VK_NUMPAD8 || VKey == VK_UP) {
				target_selected_index--;
				if (target_selected_index < 0) target_selected_index = (int)visible_items.size() - 1;
				return true;
			}

			if (VKey == VK_NUMPAD5 || VKey == VK_RETURN) {
				MenuItem* item = visible_items[target_selected_index];
				if (item->is_expandable) {
					item->expanded = !item->expanded;
					rebuild_visible_list();
				} else if (item->is_submenu) {
					load_submenu(item->filepath, item->display_name);
				} else if (item->is_toggle) {
					cheats::toggle(item->cheat_id);
				} else {
					lua::execute_buffer(item->lua_code.c_str());
					menu_visible = false;
				}
				return true;
			}
		}
		return false;
	}

	inline void init()
	{
		title_font->set_height(24); title_font->create();
		menu_font->set_height(20); menu_font->create();
		renderer::on_draw(gui_on_draw);
		rawinput::add_keyboard_handler(on_keyboard);
	}
};
