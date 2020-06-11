#include <list>
#include "platform.h"
#include "pointers.h"
#include "log.h"

timetype chrono_clock_start = getTime_c();

std::list<SWindow*> windows;

SWindow::SWindow(){
	windows.emplace_back(this);
#ifdef PLATFORM_WINDOWS
	memset(&windowClass, 0, sizeof(WNDCLASS));
#endif
}
SWindow::~SWindow(){
	windows.remove(this);
}
//==========================================================================================

#ifdef PLATFORM_WINDOWS

#include <windowsx.h>

/*#include <time.h>

uint64 queryPerformanceCounter(){
	LARGE_INTEGER time; QueryPerformanceCounter(&time); return time.QuadPart;  }
uint64 queryPerformanceFrequency(){
	LARGE_INTEGER freq; QueryPerformanceFrequency(&freq); return freq.QuadPart; }
	/*
	auto start = time(nullptr);
	auto qpc_start = queryPerformanceCounter();
	auto qpc_end = queryPerformanceCounter();
	while(start == time(nullptr)){
		qpc_end = queryPerformanceCounter(); }
	return qpc_end - qpc_start;*/
/*}

const uint64 qpc_start = queryPerformanceCounter();
const uint64 qpc_frequency = queryPerformanceFrequency();

double getTime�s(){
	uint64 current = queryPerformanceCounter() - qpc_start;
	return ((double)current) * ((1.0) / ((double)qpc_frequency));
}
float getTimems(){
	return getTime�s();
}
*/

//==========================================================================================

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{		//PAINTSTRUCT paints;
		//HDC hdc;
	return DefWindowProc(hWnd, msg, wParam, lParam);
	int i = 1;

	SWindow* pWindow = NULL;

	for(auto it = windows.begin(); it != windows.end(); ++it)
		if((*it)->getWindowHandle() == hWnd){
			pWindow = *it; break;
		}

	if(pWindow == NULL)
		return DefWindowProc(hWnd, msg, wParam, lParam);

	switch(msg){

		case WM_CREATE:
		case WM_NCCREATE:
		{
			return DefWindowProc(pWindow->getWindowHandle(), msg, wParam, lParam);
			break;
		}

		case WM_NCHITTEST:
		{//WM_NCHITTEST//0x0084
			signed short x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
			LRESULT rezultat = DefWindowProc(pWindow->getWindowHandle(), msg, wParam, lParam);
			switch(rezultat){
				case HTBORDER:{
					//printf("HTBORDER x,y = %d,%d\n",x,y);
					break;
				}
				case HTBOTTOM:{
					//printf("HTBOTTOM x,y = %d,%d\n",x,y);
					break;
				}
				case HTBOTTOMLEFT:{
					//printf("HTBOTTOMLEFT x,y = %d,%d\n",x,y);
					break;
				}
				case HTBOTTOMRIGHT:{
					//printf("HTBOTTOMRIGHT x,y = %d,%d\n",x,y);
					break;
				}
				case HTCAPTION:{
					//printf("HTCAPTION x,y = %d,%d\n",x,y);
					break;
				}
				case HTCLIENT:{
					return HTCLIENT;
					//printf("HTCLIENT x,y = %d,%d\n",x,y);
					break;
				}
				case HTERROR:{
					//printf("HTERROR x,y = %d,%d\n",x,y);
					break;
				}
				//case HTGROWBOX:{
				//			  break;}
				case HTHELP:{
					//printf("HTHELP x,y = %d,%d\n",x,y);
					break;
				}
				case HTHSCROLL:{
					//printf("HTHSCROLL x,y = %d,%d\n",x,y);
					break;
				}
				case HTLEFT:{
					//printf("HTLEFT x,y = %d,%d\n",x,y);
					break;
				}
				case HTMENU:{
					//printf("HTMENU x,y = %d,%d\n",x,y);
					break;
				}
				case HTMAXBUTTON:{
					//printf("HTMAXBUTTON x,y = %d,%d\n",x,y);
					break;
				}
				case HTMINBUTTON:{
					//printf("HTMINBUTTON x,y = %d,%d\n",x,y);
					break;
				}
				case HTNOWHERE:{
					//printf("HTNOWHERE x,y = %d,%d\n",x,y);
					break;
				}
				//case HTREDUCE:{
				//			  break;}
				case HTRIGHT:{
					//printf("HTRIGHT x,y = %d,%d\n",x,y);
					break;
				}
				case HTSIZE:{
					//printf("HTSIZE x,y = %d,%d\n",x,y);
					break;
				}
				case HTSYSMENU:{
					//printf("HTSYSMENU x,y = %d,%d\n",x,y);
					break;
				}
				case HTTOP:{
					//printf("HTTOP x,y = %d,%d\n",x,y);
					break;
				}
				case HTTOPLEFT:{
					//printf("HTTOPLEFT x,y = %d,%d\n",x,y);
					break;
				}
				case HTTOPRIGHT:{
					//printf("HTTOPRIGHT x,y = %d,%d\n",x,y);
					break;
				}
				case HTTRANSPARENT:{
					//printf("HTTRANSPARENT x,y = %d,%d\n",x,y);
					break;
				}
				case HTVSCROLL:{
					//printf("HTVSCROLL x,y = %d,%d\n",x,y);
					break;
				}
				//case HTZOOM:{
				//			  break;}
			}
			return rezultat;
		}
		case WM_DESTROY:
		{
			//OnExitFunction();
			PostQuitMessage(0);
			return 0;
			break;
		}

		default:
		{
			//pWindow->ProcessWndProcMessage(hWnd, msg, wParam, lParam);
			return DefWindowProc(hWnd, msg, wParam, lParam);
		}
	}
	return 1;
}

void SWindow::CreateProgramWindow(const char* c_name, int W, int H, int startX, int startY, uint style, bool showWindow){

	width = W; height = H; posX = startX; posY = startY;

	const char* class_name = "rdWindow";
	hInstance = GetModuleHandle(NULL);

	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hInstance = hInstance;
	windowClass.lpfnWndProc = WndProc;
	windowClass.lpszClassName = class_name;//TEXT("");
	windowClass.lpszMenuName = 0;
	windowClass.style = flags;//CS_HREDRAW | CS_VREDRAW | CS_OWNDC;// | WS_CLIPCHILDREN | WS_CLIPSIBLINGS;

	RegisterClass(&windowClass);

	size_t nameLen = strlen(c_name);
	this->name = c_name;

	this->window = CreateWindow((LPCSTR)class_name, (LPCSTR)c_name, WS_OVERLAPPEDWINDOW, posX, posY,//450,
								width+16, height+39, NULL, NULL, hInstance, NULL);
	
	this->windowDeviceContext = GetDC(this->window);

	UpdateWindow(this->window);

	ShowWindow(this->window, showWindow);

	/**/RECT rect; GetClientRect(this->window, &rect);
	long w = rect.right; long h = rect.bottom;
	width = w;
	height = h;
}

bool SWindow::SwapBackbuffer(){
	return SwapBuffers(this->windowDeviceContext);
}

void ProcessMessage(SWindow* window){

}

bool ListenForMessage(SWindow* window)
{
	MSG msg;

	//int getmessage = GetMessage(&msg,NULL,0,0);
	bool getmessage = PeekMessage(&msg, window->getWindowHandle(), NULL, NULL, PM_REMOVE) == 1;
	if(getmessage == false)
		return false;
	
	TranslateMessage(&msg);
	DispatchMessage(&msg);

	return true;
}

#endif //PLATFORM_WINDOWS

#ifdef PLATFORM_EMSCRIPTEN

void CreateProgramWindow(const char* name, int W, int H, int startX, int startY, uint style, bool showWindow){
	LOG_ERR("not implemented");
}

void ProcessMessage(SWindow* window){}

bool ListenForMessage(SWindow* window){
	return false; }

#endif //PLATFORM_EMSCRIPTEN

void ProcessCallback(SWindow* window)
{
	if(ListenForMessage(window) == true)
		ProcessMessage(window);
}

void PlatfromLoopUpdate(){
	for(auto it = windows.begin(); it != windows.end(); ++it)
		ProcessCallback(*it);
}

void MainPlatformLoop(){
	while(true){
		PlatfromLoopUpdate();
	}
}




const char* charmode(CFile::EFileMode mode){
	switch(mode)
	{
		case CFile::EFileMode::ReadFormatted: return "r";
		case CFile::EFileMode::ReadBinary: return "rb";
		case CFile::EFileMode::WriteFormatted: return "w";
		case CFile::EFileMode::WriteBinary: return "wb";
	}
	return "";
}

//---------------------------------------------------------------------------------------
bool CFile::Open(std::string name, CFile::EFileMode mode){
	if(file != nullptr) return false;
	fopen_s(&file, name.c_str(), charmode(mode));
	if(file == nullptr) return false;
	this->mode = mode;
	this->getSize();
	return true;
}

bool CFile::ReadFormatted(const char* format, ...){
	return false;
}
bool CFile::WriteFormatted(const char* format, ...){
	return false;
}

bool CFile::Read(uint size, byte* out_data, uint* out_size){
	if(file == nullptr) return false;
	uint read = (uint)fread_s(out_data, size, 1, size, file);
	if(out_size != nullptr) *out_size = read;
	return read == size;
}
bool CFile::Write(byte* data, uint size){
	if(file == nullptr) return false;
	return fwrite(data, size, 1, file) != 0;
}

bool CFile::isEOF(){
	if(file == nullptr) return true;
	return feof(file) != 0;
}

uint CFile::getSize(){
	if(file == nullptr) return 0;
	if(size != 0) return size;

	auto pos = ftell(file);
	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, pos, SEEK_SET);

	return size;
}
uint CFile::getPosition(){
	if(file == nullptr) return 0;
	return ftell(file);
}
uint CFile::getRemaining(){
	if(file == nullptr) return 0;
	auto pos = this->getPosition();
	return size - pos;
}

void CFile::Close(){
	if(file != nullptr){
		fclose(file); file = nullptr;
	}
}
//---------------------------------------------------------------------------------------
//==========================================================================================


