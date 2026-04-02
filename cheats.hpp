#pragma once
#include "MemoryPatcher.h"
#include <map>
#include <string>
#include <vector>

namespace cheats
{
	struct CheatState {
		DWORD address;
		std::vector<BYTE> original_bytes;
		bool enabled = false;
	};

	__declspec(selectany) std::map<std::string, CheatState> states;

	inline DWORD get_base() {
		return (DWORD)GetModuleHandle(NULL);
	}

	inline void init() {
		states["health_v1"].address = get_base() + 0x1A7A70;
		states["health_v1"].original_bytes = { 0xD9, 0x9E, 0x4C, 0x0D, 0x00, 0x00 };
		
		states["health_v2"].address = get_base() + 0x1A6A00; 
		states["health_v2"].original_bytes = { 0xD9, 0x9E, 0x4C, 0x0D, 0x00, 0x00 };

		states["health_v3"].address = MemoryPatcher::FindPattern("\xD9\x9E\x4C\x0D\x00\x00", "xxxxxx");
		states["health_v3"].original_bytes = { 0xD9, 0x9E, 0x4C, 0x0D, 0x00, 0x00 };
		
		states["ammo"].address = get_base() + 0x2BD845;
		states["ammo"].original_bytes = { 0x83, 0x46, 0x24, 0xFF };

		states["nospread"].address = get_base() + 0x2BC4D9;
		states["nospread"].original_bytes = { 0xD8, 0x4E, 0x54 };

		states["rapidfire"].address = get_base() + 0x2B3D65;
		states["rapidfire"].original_bytes = { 0xDC, 0x25, 0x48, 0xD5, 0xF7, 0x00 };

		
		states["allies"].address = get_base() + 0xF541D;
		states["allies"].original_bytes = { 0xD9, 0x86, 0x4C, 0x0D, 0x00, 0x00 };

		states["ohk"].address = get_base() + 0x106209;
		states["ohk"].original_bytes = { 0xD9, 0x47, 0x0C };

		states["blind"].address = get_base() + 0x4949CB;
		states["blind"].original_bytes = { 0x8A, 0x4E, 0x08 };

		states["stealth"].address = get_base() + 0x4A895E; 
		states["stealth"].original_bytes = { 0x8B, 0x46, 0x38 };
	}

	inline bool is_enabled(const std::string& id) {
		if (states.find(id) == states.end()) return false;
		return states[id].enabled;
	}

	inline void toggle(const std::string& id) {
		if (states.find(id) == states.end()) return;
		auto& cheat = states[id];
		bool enable = !cheat.enabled;

		if (enable) {
			if (id.find("health") != std::string::npos || id == "ammo" || id == "nospread" || id == "allies") {
				if (cheat.address > 0) MemoryPatcher::InstallNopPatch(cheat.address, (int)cheat.original_bytes.size());
			} else if (id == "rapidfire") {
				MemoryPatcher::InstallNopPatch(cheat.address, 6);
			} else if (id == "blind") {
				BYTE patch[] = { 0xB1, 0x00, 0x90 }; 
				MemoryPatcher::PatchAddress(cheat.address, patch, 3);
			} else if (id == "ohk") {
				BYTE patch[] = { 0xD9, 0xEE, 0x90 };
				MemoryPatcher::PatchAddress(cheat.address, patch, 3);
			} else if (id == "stealth") {
				BYTE patch[] = { 0x31, 0xC0, 0x90 }; 
				MemoryPatcher::PatchAddress(cheat.address, patch, 3);
			}
			cheat.enabled = true;
		} else {
			if (cheat.address > 0) MemoryPatcher::PatchAddress(cheat.address, cheat.original_bytes.data(), cheat.original_bytes.size());
			cheat.enabled = false;
		}
	}

	inline void add_money(int amount) {
		DWORD* money_ptr = (DWORD*)(get_base() + 0x1094324);
		if (money_ptr && !IsBadReadPtr(money_ptr, 4)) {
			*money_ptr += amount;
		}
	}
}
