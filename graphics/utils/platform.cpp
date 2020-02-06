#include <list>
#include "platform.h"

std::list<SWindow*> windows;

SWindow::SWindow(){
	windows.emplace_back(this);
}
SWindow::~SWindow(){
	windows.remove(this);
}

//==========================================================================================
#ifdef PLATFORM_WINDOWS

#include <windowsx.h>

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

void SWindow::CreateProgramWindow(const char* name, int W, int H, int startX, int startY, uint style, bool showWindow){

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

	size_t nameLen = strlen(name);
	this->name = new char[nameLen+1];
	strcpy_s(this->name, nameLen+1, name);

	this->window = CreateWindow((LPCSTR)class_name, (LPCSTR) name, WS_OVERLAPPEDWINDOW, posX, posY,//450,
								width, height, NULL, NULL, hInstance, NULL);
	
	this->windowDeviceContext = GetDC(this->window);

	UpdateWindow(this->window);

	ShowWindow(this->window, showWindow);

	RECT rect; GetClientRect(this->window, &rect);
	long w = rect.right; long h = rect.bottom;
	width = w;
	height = h;
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
void ProcessCallback(SWindow* window)
{
	if(ListenForMessage(window) == true)
		ProcessMessage(window);
}

void MainPlatformLoop(){
	while(true){
		for(auto it = windows.begin(); it != windows.end(); ++it)
			ProcessCallback(*it);
	}
}

#endif //PLATFORM_WINDOWS
//==========================================================================================
