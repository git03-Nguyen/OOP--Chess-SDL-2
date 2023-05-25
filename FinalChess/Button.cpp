#include "Button.h"

Button::Button(int _posX, int _posY, int _width, int _height, ButtonType _type) :
	posX(_posX), posY(_posY), width(_width), height(_height), type(_type), texture(nullptr) { }

Button::~Button() {
	SDL_DestroyTexture(texture); 
}

