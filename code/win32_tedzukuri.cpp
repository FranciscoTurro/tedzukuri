#include <windows.h>
#include <stdint.h>
// static here means that it is a global variable. this also defaults it to 0, no need to initialize it
// TODO: global variable temporarily
static bool running;
static BITMAPINFO bitmapInfo;
static void *bitmapMemory;
static int bitmapWidth;
static int bitmapHeight;
static int bytesPerPixel = 4; // we NEED 3 bytes per pixel because of RGB (1 byte for red, 1 for green, 1 for blue) but we add an
							  // extra one for padding, to ensure that all of our bytes are aligned on 4 bytes boundaries.
							  // this is because if we are working on 32 bits (we are), bits need to be aligned on 32 bit boundaries

// static keyword on a function means that they can only be called within this file
static void render(int xOffset, int yOffset)
{
	int pitch = bitmapWidth * bytesPerPixel; // difference between a row and the next row
	uint8_t *row = (uint8_t *)bitmapMemory;
	for (int y = 0; y < bitmapHeight; ++y)
	{
		uint32_t *pixel = (uint32_t *)row;
		for (int x = 0; x < bitmapWidth; ++x)
		{
			uint8_t green = (y + yOffset);
			uint8_t blue = (x + xOffset);

			*pixel++ = (green << 8 | blue);
		}
		row += pitch;
	}
}
static void resizeDIBSection(int width, int height)
{
	// TODO: dont free automatically, maybe free after, then free first if that fails

	if (bitmapMemory)
	{
		VirtualFree(bitmapMemory, 0, MEM_RELEASE);
	}

	bitmapWidth = width;
	bitmapHeight = height;

	bitmapInfo.bmiHeader.biSize = sizeof(bitmapInfo.bmiHeader);
	bitmapInfo.bmiHeader.biWidth = bitmapWidth;
	bitmapInfo.bmiHeader.biHeight = -bitmapHeight; //??
	bitmapInfo.bmiHeader.biPlanes = 1;
	bitmapInfo.bmiHeader.biBitCount = 32;
	bitmapInfo.bmiHeader.biCompression = BI_RGB;

	int bitmapMemorySize = (width * height) * bytesPerPixel; // we need enough bits for the area of the rectangle multiplied by 4 (see above why 4)
	bitmapMemory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);
}

static void windowUpdate(HDC deviceContext, RECT *clientRect, int x, int y, int width, int height)
{
	int windowWidth = clientRect->right - clientRect->left;
	int windowHeight = clientRect->bottom - clientRect->top;

	StretchDIBits(
		deviceContext,
		// window we are drawing to:
		0, 0, bitmapWidth, bitmapHeight,
		// window we are drawing from:
		0, 0, windowWidth, windowHeight,
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

		RECT clientRect;
		GetClientRect(windowHandle, &clientRect);

		int x = paint.rcPaint.left;
		int y = paint.rcPaint.top;
		int height = paint.rcPaint.bottom - paint.rcPaint.top;
		int width = paint.rcPaint.right - paint.rcPaint.left;
		windowUpdate(deviceContext, &clientRect, x, y, width, height);
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
			int xOffset = 0;
			int yOffset = 0;
			running = true;
			while (running)
			{
				MSG message;
				while (PeekMessage(&message, 0, 0, 0, PM_REMOVE))
				{
					if (message.message == WM_QUIT)
					{
						running = false;
					}
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
				render(xOffset, yOffset);

				HDC deviceContext = GetDC(windowHandle);
				RECT clientRect;
				GetClientRect(windowHandle, &clientRect);
				int windowHeight = clientRect.bottom - clientRect.top;
				int windowWidth = clientRect.right - clientRect.left;

				windowUpdate(deviceContext, &clientRect, 0, 0, windowWidth, windowHeight);
				ReleaseDC(windowHandle, deviceContext);
				++xOffset;
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
