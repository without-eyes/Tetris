#include <iostream>
#include <cstdio>
#include <windows.h>
#include <thread>
#include <vector>

using namespace std;

wstring tetromino[7]; // Array of tetraminos

// Field Settings
int nFieldWidth = 12;
int nFieldHeight = 18;
unsigned char* pField = nullptr;

// Screen Settings
int nScreenWidth = 80;
int nScreenHeight = 30;

// Function to rotate tetraminps
int Rotate(int px, int py, int r) {
    switch(r % 4) {
        case 0: return 4 * py + px;         // 0 deg
        case 1: return 12 + py - 4 * px;    // 90 deg
        case 2: return 15 - 4 * py - px;    // 180 deg
        case 3: return 3 - py + 4 * px;     // 270 deg
    }
    return 0;
}

// Function to check if piece fit in exact position
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
    pField = new unsigned char[nFieldWidth * nFieldHeight]; // Field variable
    for(int x = 0; x < nFieldWidth; x++) // Board Boundary
        for(int y = 0; y < nFieldHeight; y++)
            pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

    // Creating own handle
    wchar_t *screen = new wchar_t[nScreenWidth * nScreenHeight];
    for(int i = 0; i < nScreenWidth * nScreenHeight; i++)
        screen[i] = L' ';
    HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
    SetConsoleActiveScreenBuffer(hConsole);
    DWORD dwBytesWritten = 0;

    // Game logic staff
    bool bGameOver = false; // Important variable to run and end game

    srand(time(NULL));
    int nCurrentPiece = rand() % 7; // Picking random tetramino from 7 given
    int nCurrentRotation = 0;
    int nCurrentX = nFieldWidth / 2;
    int nCurrentY = 0;

    bool bKey[4]; // Array of keys(row_right, row_left, row_down, z)
    bool bRotateHold = false; // Variable to prevent holding key

    int nSpeed = 20; // Game speed
    int nSpeedCounter = 0; // Variable that makes piece fall or be blocked
    bool bForceDown = false; // Block piece if it's at bottom of field
    int nPieceCount = 0; // Checking piece amount to make game more difficult
    int nScore = 0; // Player Score

    vector<int> vLines;

    while(!bGameOver) {
        // GAME TIMING ===================================
        this_thread::sleep_for(50ms); // Game Tick
        nSpeedCounter++;
        bForceDown = (nSpeedCounter == nSpeed);

        // USER INPUT ====================================
        for(int k = 0; k < 4; k++)                                    // R   L   D  Z
            bKey[k] = (0x8000 & GetAsyncKeyState((unsigned char)("\x27\x25\x28Z"[k]))) != 0;

        // GAME LOGIC ====================================

        // Key Functions
        nCurrentX += (bKey[0] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX + 1, nCurrentY)) ? 1 : 0;
        nCurrentX -= (bKey[1] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX - 1, nCurrentY)) ? 1 : 0;
        nCurrentY += (bKey[2] && DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1)) ? 1 : 0;
        if(bKey[3]) {
            nCurrentRotation += (!bRotateHold && DoesPieceFit(nCurrentPiece, nCurrentRotation + 1, nCurrentX, nCurrentY)) ? 1 : 0;
            bRotateHold = true;
        } else {
            bRotateHold = false;
        }

        // Piece Movement/Block
        if(bForceDown) {
            if(DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY + 1))
                nCurrentY++; // Moving piece down if it can go down
            else {
                // Lock the current piece into the field
                for (int px = 0; px < 4; px++)
                    for (int py = 0; py < 4; py++)
                        if (tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
                            pField[(nCurrentY + py) * nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 1;

                // More piece amount - more difficult to play
                nPieceCount++;
                if(nPieceCount % 10 == 0)
                    if(nSpeed >= 10)
                        nSpeed--;

                // Check have we got any horizontal lines
                for(int py = 0; py < 4; py++) {
                    if(nCurrentY + py < nFieldHeight - 1) {
                        bool bLine = true;

                        // Cheking if all row cells are occupied
                        for(int px = 1; px < nFieldWidth - 1; px++)
                            bLine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

                        if(bLine) {
                            // Remove Line, set to =
                            for(int px = 1; px < nFieldWidth - 1; px++)
                                pField[(nCurrentY + py) * nFieldWidth + px] = 8; // 8 is '=' in our game

                            vLines.push_back(nCurrentY + py); // Adding lines in vec for later processing
                        }
                    }
                }

                // Score System
                nScore += 25;
                if(!vLines.empty())
                    nScore += (1 << vLines.size()) * 100; // More lines - more score points

                // Choose next piece
                nCurrentX = nFieldWidth / 2;
                nCurrentY = 0;
                nCurrentRotation = 0;
                nCurrentPiece = rand() % 7;

                // Game over if piece does not fit anymore
                bGameOver = !DoesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
            }

            // Resetting speed counter
            nSpeedCounter = 0;
        }

        // RENDER OUTPUT =========================================

        // Drawing Field
        for(int x = 0; x < nFieldWidth; x++)
            for(int y = 0; y < nFieldHeight; y++)
                screen[(y + 2) * nScreenWidth + (x + 2)] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]]; // using chars from string to display different things

        // Draw Current Piece
        for(int px = 0; px < 4; px++)
            for(int py = 0; py < 4; py++)
                if(tetromino[nCurrentPiece][Rotate(px, py, nCurrentRotation)] != L'.')
                    screen[(nCurrentY + py + 2) * nScreenWidth + (nCurrentX + px + 2)] = nCurrentPiece + 65; // 65 - letter 'A' in ASCII

        // Draw Score
        swprintf_s(&screen[2 * nScreenWidth + nFieldWidth + 6], 16, L"SCORE: %8d", nScore);

        // Shifting the upper row instead of the lower completed one
        if(!vLines.empty()) {
            // Display Frame (cheekily to draw lines)
            WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, {0, 0}, &dwBytesWritten);
            this_thread::sleep_for(400ms); // Delay a bit

            for(auto &v : vLines) {
                for(int px = 1; px < nFieldWidth - 1; px++) {
                    for(int py = v; py > 0; py--)
                        pField[py * nFieldWidth + px] = pField[(py - 1) * nFieldWidth + px];
                    pField[px] = 0;
                }
            }

            // Clearing arrays of used completed rows
            vLines.clear();
        }

        // Displaying Frame
        WriteConsoleOutputCharacterW(hConsole, screen, nScreenWidth * nScreenHeight, {0, 0}, &dwBytesWritten);
    }

    // Game Results
    CloseHandle(hConsole);
    cout << "\t*** Game Over!!! ***\n\n\tScore: " << nScore << "\n\n\n";
    system("pause");

    return 0;
}
