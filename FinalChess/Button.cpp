#include "Button.h"

Button::Button(int _posX, int _posY, int _size, ButtonType _type) :
	posX(_posX), posY(_posY), size(_size), type(_type), texture(nullptr) {}

Button::~Button() {
	SDL_DestroyTexture(texture); 
}

std::ostream& operator<<(std::ostream& os, const Button& button) {
	switch (button.type) {
	case ButtonType::NONE:
		return os << "None";
	case ButtonType::SETTINGS:
		return os << "settings";
	case ButtonType::UNDO:
		return os << "undo";
	case ButtonType::REDO:
		return os << "redo";
	}
}
