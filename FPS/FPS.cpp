#include <iostream>
#include <vector>
#include <utility>
#include <algorithm>
#include <chrono>
#include <cmath>

using namespace std;
#include <stdio.h>
#include <Windows.h>

int nScreenWidth = 120;
int nScreenHeight = 40;

float fPlayerX = 8.0f;
float fPlayerY = 8.0f;
float fPlayerA = 0.0f;

int nMapWidth = 16;
int nMapHeight = 16;

float fFOV = 3.13159f / 4.0f;
float fDepth = 16.0f;

int main()
{
	// Create screen buffer
	wchar_t* screen = new wchar_t[nScreenWidth * nScreenHeight];
	HANDLE hConsole = CreateConsoleScreenBuffer(
		GENERIC_READ | GENERIC_WRITE,
		0,
		NULL,
		CONSOLE_TEXTMODE_BUFFER,
		NULL
		);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWritten = 0;

	wstring map;
	map += L"################";
	map += L"#..............#";
	map += L"#..........##..#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..####........#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"###......#######";
	map += L"#..............#";
	map += L"#..............#";
	map += L"#..............#";
	map += L"######.........#";
	map += L"#..............#";
	map += L"################";

	auto tp1 = chrono::system_clock::now();
	auto tp2 = chrono::system_clock::now();

	// Game loop
	for (;;) {
		// Get current time
		tp2 = chrono::system_clock::now();
		chrono::duration<float> elapsedTime = tp2 - tp1;
		tp1 = tp2;
		float fElapsedTime = elapsedTime.count();

		// Player controls and rotation
		if (GetAsyncKeyState((unsigned short)'A') & 0x8000) {
			fPlayerA -= (0.8f) * fElapsedTime;
		}

		if (GetAsyncKeyState((unsigned short)'D') & 0x8000) {
			fPlayerA += (0.8f) * fElapsedTime;
		}

		if (GetAsyncKeyState((unsigned short)'W') & 0x8000) {
			fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
			fPlayerY += cosf(fPlayerA) * 5.0f * fElapsedTime;

			// Check if player collides with wall
			if (map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
				fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
				fPlayerX -= cosf(fPlayerA) * 5.0f * fElapsedTime;
			}
		}

		if (GetAsyncKeyState((unsigned short)'S') & 0x8000) {
			fPlayerX -= sinf(fPlayerA) * 5.0f * fElapsedTime;
			fPlayerY -= cosf(fPlayerA) * 5.0f * fElapsedTime;

			// Check if player collides with wall
			if (map.c_str()[(int)fPlayerY * nMapWidth + (int)fPlayerX] == '#') {
				fPlayerX += sinf(fPlayerA) * 5.0f * fElapsedTime;
				fPlayerX += cosf(fPlayerA) * 5.0f * fElapsedTime;
			}
		}

		for (int i = 0; i < nScreenWidth; ++i) {

			float fRayAngle = (
				(fPlayerA - fFOV / 2.0f) + ((float)i / (float)nScreenWidth) * fFOV
				);
			float fDistanceToWall = 0.0f;
			bool bHitWall = false;
			bool bBoundary = false;

			// Unit vector for ray within the player's space
			float fEyeX = sinf(fRayAngle);
			float fEyeY = cosf(fRayAngle);

			while (!bHitWall && fDistanceToWall < fDepth) {
				fDistanceToWall += 0.1f;

				int nTestX = (int)(fPlayerX + fEyeX * fDistanceToWall);
				int nTestY = (int)(fPlayerX + fEyeY * fDistanceToWall);

				// Check if ray is out of bounds
				if (nTestX < 0 || nTestX >= nMapWidth || nTestY < 0 || nTestY >= nMapHeight) {
					bHitWall = true;
					fDistanceToWall = fDepth;
				}
				else {
					if (map[nTestY * nMapWidth + nTestX] == '#') {
						bHitWall = true;

					}
				}
			}

			// Calculate distance to ceiling and floor
			int nCeiling = (float)(nScreenHeight / 2.0) - nScreenHeight / ((float)fDistanceToWall);
			int nFloor = nScreenHeight - nCeiling;

			short wShade = ' ';
			short fShade = ' ';

			// Shade walls based on distance
			if (fDistanceToWall <= fDepth / 4.0f) {
				wShade = 0x2588; // Closest to wall
			}
			else if (fDistanceToWall < fDepth / 3.0f) {
				wShade = 0x2593; 
			}
			else if (fDistanceToWall < fDepth / 2.0f) {
				wShade = 0x2592;
			} 
			else if (fDistanceToWall < fDepth) {
				wShade = 0x2591;
			}
			else {
				wShade = ' ';
			}

			for (int j = 0; j < nScreenHeight; ++j) {
				if (j < nCeiling) {
					screen[j * nScreenWidth + i] = ' ';
				}
				else if (j > nCeiling && j <= nFloor) {
					screen[j * nScreenWidth + i] = wShade;
				}
				else {
					float b = 1.0f - (((float)j - nScreenHeight / 2.0f) / ((float)nScreenHeight / 2.0f));

					if (b < 0.25f) {
						fShade = '#';
					}
					else if (b < 0.5) {
						fShade = '=';
					}
					else if (b < 0.75) {
						fShade = '-';
					}
					else if (b < 0.9) {
						fShade = '.';
					}
					else {
						fShade = ' ';
					}

					screen[j * nScreenWidth + i] = fShade;
				}
			}	
		}

		screen[nScreenWidth * nScreenHeight - 1] = '\0';
		WriteConsoleOutputCharacter(
			hConsole,
			screen,
			nScreenWidth * nScreenHeight,
			{ 0, 0 },
			&dwBytesWritten
		);
	}

	return 0;
}

