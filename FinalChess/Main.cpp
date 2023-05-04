#include "Main.h"

int main(int argc, char* argv[]) {

	GameManager* gameManager = nullptr;
	int errorCode = 0;

	try {
		gameManager = new GameManager("Chess", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 690, 600);
		gameManager->gameLoop(60);
	}
	catch (std::exception e) {
		std::cerr << "ERROR: " << e.what() << "\n";
		errorCode = 1;
	}
	catch (std::string e) {
		std::cerr << "ERROR: " << e << "\n";
		errorCode = 2;
	}
	catch (...) {
		std::cerr << "Undefined error: " << SDL_GetError() << "\n";
		errorCode = 3;
	}

	delete gameManager;
	std::cout << "Exitting! Error code: " << errorCode << std::endl;
	return errorCode;
}
