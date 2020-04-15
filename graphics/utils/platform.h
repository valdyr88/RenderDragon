#ifndef PLATFORM_H
#define PLATFORM_H

#include <string>
#include "types/types.h"
#include "platform_defines.h"

#ifdef PLATFORM_WINDOWS
	#include <windows.h>
#endif
#ifdef PLATFORM_LINUX
#endif
#ifdef PLATFORM_MAC
#endif

struct SWindow{
#ifdef PLATFORM_WINDOWS
	//Windows-specific window
	HWND console = nullptr;
	WNDCLASS windowClass;
	HWND window = nullptr;
	HDC windowDeviceContext = nullptr;
	HINSTANCE hInstance = nullptr;
	uint flags = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	char* name = nullptr;

	HWND getWindowHandle(){ return window; }
#endif
#ifdef PLATFORM_LINUX
	//Linux-specific window
	Window window;
	XWindowAttributes attribs;
	Display* display = nullptr;

	char* name = nullptr;
#endif
#ifdef PLATFORM_MAC
	//MacOS-specific window
#endif

	uint width = 0;
	uint height = 0;
	uint posX = 0;
	uint posY = 0;

	void CreateProgramWindow(const char* name, int W, int H, int startX, int startY, uint style, bool showWindow);

	bool SwapBackbuffer();

	SWindow();
	~SWindow();
};

void ProcessCallback(SWindow* window);
void PlatfromLoopUpdate();
void MainPlatformLoop();

//---------------------------------------------------------------------------------------
class CFile{
public:
	enum class EFileMode{
		ReadFormatted,
		ReadBinary,
		WriteFormatted,
		WriteBinary
	};

	CFile(){}
	CFile(std::string name, EFileMode mode){
		Open(name, mode); }
	~CFile(){
		Close(); }

	bool Open(std::string name, EFileMode mode);

	bool ReadFormatted(const char* format, ...);
	bool WriteFormatted(const char* format, ...);

	bool Read(uint size, byte** out_data, uint* out_size);
	bool Write(byte* data, uint size);

	bool isEOF();
	uint getSize();

	void Close();
protected:
	FILE* file = nullptr;
	EFileMode mode = EFileMode::ReadBinary;
	uint size = 0;
};
//---------------------------------------------------------------------------------------
#endif //PLATFORM_H