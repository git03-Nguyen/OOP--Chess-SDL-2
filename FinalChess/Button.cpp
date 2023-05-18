#include "Button.h"

Button::Button(int _posX, int _posY, int _width, int _height, ButtonType _type) :
	posX(_posX), posY(_posY), width(_width), height(_height), type(_type), texture(nullptr) { }

Button::~Button() {
	SDL_DestroyTexture(texture); 
}

std::ostream& operator<<(std::ostream& os, const Button& button) {
	switch (button.type) {
	case ButtonType::SETTING:
		return os << "settings";
	case ButtonType::UNDO:
		return os << "undo";
	case ButtonType::REDO:
		return os << "redo";
	case ButtonType::RESUME:
		return os << "resume";
	case ButtonType::VOLUMN:
		return os << "volumn";
	default:
		return os << "unknown";
	}
}
