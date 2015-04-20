#include <Windows.h>
#include <stdio.h>
#include <string.h>

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

//using namespace std;
//#define GHND ( GMEM_MOVABLE | GMEM_ZEROINIT )
HWND hwnd;
MSG Komunikat;
HWND g_hPrzycisk_Save;
HWND g_hPrzycisk_Load;
HWND g_Text;
HWND hwnd_clip;

void setText(char *text)
{
	HGLOBAL hGIMem;
	char *lpGIMem;
	int size = strlen(text);
	hGIMem = GlobalAlloc(GHND, (DWORD)size + 1);
	lpGIMem = GlobalLock(hGIMem);
	//SetWindowText(g_Text, "");
	int i;
	for (i = 0; i < size; i++)
	{
		*lpGIMem++ = *text++;
	}

	//SetWindowText(g_Text, lpGIMem);

	GlobalUnlock(hGIMem);
	OpenClipboard(hwnd_clip);
	EmptyClipboard();
	SetClipboardData(CF_TEXT, hGIMem);
	CloseClipboard();
}

void getText()
{
	HGLOBAL hProgMem;
	HANDLE hCbMem;
	char *lpCbMem, *lpProgMem;
	if (IsClipboardFormatAvailable(CF_TEXT))
	{
		OpenClipboard(hwnd_clip);
		hCbMem = GetClipboardData(CF_TEXT);
		//SetWindowText(g_Text, hCbMem);

		if (hCbMem == NULL)
		{
			CloseClipboard();
			printf("Error getText level #1 if");
			return;
		}
		hProgMem = GlobalAlloc(GHND, GlobalSize(hCbMem));

		if (hProgMem != NULL)
		{
			lpCbMem = GlobalLock(hCbMem);
			lpProgMem = GlobalLock(hProgMem);
			strcpy(lpProgMem, lpCbMem);
			GlobalUnlock(hCbMem);
			GlobalUnlock(hProgMem);
			SetWindowText(g_Text, lpProgMem);
			CloseClipboard();
			return;
		}
		CloseClipboard();
	}
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		//ChangeClipboardChain(hwnd, hwndNxtVwr);
		PostQuitMessage(0);
		break;

	case WM_COMMAND:
		if ((HWND)lParam == g_hPrzycisk_Save)
		{
			DWORD dlugosc = GetWindowTextLength(g_Text);
			LPSTR Bufor = (LPSTR)GlobalAlloc(GPTR, dlugosc + 1);
			GetWindowText(g_Text, Bufor, dlugosc + 1);
			setText(Bufor);
			//getText();
			//MessageBox(hwnd, "Copied", "Ha!", MB_ICONINFORMATION);
		}
		if ((HWND)lParam == g_hPrzycisk_Load)
		{
			//setText(Bufor);
			getText();
			//SetWindowText(g_Text, hwnd_clip);
		}
		break;
	

	default:
		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
	return 0;
	
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	LPSTR NazwaKlasy = "Klasa Okienka";

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = 0;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = NazwaKlasy;
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, "Wysoka Komisja odmawia rejestracji tego okna!", "Niestety...",
			MB_ICONEXCLAMATION | MB_OK);
		return 1;
	}
	
	hwnd = CreateWindowEx(WS_EX_CLIENTEDGE, NazwaKlasy, "Clipboard", WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, 600, 500, NULL, NULL, hInstance, NULL);

	g_hPrzycisk_Save = CreateWindowEx(0, "BUTTON", "Save", WS_CHILD | WS_VISIBLE,
		300, 400, 150, 30, hwnd, NULL, hInstance, NULL);

	g_hPrzycisk_Load = CreateWindowEx(0, "BUTTON", "Load", WS_CHILD | WS_VISIBLE,
		100, 400, 150, 30, hwnd, NULL, hInstance, NULL);

	g_Text = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", NULL, WS_CHILD | WS_VISIBLE | WS_BORDER, 150, 50, 250, 220,
		hwnd, NULL, hInstance, NULL);

	//print(Bufor);


	if (hwnd == NULL)
	{
		MessageBox(NULL, "Error", "Error hwnd", MB_ICONEXCLAMATION);
		return 1;
	}

	ShowWindow(hwnd, nCmdShow);
	UpdateWindow(hwnd);

	while (GetMessage(&Komunikat, NULL, 0, 0))
	{
		TranslateMessage(&Komunikat);
		DispatchMessage(&Komunikat);
	}

	return Komunikat.wParam;
}