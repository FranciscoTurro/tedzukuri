#include <windows.h>
#include <stdint.h>
#include <dsound.h>
struct offscreenBuffer
{
	BITMAPINFO info;
	void *memory;
	int width;
	int height;
	int pitch;
	int bytesPerPixel; // we NEED 3 bytes per pixel, 1 byte for red, 1 for green, 1 for blue but we add an
					   // extra one for padding, to ensure that all of our bytes are aligned on 4 bytes boundaries.
					   // this is because if we are working on 32 bits (we are), bits need to be aligned on 32 bit boundaries
};
struct windowDimensions
{
	int width;
	int height;
};

static bool running;
static offscreenBuffer globalBackbuffer;

static void initDirectSound(HWND windowHandle, int32_t bufferSize, int32_t samplesPerSecond) // NOTE: I AM making it mandatory to have the library in order to run the program, handmade hero doesnt
{
	LPDIRECTSOUND directSound;

	if (SUCCEEDED(DirectSoundCreate(0, &directSound, 0)))
	{
		WAVEFORMATEX waveFormat = {};
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nSamplesPerSec = samplesPerSecond;
		waveFormat.cbSize = 0;
		waveFormat.nChannels = 2;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;

		if (SUCCEEDED(directSound->SetCooperativeLevel(windowHandle, DSSCL_PRIORITY)))
		{
			DSBUFFERDESC bufferDescription = {};
			bufferDescription.dwSize = sizeof(bufferDescription);
			bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

			LPDIRECTSOUNDBUFFER primaryBuffer;
			if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &primaryBuffer, 0)))
			{
				if (SUCCEEDED(primaryBuffer->SetFormat(&waveFormat)))
				{
					OutputDebugStringA("Set primary buffer format\n");
				}
				else
				{
					// TODO: Logging and error system
				}
			}
			else
			{
			}
		}
		else
		{
			// TODO: Logging and error system
		}

		DSBUFFERDESC bufferDescription = {};
		bufferDescription.dwSize = sizeof(bufferDescription);
		bufferDescription.dwFlags = 0;
		bufferDescription.dwBufferBytes = bufferSize;
		bufferDescription.lpwfxFormat = &waveFormat;

		LPDIRECTSOUNDBUFFER secondaryBuffer;
		if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &secondaryBuffer, 0)))
		{
			if (SUCCEEDED(secondaryBuffer->SetFormat(&waveFormat)))
			{
				OutputDebugStringA("Set secondary buffer format\n");
			}
			else
			{
			}
		}
		else
		{
		}
	}
	else
	{
		// TODO: Logging and error system
	}
}
windowDimensions getWindowDimensions(HWND windowHandle)
{
	RECT clientRect;
	GetClientRect(windowHandle, &clientRect);

	windowDimensions result;
	result.height = clientRect.bottom - clientRect.top;
	result.width = clientRect.right - clientRect.left;

	return result;
}
static void render(offscreenBuffer *buffer, int blueOffset, int greenOffset)
{
	uint8_t *row = (uint8_t *)buffer->memory;
	for (int y = 0; y < buffer->height; ++y)
	{
		uint32_t *pixel = (uint32_t *)row;
		for (int x = 0; x < buffer->width; ++x)
		{
			uint8_t blue = (x + blueOffset);
			uint8_t green = (y + greenOffset);

			*pixel++ = (green << 8 | blue);
		}
		row += buffer->pitch;
	}
}
static void resizeDIBSection(offscreenBuffer *buffer, int width, int height)
{
	// TODO: dont free automatically, maybe free after, then free first if that fails

	if (buffer->memory)
	{
		VirtualFree(buffer->memory, 0, MEM_RELEASE);
	}

	buffer->width = width;
	buffer->height = height;
	buffer->bytesPerPixel = 4;

	buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
	buffer->info.bmiHeader.biWidth = buffer->width;
	buffer->info.bmiHeader.biHeight = -buffer->height; // negative to indicate to windows that the bitmap is top-down, not
	// bottom up. i had absolutely no chance of knowing this, just count myself lucky that i know it now
	buffer->info.bmiHeader.biPlanes = 1;
	buffer->info.bmiHeader.biBitCount = 32;
	buffer->info.bmiHeader.biCompression = BI_RGB;

	int bitmapMemorySize = (buffer->width * buffer->height) * buffer->bytesPerPixel; // we need enough bits for the area of the rectangle multiplied by 4
	buffer->memory = VirtualAlloc(0, bitmapMemorySize, MEM_COMMIT, PAGE_READWRITE);

	buffer->pitch = buffer->width * buffer->bytesPerPixel; // number of bytes in a row of the bitmap
}
static void displayBufferInWindow(HDC deviceContext, int width, int height, offscreenBuffer *buffer, int x, int y)
{
	// TODO: aspect ratio correction
	StretchDIBits(
		deviceContext,
		// window we are drawing to:
		0, 0, width, height,
		// window we are drawing from:
		0, 0, buffer->width, buffer->height,
		buffer->memory,
		&buffer->info,
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
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
	case WM_KEYDOWN:
	case WM_KEYUP:
	{
		uint32_t virtualKeyCode = wParam;

		bool wasDown = lParam & (1 << 30) != 0; // do the operation on paper to see if you get it https://learn.microsoft.com/en-us/windows/win32/inputdev/wm-keydown
		bool isDown = lParam & (1 << 31) == 0;
		if (wasDown != isDown)
		{
			if (virtualKeyCode == 'W')
			{
			}
			if (virtualKeyCode == 'A')
			{
			}
			if (virtualKeyCode == 'S')
			{
			}
			if (virtualKeyCode == 'D')
			{
			}
			if (virtualKeyCode == 'Q')
			{
			}
			if (virtualKeyCode == 'E')
			{
			}
			if (virtualKeyCode == VK_UP)
			{
			}
			if (virtualKeyCode == VK_LEFT)
			{
			}
			if (virtualKeyCode == VK_RIGHT)
			{
			}
			if (virtualKeyCode == VK_DOWN)
			{
			}
			if (virtualKeyCode == VK_ESCAPE)
			{
			}
			if (virtualKeyCode == VK_SPACE)
			{
			}
		}
		bool altKeyWasDown = ((lParam & (1 << 29)) != 0);
		if (virtualKeyCode == VK_F4 && altKeyWasDown)
		{
			running = false;
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT paint;
		HDC deviceContext = BeginPaint(windowHandle, &paint);

		windowDimensions dimensions = getWindowDimensions(windowHandle);

		int x = paint.rcPaint.left;
		int y = paint.rcPaint.top;
		displayBufferInWindow(deviceContext, dimensions.width, dimensions.height, &globalBackbuffer, x, y);
		EndPaint(windowHandle, &paint);
	}
	break;
	case WM_SIZE:
	{
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
	WNDCLASSA windowClass = {};

	resizeDIBSection(&globalBackbuffer, 1280, 720);

	windowClass.style = CS_HREDRAW | CS_VREDRAW;
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

			initDirectSound(windowHandle, 48000, 48000 * sizeof(int16_t) * 2);

			running = true;
			while (running)
			{
				MSG message;
				while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
				{
					if (message.message == WM_QUIT)
					{
						running = false;
					}
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
				render(&globalBackbuffer, xOffset, yOffset);

				HDC deviceContext = GetDC(windowHandle);

				windowDimensions dimensions = getWindowDimensions(windowHandle);

				displayBufferInWindow(deviceContext, dimensions.width, dimensions.height, &globalBackbuffer, 0, 0);
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
