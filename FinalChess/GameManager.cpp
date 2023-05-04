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
	CELL_SIZE = round((BOARD_SIZE - 2 * BOARD_BORDER) / 8.0f);

	board = new Board();
	gui = new GuiManager(window, board);

	// flags
	isRunning = true;
	currentTurn = Color::White; // white always goes first
	focusingBtn = Button::NONE;
	clickedPiece = nullptr;
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
		if (boardStateChange) gui->render(currentTurn, clickedPiece, focusingBtn);

		// set default flags -> when no event is handled -> no change in GUI
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
	Button tempButton;
	SDL_Event e;

	// 1 loop => perform 1 event
	while (SDL_PollEvent(&e)) { // => Differences between WaitEvent and PollEvent
		switch (e.type) {
		case SDL_QUIT:
			isRunning = false;
			return;

		// Mouse over buttons -> refresh GUI (hover button) -> handleEvent() again
		case SDL_MOUSEMOTION:
		SDL_GetMouseState(&x, &y);
		tempButton = gui->getButton(x, y);
		if (focusingBtn != tempButton) {
			focusingBtn = tempButton;
			boardStateChange = true;
		}
		else {
			boardStateChange = false;
		}
		break;

		case SDL_MOUSEBUTTONDOWN: // Click on piece / button
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
	if (gui->isOnBoard(x, y)) {
		int boardX = (x - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
		int boardY = (y - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
		(clickedPiece) ? handleChoosingMove(boardX, boardY) : handleClickOnBoard(boardX, boardY);
	}

	// Click on others - i.e: Menu, ...
	else if ((focusingBtn = gui->getButton(x, y)) != Button::NONE) {
			handleClickButton(focusingBtn);
	}
	
	// Click on empty space -> return to handleEvent()
	else {
		std::cout << "Clicking on empty space: " << x << " " << y << std::endl;
		boardStateChange = false;
	}

}

void GameManager::handleClickOnBoard(int boardX, int boardY) {
	clickedPiece = board->piecesOnBoard[boardX][boardY];
	if (!clickedPiece || clickedPiece->color != currentTurn) {
		std::cout << "Click on invalid piece " << boardX << " " << boardY << std::endl;
		clickedPiece = nullptr;
	}
	else {
		// If click to a piece -> go to choosing next move
		std::cout << "Click on " << clickedPiece->imagePath << " " << boardX << " " << boardY << std::endl;
	}
}

void GameManager::handleChoosingMove(int newX, int newY) {
	// If choose "legal" next move -> move -> return to handleEvent() -> other turn
	if (clickedPiece->isLegalMove(newX, newY)) {
		if (board->movePiece(clickedPiece, newX, newY)) {
			std::cout << "Moved to " << newX << " " << newY << std::endl;
			clickedPiece = nullptr;
			changeTurn();
		}
	}
	// If choose "illegal" next move -> return to handleEvent() -> pick piece again
	else {
		std::cout << "Illegal move! " << newX << " " << newY << std::endl;
		clickedPiece = nullptr;
	}

}

void GameManager::handleClickButton(Button clickedBtn) {
	// Open menu
	gui->renderClickBtn(clickedBtn);

	// Hearing events in menu...
	SDL_Delay(2000);

}


