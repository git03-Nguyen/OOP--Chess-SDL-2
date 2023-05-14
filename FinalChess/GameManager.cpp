#include "GameManager.h"

int GameManager::checkWinLose() const {
	bool runOutOfMove = true;
	for (int i = 0; i < 8 && runOutOfMove; i++) {
		for (int j = 0; j < 8 && runOutOfMove; j++) {
			runOutOfMove = !((board->piecesOnBoard[i][j] && board->piecesOnBoard[i][j]->color != currentTurn
				&& board->piecesOnBoard[i][j]->tableMove.size() != 0));
		}
	}
	if (!runOutOfMove) return 0;
	return (currentTurn == Color::White) ? 1 : -1;
}

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
	focusingBtn = nullptr;
	clickedPiece = nullptr;
	boardStateChange = true;
	matchResult = 0;

	gui->render(currentTurn, clickedPiece, focusingBtn);

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

		// If a team has won -> menu WIN -> exit or return to main menu
		if (matchResult) {
			// gui->showMatchResult(matchResult);
			// GameState -> MatchResultState
			if (matchResult == 1) cout << "WHITE WIN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
			else cout << "BLACK WIN!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
			//--> return to handle event with MatchResultScreen, not PlayingScreen
		}
		
		frameTime = SDL_GetTicks() - frameStart;

		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}
}

// TODO: try-catch
void GameManager::handleEvent() {
	int x, y;
	Button* tempButton;
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
	else if (((focusingBtn = gui->getButton(x, y)))) {
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
			// Before changing turn, check win/lose
			if (!(matchResult = checkWinLose())) changeTurn();
		}
	}
	// If choose "illegal" next move -> return to handleEvent() -> pick piece again
	else {
		std::cout << "Illegal move! " << newX << " " << newY << std::endl;
		clickedPiece = nullptr;
	}

}

void GameManager::handleClickButton(Button* clickedBtn) {
	// Open menu
	if (!clickedBtn) return;
	gui->renderClickBtn(clickedBtn);

	// Hearing events in menu...
	SDL_Delay(100);

}


