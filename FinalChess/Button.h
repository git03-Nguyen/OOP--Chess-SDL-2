#pragma once

#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

enum ButtonType { LOAD, HUMAN, COM, ABOUT, EASY, HARD, WHITE, BLACK, SETTING, UNDO, REDO, VOLUMN, RESTART, QUIT, RESUME, QUEEN, BISHOP, KNIGHT, ROOK, SIZE };

class Button {
public:
	ButtonType type;
	int width, height;
	int posX, posY;
	SDL_Texture* texture;

	Button(int _posX, int _posY, int _width, int _height, ButtonType _type);
	~Button();

};





