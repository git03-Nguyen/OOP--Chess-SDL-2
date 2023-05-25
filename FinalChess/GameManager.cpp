#include "GameManager.h"

void GameManager::changeTurn() {
	if (!player) return;
	
	// Disable continuing to play after win/lose
	if ((gameState->matchResult = board->getMatchResult()) != MatchResult::None) {
		gameState->guiHasChanged = true;
		player = nullptr; // Disable continuing to play after winning/losing
		gameState->state = State::MATCH_RESULT;
		return;
	}

	player = ((player1->color == board->currentTurn) ? player1 : player2);

}

void GameManager::newGameBoard(vector<string> oldMoves) {
	// Initializing models
	board = new Board(oldMoves);
	gui->setBoard(board);
	board->renderer = gui->getRenderer();

	// gameState - flags
	gameState->state = State::PLAYING;
	gameState->guiHasChanged = true;

	player1 = new Human(Color::White); // 1st player is always human
	player = (player1->color == board->currentTurn) ? player1 : player2;
	changeTurn();
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

	player1 = player2 = player = nullptr;
	board = nullptr;
	gui = new GuiManager(window);
	gameState = new GameState();
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

		// AI's moves
		if (player && player->type == PlayerType::ComAI) {
			player->makeMove(board);
			gui->render(gameState);
			changeTurn();
		}

		SDL_Delay(1);

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

		SDL_GetMouseState(&gameState->x, &gameState->y);

		// Event Hover button
		if (e.type == SDL_MOUSEMOTION) {
			Button* mouseOnButton = gui->getButton(gameState);
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
			if (gameState->clickedButton = gui->getButton(gameState)) {
				handleClickButton(gameState->clickedButton);
			}
			// Click on board (human-turn) -> highlight, choose moves
			else if (player && player->type == PlayerType::Human && gui->mouseIsOnBoard(gameState)) {
				int boardX = (gameState->x - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
				int boardY = (gameState->y - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
				(!gameState->clickedPiece) ? handleClickOnBoard(boardX, boardY) : handleChoosingMove(boardX, boardY);
			}
			// Click on NOT board or buttons -> empty space
			else {
				std::cout << "Clicked on empty space: " << gameState->x << " " << gameState->y << std::endl;
			}
			//return;
		}

		// Other events ...
		
	}
}

// -----------------------------------------------------

void GameManager::handleClickOnBoard(int boardX, int boardY) {
	gameState->clickedPiece = board->pieces[boardX][boardY];

	if (!gameState->clickedPiece || gameState->clickedPiece->color != board->currentTurn) {
		// If click to INVALID piece -> return to hear event again
		std::cout << "Click on invalid piece [" << boardX << "][" << boardY << "]" << std::endl;
		gameState->clickedPiece = nullptr;
	}
	else {
		// If click to a VALID piece -> go to choosing next move
		//std::cout << "Click on " << gameState->clickedPiece->imagePath << " [" << boardX << "][" << boardY << "]" << std::endl;
	}

	gameState->guiHasChanged = true;
}

void GameManager::handleChoosingMove(int newX, int newY) {
	// If choose "legal" next move -> move -> check -> return to handleEvent() -> other turn
	if (gameState->clickedPiece->isLegalMove(newX, newY)) {
		
		// Promotion -> we  have to wait for promotion
		if (board->movePiece(gameState->clickedPiece, newX, newY) == MoveID::Promotion) {
			std::cout << "Promotion: [" << newX << "][" << newY << "]" << std::endl;
			gameState->pX = newX; gameState->pY = newY;
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
	fstream fs;
	char lineFEN[100]; memset(lineFEN, 0, 100);
	vector<string> history;
	int x, y;

	switch (clickedButton->type) {
	case ButtonType::SETTING:
		cout << "Clicked setting!" << endl;
		gameState->focusedButton = nullptr;
		gameState->state = State::SETTING_MENU;
		break;

	case ButtonType::UNDO:
		cout << "Clicked undo!" << endl;
		if (board->canUndo()) {
			gameState->clickedPiece = nullptr;
			if (gameState->matchResult != MatchResult::None) {
				player = (player1->color == board->currentTurn) ? player1 : player2;
			}
			board->undo();
			if (player2->type == PlayerType::ComAI) board->undo();
			else changeTurn();
			gui->setBoard(board);
		}
		break;

	case ButtonType::REDO:
		cout << "Clicked redo!" << endl;

		if (board->canRedo()) {
			gameState->clickedPiece = nullptr;
			board->redo();
			if (player2->type == PlayerType::ComAI) board->redo();
			else changeTurn();
			gui->setBoard(board);
		}
		break;

	case ButtonType::RESUME:
		cout << "Clicked resume!" << endl;
		gameState->focusedButton = nullptr;
		gameState->state = State::PLAYING;
		break;

	case ButtonType::VOLUMN:
		SDL_GetMouseState(&gameState->x, &gameState->y);
		gameState->volumn = (gameState->x - gameState->clickedButton->posX) * 101.0 / gameState->clickedButton->width;
		break;

	case ButtonType::QUEEN:
	case ButtonType::BISHOP:
	case ButtonType::KNIGHT:
	case ButtonType::ROOK:
		cout << "Clicked Promotion!" << endl;
		gameState->focusedButton = nullptr;
		gameState->promotion = 1 + (clickedButton->type - ButtonType::QUEEN);
		board->promotePawn(gameState->clickedPiece, gameState->pX, gameState->pY, gameState->promotion);
		gameState->promotion = 0; gameState->clickedPiece = nullptr; gameState->x = gameState->y = -1;
		gameState->state = State::PLAYING;
		changeTurn();
		break;

	// Other Buttons
	// ...
	case ButtonType::QUIT:
		cout << "Return to menu!" << endl;
		try {
			fs.open("match.fen", ios::out);
			if (!fs.is_open()) throw "Cannot write file!";

			if (player2->type == PlayerType::Human) fs << 0 << endl;
			else if (dynamic_cast<ComAI*>(player2)->diff == Difficulty::RANDOM) fs << 1 << endl;
			else fs << 2 << endl;

			for (int i = 0; i < board->history.size(); i++) {
				fs << board->history[i] << endl;
			}

			fs.close();
		}
		catch (const char* e) {
			cout << e << endl;
		}

		delete board, player2;
		board = nullptr; player2 = nullptr;
		gameState->focusedButton = nullptr;
		gameState->clickedPiece = nullptr;
		gameState->state = State::MAIN_MENU;
		break;

	case ButtonType::RESTART:
		cout << "Restart game! " << endl;
		delete board;
		newGameBoard();
		gameState->state = State::PLAYING;
		break;

	case ButtonType::LOAD:
		cout << "Resume old games!" << endl;
		try {
			history.clear();
			fs.open("match.fen", ios::in);
			if (!fs.is_open()) throw "No old data!";
			fs >> lineFEN[0];
			if (lineFEN[0] == '0') player2 = new Human(Color::Black);
			else if (lineFEN[0] == '1') player2 = new ComAI(Color::Black, Difficulty::RANDOM);
			else player2 = new ComAI(Color::Black, Difficulty::HARD);
			while (!fs.eof()) {
				fs.getline(lineFEN, 100);
				if (strlen(lineFEN)) {
					history.push_back(lineFEN);
					memset(lineFEN, 0, 100);
				}
			}
			fs.close();
		}
		catch (const char* e) {
			cout << e << endl;
		}
		newGameBoard(history);
		history.clear();
		break;

	case ButtonType::HUMAN:
		cout << "PVP!" << endl;
		player2 = new Human(Color::Black);
		newGameBoard();
		gameState->state = State::PLAYING;
		break;

	case ButtonType::COM:
		cout << "PVE!" << endl;
		player2 = new ComAI(Color::Black, Difficulty::HARD); 
		newGameBoard();
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


