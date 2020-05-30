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
	//ToDo: dodat delayed loading i delayed izvrsavanje calling funkcije (koja je pozvala CFile::DelayedOpen().
	//UniquePtr<CDelayedExec> DelayedOpen(std::string name, EFileMode mode, );

	bool ReadFormatted(const char* format, ...);
	bool WriteFormatted(const char* format, ...);

	bool Read(uint size, byte* out_data, uint* out_size = nullptr);
	bool Write(byte* data, uint size);

	template<typename type>
	bool Read(uint count, type* out_data, uint* out_count = nullptr);
	template<typename type>
	bool Write(type* data, uint count);

	bool isEOF();
	uint getSize();
	uint getRemaining();
	uint getPosition();
	bool isOpen(){ return file != nullptr; }

	void Close();
protected:
	FILE* file = nullptr;
	EFileMode mode = EFileMode::ReadBinary;
	uint size = 0;
};

template<typename type>
bool CFile::Read(uint count, type* out_data, uint* out_count){
	byte* ptr = (byte*)out_data;
	bool r = this->Read(count*sizeof(type), ptr, out_count);
	if(out_count != nullptr) *out_count /= sizeof(type);
	return r;
}

template<typename type>
bool CFile::Write(type* data, uint count){
	return Write((byte*)data, count*sizeof(type));
}
//---------------------------------------------------------------------------------------

#include <chrono>

typedef std::chrono::high_resolution_clock::time_point timetype;

extern timetype chrono_clock_start;
inline timetype getTime_c(){ return std::chrono::high_resolution_clock::now(); }
inline double getTime_s(){ //µs
	std::chrono::duration<double> diff = std::chrono::duration_cast<std::chrono::duration<double>>(getTime_c() - chrono_clock_start); return diff.count(); }

float getTime_ms();

//---------------------------------------------------------------------------------------
#endif //PLATFORM_H