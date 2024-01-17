#include <windows.h>

// static here means that it is a global variable. this also defaults it to 0, no need to initialize it
// TODO: global variable temporarily
static bool running;
static BITMAPINFO bitmapInfo;
static void *bitmapMemory;
static HBITMAP bitmapHandle;
static HDC bitmapDeviceContext;
// static keyword on a function means that they can only be called within this file

static void
resizeDIBSection(int width, int height)
{
	// TODO: dont free automatically, maybe free after, then free first if that fails

	if (bitmapHandle)
	{
		DeleteObject(bitmapHandle);
	}
	if (!bitmapDeviceContext)
	{
		bitmapDeviceContext = CreateCompatibleDC(0);
	}

	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biWidth = width;
	bitmapInfo.bmiHeader.biHeight = height;
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	bitmapHandle = CreateDIBSection(
		bitmapDeviceContext,
		&bitmapInfo,
		DIB_RGB_COLORS,
		&bitmapMemory,
		0,
		0);
}

static void windowUpdate(HDC deviceContext, int x, int y, int width, int height)
{
	StretchDIBits(
		deviceContext,
		// window we are drawing to:
		x,
		y,
		width,
		height,
		// window we are drawing from:
		x,
		y,
		width,
		height,
		bitmapMemory,
		&bitmapInfo,
		DIB_RGB_COLORS,
		SRCCOPY);
}

LRESULT CALLBACK windowCallback(
	HWND windowHandle,
	UINT message,
	WPARAM wParam,
	LPARAM lParam)
{
	LRESULT result = 0;

	switch (message)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT paint;
		HDC deviceContext = BeginPaint(windowHandle, &paint);
		int x = paint.rcPaint.left;
		int y = paint.rcPaint.top;
		int height = paint.rcPaint.bottom - paint.rcPaint.top;
		int width = paint.rcPaint.right - paint.rcPaint.left;
		windowUpdate(deviceContext, x, y, width, height);
		EndPaint(windowHandle, &paint);
	}
	break;
	case WM_SIZE:
	{
		RECT clientRect;
		GetClientRect(windowHandle, &clientRect);
		int height = clientRect.bottom - clientRect.top;
		int width = clientRect.right - clientRect.left;
		resizeDIBSection(width, height);
	}
	break;
	case WM_DESTROY:
	{
		OutputDebugStringA("WM_DESTROY\n");
		// TODO: might want to handle this as an error, so i can recreate window
	}
	break;
	case WM_CLOSE:
	{
		running = false;
		OutputDebugStringA("WM_CLOSE\n");
		// TODO: window pop-up "are you sure you want to close the application?"
	}
	break;
	case WM_ACTIVATEAPP:
	{
		OutputDebugStringA("WM_ACTIVATEAPP\n");
	}
	break;
	default:
	{
		result = DefWindowProc(windowHandle, message, wParam, lParam);
	}
	break;
	}

	return result;
};

int CALLBACK WinMain(
	HINSTANCE instance,
	HINSTANCE prevInstance,
	PSTR cmdline,
	int cmdshow)
{
	WNDCLASSA windowClass{};
	windowClass.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW;
	windowClass.lpfnWndProc = windowCallback;
	windowClass.hInstance = instance;
	windowClass.lpszClassName = "TedzukuriWindowClass";

	if (RegisterClassA(&windowClass))
	{
		HWND windowHandle = CreateWindowExA(
			0,
			windowClass.lpszClassName,
			"Tedzukuri",
			WS_OVERLAPPEDWINDOW | WS_VISIBLE,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			CW_USEDEFAULT,
			0,
			0,
			instance,
			0);

		if (windowHandle)
		{
			running = true;
			MSG message;
			while (running)
			{
				BOOL messageResult = GetMessage(&message, 0, 0, 0);
				if (messageResult > 0)
				{
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
				else
				{
					break;
				}
			}
		}
		else
		{
			// TODO: Logging and error system
		}
	}
	else
	{
		// TODO: Logging and error system
	}

	return 0;
}
