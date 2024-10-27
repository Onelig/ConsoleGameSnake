#include <limits>
#include <iostream>
#include <windows.h>
#include <random>
#include <deque>
#include <ranges>
#include <chrono>
#include <fcntl.h>
#include <io.h>

#include "TimerG.h"
#include "AvaibleCoords.h"

#define _GetAsyncKeyState_UP    ((GetAsyncKeyState('W') & 0x01) || (GetAsyncKeyState(VK_UP) & 0x01)    || (GetAsyncKeyState(VK_NUMPAD8) & 0x01))
#define _GetAsyncKeyState_DOWN  ((GetAsyncKeyState('S') & 0x01) || (GetAsyncKeyState(VK_DOWN) & 0x01)  || (GetAsyncKeyState(VK_NUMPAD2) & 0x01))
#define _GetAsyncKeyState_RIGHT ((GetAsyncKeyState('D') & 0x1)  || (GetAsyncKeyState(VK_RIGHT) & 0x01) || (GetAsyncKeyState(VK_NUMPAD6) & 0x01))
#define _GetAsyncKeyState_LEFT  ((GetAsyncKeyState('A') & 0x01) || (GetAsyncKeyState(VK_LEFT) & 0x01)  || (GetAsyncKeyState(VK_NUMPAD4) & 0x01))

std::vector<COORD>* alaible_coord = nullptr;
COORD* alaible_coord_info = nullptr;

UINT minutes = 0;
UINT seconds = 0;
USHORT score = 0;

bool isRun = TRUE;
bool isFruitExist = FALSE;
/// random module
std::random_device random_device;
std::mt19937 random_gen(random_device());
std::uniform_int_distribution<size_t> uidist;

enum Direction
{
	X, neg_X, Y, neg_Y
};

Direction snake_direction(X);
COORD fruit_position{ -1, -1 };
std::deque<COORD> snake_coords;

Timer timer;

void SetBaseInfo();

void clearArea(const COORD& coords);
void hideCursor();

void ReadAsyncKey();
void DrawSnake();
void SpawnFruit();
void ShowZone(const COORD& coords);
void ShowInfo();

int main()
{
	SetBaseInfo();

	hideCursor();
	timer.Start();
	
	ShowZone(*alaible_coord_info);

	while (isRun)
	{
		clearArea(*alaible_coord_info);
		
		ShowInfo();

		ReadAsyncKey();
		DrawSnake();
		SpawnFruit();
		Sleep(200);
	}
	system("cls");
	std::cout << "Game over\n";
	return 0;
}


void SetBaseInfo()
{
	bool isCin = false;
	USHORT* arena_tshort = new USHORT;
	
	do
	{
		system("cls");
		std::cout << "Choose the size of the playing field: \n";
		std::cout << "1 - Small arena\n";
		std::cout << "2 - Medium arena\n";
		std::cout << "3 - Huge arena";
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ sizeof("Choose the size of the playing field:"), 0 });

		if (!(std::cin >> *arena_tshort))
		{
			isCin = true;
			std::cin.clear();
			std::cin.ignore(10000, '\n');
			system("cls");
			std::cout << "Invalid type. Try again";
			Sleep(5000);
		}
		else if (*arena_tshort <= 0 || *arena_tshort >= 4)
		{
			system("cls");
			isCin = true;
			std::cout << "Incorrect value. Try again";
			Sleep(5000);
		}
		else
			isCin = false;
	} while (isCin);


	switch (*arena_tshort)
	{
	case 1:
	{
		alaible_coord = &small_arena;
		alaible_coord_info = &small_arena_info;
		break;
	}
	case 2:
	{
		alaible_coord = &medium_arena;
		alaible_coord_info = &medium_arena_info;
		break;
	}
	case 3:
	{
		alaible_coord = &huge_arena;
		alaible_coord_info = &huge_arena_info;
		break;
	}
	}

	delete arena_tshort;
	uidist = std::uniform_int_distribution<size_t>{ 0, alaible_coord->size() };
	snake_coords.emplace_back(alaible_coord_info->X / 2, alaible_coord_info->Y / 2);
	snake_coords.emplace_back(alaible_coord_info->X / 2 + 1, alaible_coord_info->Y / 2 + 1);

	system("cls");
}

void ShowZone(const COORD& coords)
{
	for (size_t i = 0; i <= coords.X; i++)
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ static_cast<short>(i) , static_cast<short>(coords.Y + 1) });
		std::cout << "#";
	}
	for (size_t i = 0; i <= coords.Y; i++)
	{
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ static_cast<short>(coords.X + 1) , static_cast<short>(i) });
		std::cout << "#";
	}

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ static_cast<short>(coords.X + 1) , static_cast<short>(coords.Y + 1) });
	std::cout << "#";
}

void ShowInfo()
{
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD( 0, alaible_coord_info->Y + 4 ));
	seconds = timer.GetDuration();
	if (seconds >= 60)
	{
		++minutes;
		seconds -= minutes * 60;
	}
	std::cout << "Score: " << score << std::endl;
	std::cout << "Time: " << minutes << " min " << seconds << " sec";
}

void clearArea(const COORD& coords)
{
	for (short x = 0; x <= coords.X; x++) {
		for (short y = 0; y <= coords.Y; y++) {
			SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), COORD{ x, y });
			std::cout << " ";
		}
	}
}

void hideCursor()
{
	CONSOLE_CURSOR_INFO cursorInfo;

	GetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	cursorInfo.bVisible = false;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
}

void ReadAsyncKey()
{

	if (GetAsyncKeyState(VK_ESCAPE) & 0x01)
	{
		isRun = false;
		return;
	}


	if (_GetAsyncKeyState_UP)
	{
		if (snake_direction != Direction::neg_Y)
			snake_direction = Direction::Y;
	}
	else if (_GetAsyncKeyState_DOWN)
	{

		if (snake_direction != Direction::Y)
			snake_direction = Direction::neg_Y;
	}
	else if (_GetAsyncKeyState_RIGHT)
	{
		if (snake_direction != Direction::neg_X)
			snake_direction = Direction::X;
	}
	else if (_GetAsyncKeyState_LEFT)
	{
		if (snake_direction != Direction::X)
			snake_direction = Direction::neg_X;
	}
}

void DrawSnake()
{
	switch (snake_direction)
	{
	case X:
		snake_coords.emplace_back(COORD{ static_cast<short>((snake_coords.cend() - 1)->X + 1), static_cast<short>((snake_coords.cend() - 1)->Y) });
		break;

	case neg_X:
		snake_coords.emplace_back(COORD{ static_cast<short>((snake_coords.cend() - 1)->X - 1), static_cast<short>((snake_coords.cend() - 1)->Y) });
		break;

	case Y:
		snake_coords.emplace_back(COORD{ static_cast<short>((snake_coords.cend() - 1)->X), static_cast<short>((snake_coords.cend() - 1)->Y - 1) });
		break;

	case neg_Y:
		snake_coords.emplace_back(COORD{ static_cast<short>((snake_coords.cend() - 1)->X), static_cast<short>((snake_coords.cend() - 1)->Y + 1) });
		break;
	}

	for (size_t i = 0; i < snake_coords.size() - 1; i++)
	{
		for (size_t j = i + 1; j < snake_coords.size(); j++)
		{
			if (snake_coords[i].X == snake_coords[j].X && snake_coords[i].Y == snake_coords[j].Y)
			{
				isRun = false;
				return;
			}
		}
	}

	if (snake_coords.rbegin()->X == fruit_position.X && snake_coords.rbegin()->Y == fruit_position.Y)
	{
		snake_coords.emplace_front(fruit_position);
		++score;
		isFruitExist = false;
	}
	
	if (snake_coords.rbegin()->X == -1 || snake_coords.rbegin()->X == alaible_coord_info->X + 1 || snake_coords.rbegin()->Y == -1 || snake_coords.rbegin()->Y == alaible_coord_info->Y + 1)
	{
		isRun = FALSE;
		return;
	}
	
	for (std::deque<COORD>::const_reverse_iterator i = snake_coords.rbegin(); i != snake_coords.rend() - 1; ++i) // const COORD i : std::ranges::views::reverse(snake_coords)
	{
		alaible_coord->erase(std::remove_if(alaible_coord->begin(), alaible_coord->end(), [&i](const COORD b) { return i->X == b.X && i->Y == b.Y; }), alaible_coord->end());
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), *i);
		std::cout << "#";
	}

	


	alaible_coord->emplace_back(*snake_coords.begin());
	snake_coords.pop_front();
}

void SpawnFruit()
{
	if (!isFruitExist)
	{
		std::vector<COORD>::const_iterator elementCoordF = (alaible_coord->begin() + (uidist(random_gen) % alaible_coord->size()));
		fruit_position = { elementCoordF->X, elementCoordF->Y };
		alaible_coord->erase(elementCoordF);
		isFruitExist = TRUE;
	}

	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), fruit_position);
	std::cout << "$";
}