
typedef void (* D3D9DeviceCreateFunc)(IDirect3DDevice9* device);
typedef void (* D3D9DeviceBeginSceneFunc)(IDirect3DDevice9* device);
typedef void (* D3D9DeviceEndSceneFunc)(IDirect3DDevice9* device);
typedef void (* D3D9DevicePreResetFunc)();
typedef void (* D3D9DevicePostResetFunc)(IDirect3DDevice9* device, HRESULT res);
typedef void (* D3D9DevicePrePresentFunc)(IDirect3DDevice9* device);
typedef void (* D3D9DevicePostPresentFunc)(IDirect3DDevice9* device, HRESULT res);


enum D3D9_DEVICE_FUNCTION{
	
	CREATE = 15,
	PRERESET = 16,
	POSTRESET = 119,
	POSTPRESENT = 17,
	PREPRESENT = 18,
	
	ENDSCENE = 42,
	CLEAR = 43
	
};
#define D3D9_DEVICE_FUNC_COUNT 120 


#define D3D_DEVICE_PROXY_STATUS_NOTREADY -1
#define D3D_DEVICE_PROXY_STATUS_WRONG_FUNC_ID -2
#define D3D_DEVICE_PROXY_STATUS_WRONG_FUNC -3