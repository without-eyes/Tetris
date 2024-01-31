#include <iostream>
#include <stdio.h>
#include <windows.h>
#include <thread>
#include <vector>

#define UNICODE
#define _UNICODE

using namespace std;

wstring tetromino[7];

int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char* pField = nullptr;

int nScreenWidth = 80;
int nScreenHeight = 30;

int Rotate(int px, int py, int r) {
    switch(r % 4) {
        case 0: return 4 * py + px;         // 0 deg
        case 1: return 12 + py - 4 * px;    // 90 deg
        case 2: return 15 - 4 * py - px;    // 180 deg
        case 3: return 3 - py + 4 * px;     // 270 deg
    }
    return 0;
}

bool DoesPieceFit(int nTetramino, int nRotation, int nPosX, int nPosY) {
    for(int px = 0; px < 4; px++) {
        for(int py = 0 ; py < 4; py++){
            // Get index into piece
            int pi = Rotate(px, py, nRotation);

            // Get index into field
            int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

            if(nPosX + px >= 0 && nPosX + px < nFieldWidth) {
                if(nPosY + py >= 0 && nPosY + py < nFieldHeight) {
                    if(tetromino[nTetramino][pi] == L'X' && pField[fi] != 0)
                        return false;
                }
            }
        }
    }

    return true;
}

int main() {
    // Creating assets
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");
    tetromino[0].append(L"..X.");

    tetromino[1].append(L"..X.");
    tetromino[1].append(L".XX.");
    tetromino[1].append(L".X..");
    tetromino[1].append(L"....");

    tetromino[2].append(L".X..");
    tetromino[2].append(L".XX.");
    tetromino[2].append(L"..X.");
    tetromino[2].append(L"....");

    tetromino[3].append(L"....");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L".XX.");
    tetromino[3].append(L"....");

    tetromino[4].append(L"..X.");
    tetromino[4].append(L".XX.");
    tetromino[4].append(L"..X.");
    tetromino[4].append(L"....");

    tetromino[5].append(L"....");
    tetromino[5].append(L".XX.");
    tetromino[5].append(L"..X.");
    tetromino[5].append(L"..X.");

    tetromino[6].append(L"....");
    tetromino[6].append(L".XX.");
    tetromino[6].append(L".X..");
    tetromino[6].append(L".X..");

    // Creating play field buffer
    pField = new unsigned char[nFieldWidth * nFieldHeight];
    for(int x = 0; x < nFieldWidth; x++) // Board Boundary
        for(int y = 0; y < nFieldHeight; y++)
            pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

    // Creating own console
    wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
    for(int i = 0; i < nScreenWidth * nScreenHeight; i++)
        screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    // Game logic staff
    bool bGameOver = false;

    int nCurrentPiece = 0;
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth / 2;
    int nCurrentY = 0;

    bool bKey[4];

    while(!bGameOver) {
        // Game Timing
        this_thread::sleep_for(50ms);

        // Input
        for(int k = 0; k < 4; k++)                                    // R   L   D  Z
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

        // Game Logic
        nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
        nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
        nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;

        nCurrentRotation += (bKey[3] && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;

        // Render Output

        // Drawing Field
        for(int x = 0; x < nFieldWidth; x++)
            for(int y = 0; y < nFieldHeight; y++)
                screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]]; // using chars from string to display different things

        // Draw Current Piece
        for(int px = 0; px < 4; px++)
            for(int py = 0; py < 4; py++)
                if(tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] == L'X')
                    screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65; // 65 - letter 'A' in ASCII

        // Displaying Frame
        WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, {0, 0}, &dwBytesWritten);
    }

    return 0;
}
