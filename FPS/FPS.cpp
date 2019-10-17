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
	map += L"#              #";
	map += L"#              #";
	map += L"#              #";
	map += L"#              #";
	map += L"#              #";
	map += L"#              #";
	map += L"#              #";
	map += L"#              #";
	map += L"#              #";
	map += L"#              #";
	map += L"#              #";
	map += L"#              #";
	map += L"#              #";
	map += L"#              #";
	map += L"################";

	// Game loop
	for (;;) {
		for (int i = 0; i < nScreenWidth; ++i) {

			float fRayAngle = (
				(fPlayerA - fFOV / 2.0f) + ((float)i / (float)nScreenWidth) * fFOV
				);
			float fDistanceToWall = 0.0f;
			bool bHitWall = false;

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

			for (int j = 0; j < nScreenHeight; ++j) {
				if (j < nCeiling) {
					screen[j * nScreenWidth + i] = ' ';
				}
				else if (j > nCeiling && j <= nFloor) {
					screen[j * nScreenWidth + i] = '#';
				}
				else {
					screen[j * nScreenWidth + i] = ' ';
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

