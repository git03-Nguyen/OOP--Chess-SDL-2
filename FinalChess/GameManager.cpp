#include "GameManager.h"

void GameManager::changeTurn() {
	if (!player) {
		return;
	}
	if ((gameState->matchResult = board->checkWinLose()) != 0) { // -1 or 1
		gameState->guiHasChanged = true;
		player = nullptr; // Disable continuing to play after winning/losing
		gameState->state = State::MATCH_RESULT;
	}
	else {
		player = ((player == player1) ? player2 : player1);
		gameState->currentColor = player->color;
	}

}

void GameManager::initialize(const char* fen) {
	// Initializing models
	board = new Board(fen);
	gui = new GuiManager(window, board);
	board->renderer = gui->getRenderer();

	// gameState - flags
	gameState = new GameState();
	gameState->currentColor = board->currentTurn;

	player = (player1->color == board->currentTurn) ? player1 : player2;

	cout << board->getFEN() << endl;

	
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

	player1 = new Human(Color::White); // 1st player is always human
	initialize();
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
void GameManager::gameLoop() {

	while (gameState->isRunning) {

		// Handle events (quit game, move mouse around (hover button), 
		//				click mouse (on board (pieces, no pieces, moves, invalid moves),
		//							on button (main menu, choose mode menu, playing game, promotion, match result, setting menu ) ),
		//				...
		handleEvent(); 

		// Render GUI based on game state
		if (gameState->guiHasChanged) gui->render(gameState);

		// AI's move
		if (player && player->type == PlayerType::ComAI) { 
			SDL_Delay(500);
			player->makeMove(board);
			changeTurn();
			gameState->guiHasChanged = true;
			gui->render(gameState);
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
			// Click on buttons -> maybe change the GameState
			if (gameState->clickedButton = gui->getButton(gameState, x, y)) {
				handleClickButton(gameState->clickedButton);
			}
			// Click on board (human-turn) -> highlight, choose moves
			else if (player && player->type == PlayerType::Human && gameState->state == State::PLAYING && gui->isOnBoard(x, y)) {
				int boardX = (x - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
				int boardY = (y - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
				(!gameState->clickedPiece) ? handleClickOnBoard(boardX, boardY) : handleChoosingMove(boardX, boardY);
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
	gameState->clickedPiece = board->pieces[boardX][boardY];

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
		board->movePiece(gameState->clickedPiece, newX, newY);

		if (gameState->clickedPiece->id == PieceID::Pawn && (newY == 0 || newY == 7)) { // Promotion
			std::cout << "Promotion: [" << newX << "][" << newY << "]" << std::endl;
			gameState->state = State::PROMOTION;
		}
		else { // Normal legal move
			std::cout << "Moved to [" << newX << "][" << newY << "]" << std::endl;
			// Before changing turn, check win/lose
			gameState->clickedPiece = nullptr;
			changeTurn();
		}
		
	}
	// If choose "illegal" next move -> return to handleEvent() -> pick piece again
	else {	
		std::cout << "Illegal move! [" << newX << "][" << newY << "]" << std::endl;
		gameState->clickedPiece = nullptr;
	}

	gameState->guiHasChanged = true;
}

void GameManager::handleClickButton(Button* clickedButton) {
	if (!clickedButton) return;

	int x, y;

	switch (clickedButton->type) {
	case ButtonType::SETTING:
		cout << "Clicked setting!" << endl;
		cout << "SETTING_MENU is showing ... " << endl;
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

	case ButtonType::VOLUMN:
		cout << "Clicked volumn!" << endl;
		SDL_GetMouseState(&x, &y);
		gameState->volumn = (x - clickedButton->posX) * 101.0 / clickedButton->width;
		cout << gameState->volumn << endl;
		break;

	case ButtonType::QUEEN:
	case ButtonType::BISHOP:
	case ButtonType::KNIGHT:
	case ButtonType::ROOK:
		cout << "Clicked Promotion!" << endl;
		gameState->focusedButton = nullptr;
		gameState->promotion = 1 + (clickedButton->type - ButtonType::QUEEN);
		board->promotePawn(gameState->clickedPiece, gameState->promotion);
		gameState->promotion = 0; gameState->clickedPiece = nullptr;
		changeTurn();
		gameState->state = State::PLAYING;
		break;

	// Other Buttons
	// ...
	case ButtonType::QUIT:
		cout << "Return to menu!" << endl;
		delete gui, board, gameState, player2;
		initialize();
		break;

	case ButtonType::RESTART:
		cout << "Restart game! " << endl;
		delete gui, board, gameState;
		initialize();
		gameState->state = State::PLAYING;
		break;

	case ButtonType::LOAD:
		cout << "Resume old games!" << endl;
		break;

	case ButtonType::HUMAN:
		cout << "PVP!" << endl;
		player2 = new Human(Color::Black);
		gameState->state = State::PLAYING;
		break;
	case ButtonType::COM:
		cout << "PVE!" << endl;
		player2 = new ComAI(Color::Black, Difficulty::RANDOM);
		gameState->state = State::PLAYING;
		break;

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


