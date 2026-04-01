#pragma once

namespace rawinput
{
	typedef BOOL(WINAPI* fPeekMessage)(
		_Out_     LPMSG lpMsg,
		_In_opt_  HWND hWnd,
		_In_      UINT wMsgFilterMin,
		_In_      UINT wMsgFilterMax,
		_In_      UINT wRemoveMsg
		);

	BOOL WINAPI PeekMessage_Hook(
		_Out_     LPMSG lpMsg,
		_In_opt_  HWND hWnd,
		_In_      UINT wMsgFilterMin,
		_In_      UINT wMsgFilterMax,
		_In_      UINT wRemoveMsg
	);

	std::function<bool(RAWMOUSE)> on_mousemove;
	std::vector<std::function<bool(RAWKEYBOARD)>> keyboard_handlers;

	inline void add_keyboard_handler(std::function<bool(RAWKEYBOARD)> handler)
	{
		keyboard_handlers.push_back(handler);
	}

	fPeekMessage mOriginalPeekMessage;

	inline bool ProcessMessage(LPMSG message)
	{
		UINT dwSize;
		GetRawInputData((HRAWINPUT)message->lParam, RID_INPUT, NULL, &dwSize, sizeof(RAWINPUTHEADER));

		LPBYTE lpb = new BYTE[dwSize];
		bool result = false;

		if (lpb)
		{
			if (GetRawInputData((HRAWINPUT)message->lParam, RID_INPUT, lpb, &dwSize, sizeof(RAWINPUTHEADER)) == dwSize)
			{
				RAWINPUT* raw = (RAWINPUT*)lpb;

				if (raw->header.dwType == RIM_TYPEKEYBOARD)
				{
					for (auto& handler : keyboard_handlers) {
						if (handler != nullptr && handler(raw->data.keyboard)) {
							result = true;
							break;
						}
					}
				}
				else if (raw->header.dwType == RIM_TYPEMOUSE)
				{
					if(on_mousemove != nullptr)
						result = on_mousemove(raw->data.mouse);
				}
			}
			delete[] lpb;
		}
		
		return result;
	}

	BOOL WINAPI PeekMessage_Hook(
		_Out_     LPMSG lpMsg,
		_In_opt_  HWND hWnd,
		_In_      UINT wMsgFilterMin,
		_In_      UINT wMsgFilterMax,
		_In_      UINT wRemoveMsg
	)
	{
		bool bResult = mOriginalPeekMessage(lpMsg, hWnd, wMsgFilterMin, wMsgFilterMax, wRemoveMsg) == 1;
		bool doInterupt = false;
		if (bResult)
		{
			switch (lpMsg->message)
			{
			case WM_INPUT:
			{
				doInterupt = ProcessMessage(lpMsg);
				break;
			}
			default:
				break;
			}
		}

		
		return bResult && !doInterupt;
	}

	inline void hook()
	{
		mOriginalPeekMessage = reinterpret_cast <fPeekMessage> (DetourFunction(DetourFindFunction("USER32.DLL", "PeekMessageW"), reinterpret_cast < PBYTE > (PeekMessage_Hook)));
	}
};