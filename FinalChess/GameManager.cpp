#include "GameManager.h"

int GameManager::checkWinLose() const {
	bool runOutOfMove = true;
	for (int i = 0; i < 8 && runOutOfMove; i++) {
		for (int j = 0; j < 8 && runOutOfMove; j++) {
			runOutOfMove = !((board->piecesOnBoard[i][j] && board->piecesOnBoard[i][j]->color != gameState->currentColor
				&& board->piecesOnBoard[i][j]->tableMove.size() != 0));
		}
	}
	if (!runOutOfMove) return 0;
	return (gameState->currentColor == Color::White) ? 1 : -1;
}

void GameManager::changeTurn() {
	if ((gameState->matchResult = checkWinLose()) != 0) { // -1 or 1
		gameState->guiHasChanged = true;
		gameState->state = State::MATCH_RESULT;
	}
	else {
		player = ((player == player1) ? player2 : player1);
		gameState->currentColor = player->color;
	}
}

GameManager::GameManager(const char* title, int xPos, int yPos, int width, int height) {

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) throw SDL_GetError();
	std::cout << "Subsystems Initialized! ... " << std::endl;

	window = SDL_CreateWindow(title, xPos, yPos, width, height, 0);
	if (!window) throw SDL_GetError();
	std::cout << "Window created!" << std::endl;

	SCREEN_WIDTH = width;	SCREEN_HEIGHT = height;
	BOARD_SIZE = SCREEN_HEIGHT - 2 * BOARD_OFFSET;
	CELL_SIZE = round((BOARD_SIZE - 2 * BOARD_BORDER) / 8.0f);

	// Initializing models
	board = new Board();
	gui = new GuiManager(window, board);

	// Players
	player1 = new Human(Color::White);
	player2 = new Human(Color::Black);
	player = player1; // who choose White will go first
	
	// gameState - flags
	gameState = new GameState();
	
	// First time rendering
	gui->render(gameState);

}

GameManager::~GameManager() {
	delete board, gui, player1, player2, gameState;
	SDL_DestroyWindow(window);
	SDL_Quit();
}

// ----------------------------------------
// The only loop in the game is here
void GameManager::gameLoop(int fps) {

	int frameDelay = 1000 / fps;
	Uint32 frameStart = 0;
	int frameTime = 0;

	while (gameState->isRunning) {
		frameStart = SDL_GetTicks();

		// Handle events (quit game, move mouse around (hover button), 
		//				click mouse (on board (pieces, no pieces, moves, invalid moves),
		//							on button (main menu, choose mode menu, playing game, promotion, match result, setting menu ) ),
		//				...
		handleEvent(); 

		// Render GUI based on game state
		if (gameState->guiHasChanged) gui->render(gameState);
		
		frameTime = SDL_GetTicks() - frameStart;
		if (frameDelay > frameTime) {
			SDL_Delay(frameDelay - frameTime);
		}
	}
}

// ------------------------------
void GameManager::handleEvent() {
	SDL_Event e;
	while (SDL_PollEvent(&e)) { // => Differences between WaitEvent and PollEvent

		// -> Event Quit
		if (e.type == SDL_QUIT) {
			gameState->isRunning = false;
			return;
		}

		int x, y;
		SDL_GetMouseState(&x, &y);

		// Event Hover button
		if (e.type == SDL_MOUSEMOTION) {
			Button* mouseOnButton = gui->getButton(gameState, x, y);
			if (gameState->focusedButton != mouseOnButton) {
				gameState->focusedButton = mouseOnButton;
				gameState->guiHasChanged = true;
			}
			else {
				gameState->guiHasChanged = false;
			}
			return;
		}

		// Event Click 
		if (e.type == SDL_MOUSEBUTTONDOWN) {
			// Click on board -> highlight, choose moves
			if (gameState->state == State::PLAYING && gui->isOnBoard(x, y)) {
				int boardX = (x - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
				int boardY = (y - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
				(!gameState->clickedPiece) ? handleClickOnBoard(boardX, boardY) : handleChoosingMove(boardX, boardY);
			}
			// Click on buttons -> maybe change the GameState
			else if (gameState->clickedButton = gui->getButton(gameState, x, y)) {
				handleClickButton(gameState->clickedButton);
			}
			// Click on NOT board or buttons -> empty space
			else {
				std::cout << "Clicked on empty space: " << x << " " << y << std::endl;
			}
			//return;
		}

		// Other events ...
		
	}
}

// -----------------------------------------------------

void GameManager::handleClickOnBoard(int boardX, int boardY) {
	gameState->clickedPiece = board->piecesOnBoard[boardX][boardY];

	if (!gameState->clickedPiece || gameState->clickedPiece->color != gameState->currentColor) {
		// If click to INVALID piece -> return to hear event again
		std::cout << "Click on invalid piece [" << boardX << "][" << boardY << "]" << std::endl;
		gameState->clickedPiece = nullptr;
	}
	else {
		// If click to a VALID piece -> go to choosing next move
		std::cout << "Click on " << gameState->clickedPiece->imagePath << " [" << boardX << "][" << boardY << "]" << std::endl;
	}

	gameState->guiHasChanged = true;
}

void GameManager::handleChoosingMove(int newX, int newY) {
	// If choose "legal" next move -> move -> return to handleEvent() -> other turn
	if (gameState->clickedPiece->isLegalMove(newX, newY)) {
		if (board->movePiece(gameState->clickedPiece, newX, newY)) {
			std::cout << "Moved to [" << newX << "][" << newY << "]" << std::endl;
			// Before changing turn, check win/lose
			changeTurn();
		}
	}
	// If choose "illegal" next move -> return to handleEvent() -> pick piece again
	else {	
		std::cout << "Illegal move! [" << newX << "][" << newY << "]" << std::endl;
	}

	gameState->clickedPiece = nullptr;
	gameState->guiHasChanged = true;
}

void GameManager::handleClickButton(Button* clickedButton) {
	if (!clickedButton) return;

	switch (clickedButton->type) {
	case ButtonType::SETTING:
		cout << "Clicked setting!" << endl;
		gameState->focusedButton = nullptr;
		gameState->state = State::SETTING_MENU;
		break;

	case ButtonType::UNDO:
		cout << "Clicked undo!" << endl;
		board->undo();
		break;

	case ButtonType::REDO:
		cout << "Clicked redo!" << endl;
		board->redo();
		break;

	case ButtonType::RESUME:
		cout << "Clicked resume!" << endl;
		gameState->focusedButton = nullptr;
		gameState->state = State::PLAYING;
		break;

	// Other Buttons
	// ...

	default:
		break;
	}

	gameState->clickedButton = nullptr;
	gameState->guiHasChanged = true;
}

// -----------------------------------------------

int GameManager::testCaseDepth(int depth) const {
	return 0;
}


