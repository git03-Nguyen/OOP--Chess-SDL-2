#pragma once

#include <string>
#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

#define BUTTON_SIZE 60
#define SETTING_POS_X 600
#define SETTING_POS_Y 500
#define UNDO_POS_X 600
#define UNDO_POS_Y 280
#define REDO_POS_X 600
#define REDO_POS_Y 350

enum class ButtonType { NONE = -1, SETTINGS = 0, UNDO = 1, REDO = 2 };

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


