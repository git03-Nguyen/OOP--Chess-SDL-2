#include "GameManager.h"

void GameManager::changeTurn() {
	currentTurn = (currentTurn == Color::White) ? Color::Black : Color::White;
}

GameManager::GameManager(const char* title, int xPos, int yPos, int width, int height) {

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) throw SDL_GetError();
	std::cout << "Subsystems Initialized! ... " << std::endl;

	window = SDL_CreateWindow(title, xPos, yPos, width, height, 0);
	if (!window) throw SDL_GetError();
	std::cout << "Window created!" << std::endl;

	SCREEN_WIDTH = width;
	SCREEN_HEIGHT = height;
	BOARD_SIZE = SCREEN_HEIGHT - 2 * BOARD_OFFSET;
	CELL_SIZE = round((BOARD_SIZE - 2 * BOARD_BORDER) / 8.0);

	board = new Board();
	gui = new GuiManager(window, board);

	// flags
	isRunning = true;
	currentTurn = Color::White; // white always goes first
	boardStateChange = true;

}

GameManager::~GameManager() {
	delete board, gui;
	SDL_DestroyWindow(window);
	SDL_Quit();

}

void GameManager::gameLoop(int fps) {

	int frameDelay = 1000 / fps;
	Uint32 frameStart = 0;
	int frameTime = 0;

	while (isRunning) {
		frameStart = SDL_GetTicks();
		handleEvent();
		if (boardStateChange) gui->render(currentTurn);
		boardStateChange = false;
		frameTime = SDL_GetTicks() - frameStart;

		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}
}

// TODO: try-catch
void GameManager::handleEvent() {
	int x, y;
	SDL_Event e;
	// 1 loop => perform 1 event
	while (SDL_PollEvent(&e)) { // => Khac biet giua WaitEvent va PollEvent
		switch (e.type) {
		case SDL_QUIT:
			isRunning = false;
			return;

		// Mouse over buttons -> hearing next move again
		case SDL_MOUSEMOTION:
		SDL_GetMouseState(&x, &y);
		if (gui->isOverButton(x, y)) {
			//gui->hoverButton(x, y);
			std::cout << "Mouse over button" << std::endl;
		}
		break;

		case SDL_MOUSEBUTTONDOWN: // Click -> piece / to move / menu
			boardStateChange = true;
			handleMouseClick(e);
			break;

		default:
			break;
		}

	}
}

void GameManager::handleMouseClick(SDL_Event& e) {
	int x, y;
	SDL_GetMouseState(&x, &y);

	// Click on board
	if (gui->isOverBoard(x, y)) {
		int boardX = (x - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
		int boardY = (y - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
		handleClickOnBoard(boardX, boardY);
	}

	// Click on others - i.e: Menu, ...
	else if (gui->isOverButton(x, y)) {
		handleClickButton(x, y);
	}
	
	// Click on empty space -> return to handleEvent()
	else {
		std::cout << "Clicking on empty space: " << x << " " << y << std::endl;
	}

}

void GameManager::handleClickOnBoard(int boardX, int boardY) {
	Piece* clickedPiece = board->piecesOnBoard[boardX][boardY];
	if (!clickedPiece || clickedPiece->color != currentTurn) {
		std::cout << "Click on no valid piece" << boardX << boardY << std::endl;
		return;
	}

	// If click to a piece -> go to choosing next move
	std::cout << "Click on " << clickedPiece->imagePath << " " << boardX << " " << boardY << std::endl;
	gui->renderHighLight(clickedPiece);

	// Hearing for next move - while processing other events (quit, menu, ...) 
	SDL_Event e;
	int x, y, newX, newY;
	while (SDL_WaitEvent(&e)) {
		switch (e.type) {
		case SDL_QUIT:
			isRunning = false;
			return;

		// Mouse over buttons -> hearing next move again
		case SDL_MOUSEMOTION:
			SDL_GetMouseState(&x, &y);
			if (gui->isOverButton(x,y)) {
				//gui->hoverButton(x, y);
				std::cout << "Mouse over button" << std::endl;
			}
			break;

		// Clicking while highlighting
		case SDL_MOUSEBUTTONDOWN:
			SDL_GetMouseState(&x, &y);
			newX = (x - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
			newY = (y - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;

			if (gui->isOverBoard(x, y)) {
				// Choose "legal" next move -> return to handleEvent()
				if (clickedPiece->isLegalMove(newX, newY)) {
					if (board->movePiece(clickedPiece, newX, newY)) {
						std::cout << "Moved to " << newX << " " << newY << std::endl;
						changeTurn();
						return;
					}
				}
				// Choose "illegal" next move -> return to handleEvent() 
				else {
					std::cout << "Illegal move! " << newX << " " << newY << std::endl;
					return;
				}
			}
			// Choose menu -> go to menu -> return to hear for next move
			else if (gui->isOverButton(x, y)) {
				handleClickButton(x, y);
			}
			// Choose blank space -> return to hear for next move
			else {
				std::cout << "Clicking on empty space: " << x << " " << y << std::endl;
			}

		}
		
	}
}

void GameManager::handleClickButton(int x, int y) {
	Button buttonClicked = gui->getButton(x, y);
	switch (buttonClicked) {
	case Button::SETTINGS:
		std::cout << "Clicking on button settings " << std::endl;
		gui->renderClickBtn(buttonClicked);
		break;
	case Button::UNDO:
		break;
	case Button::REDO:
		break;
	default:
		break;
	}

}


