#include <windows.h>

int CALLBACK WinMain(
    HINSTANCE hInst,
    HINSTANCE hInstPrev,
    PSTR cmdline,
    int cmdshow)
{
    MessageBoxA(
        NULL,
        "name is because im a weeb",
        "Tedzukuri",
        MB_OK | MB_ICONINFORMATION);
    return 0;
}
