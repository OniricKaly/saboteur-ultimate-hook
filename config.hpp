#pragma once
namespace config
{
	int			console_rows = 8;

	bool		external_console = false;
	bool		debug_strings = false;

	void init()
	{
		CIniFile ini;
		ini.Load("hookconfig.ini");

		CIniSection* console_section = ini.GetSection("console");

		if (console_section)
		{
			console_rows = std::stoi(console_section->GetKeyValue("rows"));
			external_console = (bool)std::stoi(console_section->GetKeyValue("external_console"));
			debug_strings = (bool)std::stoi(console_section->GetKeyValue("debug_strings"));
		}
	}
};