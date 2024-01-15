#include <windows.h>

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
		static DWORD operation = WHITENESS;

		PatBlt(deviceContext, x, y, width, height, operation);

		if (operation == WHITENESS)
			operation = BLACKNESS;
		else
			operation = WHITENESS;

		EndPaint(windowHandle, &paint);
	}
	break;
	case WM_SIZE:
	{
		OutputDebugStringA("WM_SIZE\n");
	}
	break;
	case WM_DESTROY:
	{
		OutputDebugStringA("WM_DESTROY\n");
	}
	break;
	case WM_CLOSE:
	{
		OutputDebugStringA("WM_CLOSE\n");
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
			MSG message;
			for (;;)
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
