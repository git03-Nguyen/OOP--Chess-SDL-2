#pragma once

#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

enum ButtonType { HUMAN, COM, EASY, HARD, WHITE, BLACK, SETTING, UNDO, REDO, QUEEN, BISHOP, KNIGHT, ROOK, BACK };

class Button {
public:
	ButtonType type;
	int size;
	int posX, posY;
	SDL_Texture* texture;

	Button(int _posX, int _posY, int _size, ButtonType _type);
	~Button();

	friend std::ostream& operator<<(std::ostream& os, const Button& button);

};





