#include <windows.h>
#include <stdint.h>
#include <dsound.h>
#include <string>
#include <math.h>

#define pi32 3.14159265359f

struct OffscreenBuffer
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
struct WindowDimensions
{
	int width;
	int height;
};
struct SoundOutput
{
	int samplesPerSecond;
	int toneHz;
	int wavePeriod;
	int bytesPerSample;
	uint32_t runningSampleIndex;
	int secondaryBufferSize;
	int toneVolume;
};

static bool globalRunningFlag;
static OffscreenBuffer globalBitmapBackbuffer;
static LPDIRECTSOUNDBUFFER globalSecondaryAudioBuffer;

static void initDirectSound(HWND windowHandle, int32_t bufferSize, int32_t samplesPerSecond) // NOTE: I AM making it mandatory to have the library in order to run the program, handmade hero doesnt
{
	LPDIRECTSOUND directSound;

	if (SUCCEEDED(DirectSoundCreate(0, &directSound, 0)))
	{
		WAVEFORMATEX waveFormat = {};
		waveFormat.wFormatTag = WAVE_FORMAT_PCM;
		waveFormat.nChannels = 2;
		waveFormat.nSamplesPerSec = samplesPerSecond;
		waveFormat.wBitsPerSample = 16;
		waveFormat.nBlockAlign = (waveFormat.nChannels * waveFormat.wBitsPerSample) / 8;
		waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
		waveFormat.cbSize = 0;

		if (SUCCEEDED(directSound->SetCooperativeLevel(windowHandle, DSSCL_PRIORITY)))
		{
			DSBUFFERDESC bufferDescription = {};
			bufferDescription.dwSize = sizeof(bufferDescription);
			bufferDescription.dwFlags = DSBCAPS_PRIMARYBUFFER;

			LPDIRECTSOUNDBUFFER primaryAudioBuffer;
			if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &primaryAudioBuffer, 0)))
			{
				if (SUCCEEDED(primaryAudioBuffer->SetFormat(&waveFormat)))
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

		if (SUCCEEDED(directSound->CreateSoundBuffer(&bufferDescription, &globalSecondaryAudioBuffer, 0)))
		{
			OutputDebugStringA("Secondary buffer created\n");
		}
	}
	else
	{
		// TODO: Logging and error system
	}
}
WindowDimensions getWindowDimensions(HWND windowHandle)
{
	RECT clientRect;
	GetClientRect(windowHandle, &clientRect);

	WindowDimensions result;
	result.height = clientRect.bottom - clientRect.top;
	result.width = clientRect.right - clientRect.left;

	return result;
}
static void render(OffscreenBuffer *buffer, int blueOffset, int greenOffset)
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
static void resizeDIBSection(OffscreenBuffer *buffer, int width, int height)
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
static void displayBufferInWindow(HDC deviceContext, int width, int height, OffscreenBuffer *buffer, int x, int y)
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
			globalRunningFlag = false;
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT paint;
		HDC deviceContext = BeginPaint(windowHandle, &paint);

		WindowDimensions dimensions = getWindowDimensions(windowHandle);

		int x = paint.rcPaint.left;
		int y = paint.rcPaint.top;
		displayBufferInWindow(deviceContext, dimensions.width, dimensions.height, &globalBitmapBackbuffer, x, y);
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
		globalRunningFlag = false;
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

static void fillSoundBuffer(SoundOutput *soundOutput, DWORD byteToLock, DWORD bytesToWrite)
{
	void *region1;
	DWORD region1Size;
	void *region2;
	DWORD region2Size;

	HRESULT lockResult = globalSecondaryAudioBuffer->Lock(byteToLock, bytesToWrite,
														  &region1, &region1Size,
														  &region2, &region2Size,
														  0);

	if (SUCCEEDED(lockResult))
	{
		OutputDebugStringA("Lock worked\n");

		DWORD region1SampleCount = region1Size / soundOutput->bytesPerSample;
		int16_t *sampleOut = (int16_t *)region1;

		for (DWORD sampleIndex = 0; sampleIndex < region1SampleCount; ++sampleIndex)
		{
			float position = 2.0f * pi32 * (float)soundOutput->runningSampleIndex / (float)soundOutput->wavePeriod;
			float sineValue = sinf(position);									  // returns a number between 1 and -1
			int16_t sampleValue = (int16_t)(sineValue * soundOutput->toneVolume); // scales the sine up to the volume
			*sampleOut++ = sampleValue;
			*sampleOut++ = sampleValue;

			++soundOutput->runningSampleIndex;
		}
		sampleOut = (int16_t *)region2;
		DWORD region2SampleCount = region2Size / soundOutput->bytesPerSample;

		for (DWORD sampleIndex = 0; sampleIndex < region2SampleCount; ++sampleIndex)
		{
			float position = 2.0f * pi32 * (float)soundOutput->runningSampleIndex / (float)soundOutput->wavePeriod;
			float sineValue = sinf(position);
			int16_t sampleValue = (int16_t)(sineValue * soundOutput->toneVolume);
			*sampleOut++ = sampleValue;
			*sampleOut++ = sampleValue;

			++soundOutput->runningSampleIndex;
		}
		globalSecondaryAudioBuffer->Unlock(region1, region1Size, region2, region2Size);
	}
	else
	{
		OutputDebugStringA("Lock failed with HRESULT: ");
		OutputDebugStringA(std::to_string(lockResult).c_str());
		OutputDebugStringA("\n");
	}
}

int CALLBACK WinMain(
	HINSTANCE instance,
	HINSTANCE prevInstance,
	PSTR cmdline,
	int cmdshow)
{
	WNDCLASSA windowClass = {};

	resizeDIBSection(&globalBitmapBackbuffer, 1280, 720);

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
			HDC deviceContext = GetDC(windowHandle);

			int xOffset = 0;
			int yOffset = 0;

			SoundOutput soundOutput = {};
			soundOutput.samplesPerSecond = 48000;
			soundOutput.toneHz = 256;
			soundOutput.toneVolume = 3000;
			soundOutput.runningSampleIndex = 0;
			soundOutput.wavePeriod = soundOutput.samplesPerSecond / soundOutput.toneHz;
			soundOutput.bytesPerSample = sizeof(int16_t) * 2;
			soundOutput.secondaryBufferSize = soundOutput.samplesPerSecond * soundOutput.bytesPerSample;

			initDirectSound(windowHandle, soundOutput.secondaryBufferSize, soundOutput.samplesPerSecond);

			fillSoundBuffer(&soundOutput, 0, soundOutput.secondaryBufferSize);

			globalSecondaryAudioBuffer->Play(0, 0, DSBPLAY_LOOPING);

			globalRunningFlag = true;

			while (globalRunningFlag)
			{
				MSG message;
				while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
				{
					if (message.message == WM_QUIT)
					{
						globalRunningFlag = false;
					}
					TranslateMessage(&message);
					DispatchMessage(&message);
				}
				render(&globalBitmapBackbuffer, xOffset, yOffset);

				// AUDIO

				DWORD playCursor;
				DWORD writeCursor;

				if (SUCCEEDED(globalSecondaryAudioBuffer->GetCurrentPosition(&playCursor, &writeCursor)))
				{
					DWORD byteToLock = ((soundOutput.runningSampleIndex * soundOutput.bytesPerSample) %
										soundOutput.secondaryBufferSize);
					DWORD bytesToWrite;

					if (byteToLock == playCursor)
					{
						bytesToWrite = 0;
					}
					else if (byteToLock > playCursor)
					{
						bytesToWrite = (soundOutput.secondaryBufferSize - byteToLock);
						bytesToWrite += playCursor;
					}
					else
					{
						bytesToWrite = playCursor - byteToLock;
					}
					fillSoundBuffer(&soundOutput, byteToLock, bytesToWrite);
				}
				else
				{
					OutputDebugStringA("GetCurrentPosition failed\n");
				}
				WindowDimensions dimensions = getWindowDimensions(windowHandle);
				displayBufferInWindow(deviceContext, dimensions.width, dimensions.height, &globalBitmapBackbuffer, 0, 0);

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
