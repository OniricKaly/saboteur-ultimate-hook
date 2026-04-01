#include "cheats.hpp"

namespace lua 
{
	
	
	
	
	typedef int(__cdecl* luaL_do_string)(DWORD lua_State, const char *buff);
	unsigned long				internal_lua_state = NULL;
	luaL_do_string				internal_fdostring = nullptr;
	std::function<void(const char*)>	print_callback = NULL;
	

	
	
	
	int __cdecl hook_luaL_dostring(DWORD L, const char *buff)
	{
		if (config::debug_strings)
		{
			printf("Internal Lua doString: %s\n", buff);
		}
		if (internal_lua_state == NULL)
		{
			internal_lua_state = L;
		}
		
		return internal_fdostring(L, buff);
	}

	
	void execute_buffer(const char* buffer)
	{
		unsigned long function_address			= 0x4041B0;
		unsigned long function_address_execute	= 0x401F90;

		
		std::string s_buf(buffer);
		if (s_buf.find("Cheats.") == 0) {
			if (s_buf == "Cheats.Health(true)") cheats::toggle("health");
			else if (s_buf == "Cheats.Health(false)") cheats::toggle("health");
			else if (s_buf == "Cheats.Ammo(true)") cheats::toggle("ammo");
			else if (s_buf == "Cheats.Ammo(false)") cheats::toggle("ammo");
			else if (s_buf == "Cheats.NoSpread(true)") cheats::toggle("nospread");
			else if (s_buf == "Cheats.NoSpread(false)") cheats::toggle("nospread");
			else if (s_buf == "Cheats.RapidFire(true)") cheats::toggle("rapidfire");
			else if (s_buf == "Cheats.RapidFire(false)") cheats::toggle("rapidfire");
			else if (s_buf == "Cheats.MoneyRec()") cheats::add_money(10000);
			
			else if (s_buf == "Cheats.Blind(true)") cheats::toggle("blind");
			else if (s_buf == "Cheats.Blind(false)") cheats::toggle("blind");
			else if (s_buf == "Cheats.Allies(true)") cheats::toggle("allies");
			else if (s_buf == "Cheats.Allies(false)") cheats::toggle("allies");
			else if (s_buf == "Cheats.OHK(true)") cheats::toggle("ohk");
			else if (s_buf == "Cheats.OHK(false)") cheats::toggle("ohk");
			return;
		}

		if (internal_lua_state == NULL) return;

		__asm
		{
			push buffer
			push internal_lua_state
			call function_address
			add esp, 0x8

			push    0
			push    0
			push    0
			push    internal_lua_state
			call    function_address_execute
			add     esp, 0x10
		}
	}

	int __cdecl hook_fputs(const char *Str, FILE *File)
	{
		if (config::debug_strings == true)
		{
			printf(Str);
		}
		if (print_callback)
			print_callback(Str);
		return 1;
	}


	
	void hook()
	{
		cheats::init();
		internal_fdostring = (luaL_do_string)DetourFunction((PBYTE)0x004041B0, (PBYTE)&hook_luaL_dostring);
		MemoryPatcher::InstallNopPatch(0x006FAB6B, 5);
		DetourFunction((PBYTE)0x00E4DFAC, (PBYTE)&hook_fputs);

	}
};