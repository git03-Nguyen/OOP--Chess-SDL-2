#include "GuiManager.h"

GuiManager::GuiManager(SDL_Window* _window, Board* _board) : window(_window), board(_board) {
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	TTF_Init();

	SDL_Point size;
	SDL_GetWindowSize(window, &size.x, &size.y);
	SCREEN_WIDTH = size.x; SCREEN_HEIGHT = size.y;
	BOARD_SIZE = SCREEN_HEIGHT - 2 * BOARD_OFFSET;
	CELL_SIZE = round((BOARD_SIZE - 2 * BOARD_BORDER) / 8.0);
	SIDEBAR_WIDTH = SCREEN_WIDTH - BOARD_OFFSET - BOARD_SIZE;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer) std::cout << "Renderer created!" << std::endl; else throw "Cannot create renderer!";

	buffer = nullptr;

	bgTexture = IMG_LoadTexture(renderer, "../Assets/Background/background.jpg");
	boardTexture = IMG_LoadTexture(renderer, "../Assets/Background/brown_board.jpg");
	blackTurnTexture = IMG_LoadTexture(renderer, "../Assets/Background/black_move.png");
	whiteTurnTexture = IMG_LoadTexture(renderer, "../Assets/Background/white_move.png");
	nextMoveTexture = IMG_LoadTexture(renderer, "../Assets/Buttons/next_move.png");
	killingMoveTexture = IMG_LoadTexture(renderer, "../Assets/Buttons/killing_move.png");

	mainMenuTexture = IMG_LoadTexture(renderer, "../Assets/Background/main_menu.png");

	settingBoardTexture = IMG_LoadTexture(renderer, "../Assets/Buttons/setting_board.png"); 
	sliderVolumnTexture = IMG_LoadTexture(renderer, "../Assets/Buttons/slider_volumn.png");
	dotVolumnTexture = IMG_LoadTexture(renderer, "../Assets/Buttons/dot_volumn.png");

	whiteWinTexture = IMG_LoadTexture(renderer, "../Assets/Background/white_win.png");
	blackWinTexture = IMG_LoadTexture(renderer, "../Assets/Background/black_win.png");
	
	// new buttons

	buttons.resize(ButtonType::SIZE);
	for (auto& button : buttons) button = nullptr;

	buttons[ButtonType::LOAD] = new Button(310, 280, 100, 50, ButtonType::LOAD);
	buttons[ButtonType::LOAD]->texture = IMG_LoadTexture(renderer, "../Assets/Buttons/load.png");
	buttons[ButtonType::HUMAN] = new Button(305, 350, 100, 50, ButtonType::HUMAN);
	buttons[ButtonType::HUMAN]->texture = IMG_LoadTexture(renderer, "../Assets/Buttons/pvp.png");
	buttons[ButtonType::COM] = new Button(305, 420, 100, 50, ButtonType::COM);
	buttons[ButtonType::COM]->texture = IMG_LoadTexture(renderer, "../Assets/Buttons/pve.png");


	buttons[ButtonType::SETTING] = new Button(SETTING_POS_X, SETTING_POS_Y, BUTTON_SIZE, BUTTON_SIZE, ButtonType::SETTING);
	buttons[ButtonType::SETTING]->texture = IMG_LoadTexture(renderer, "../Assets/Buttons/settings.png");
	buttons[ButtonType::UNDO] = new Button(UNDO_POS_X, UNDO_POS_Y, BUTTON_SIZE, BUTTON_SIZE, ButtonType::UNDO);
	buttons[ButtonType::UNDO]->texture = IMG_LoadTexture(renderer, "../Assets/Buttons/undo.png");
	buttons[ButtonType::REDO] = new Button(REDO_POS_X, REDO_POS_Y, BUTTON_SIZE, BUTTON_SIZE, ButtonType::REDO);
	buttons[ButtonType::REDO]->texture = IMG_LoadTexture(renderer, "../Assets/Buttons/redo.png");

	buttons[ButtonType::RESUME] = new Button(RESUME_POS_X, RESUME_POS_Y, RESUME_SIZE, RESUME_SIZE, ButtonType::RESUME);
	buttons[ButtonType::RESUME]->texture = IMG_LoadTexture(renderer, "../Assets/Buttons/resume.png");
	buttons[ButtonType::VOLUMN] = new Button(VOLUMN_POS_X, VOLUMN_POS_Y, VOLUMN_HEIGHT, VOLUMN_WIDTH, ButtonType::VOLUMN);
	
	buttons[ButtonType::QUIT] = new Button(0, 0, 50, 50, ButtonType::QUIT);
	buttons[ButtonType::QUIT]->texture = IMG_LoadTexture(renderer, "../Assets/Buttons/quit.png");
	buttons[ButtonType::RESTART] = new Button(100, 0, 50, 50, ButtonType::RESTART);
	buttons[ButtonType::RESTART]->texture = IMG_LoadTexture(renderer, "../Assets/Buttons/restart.png");

	buttons[ButtonType::QUEEN] = new Button(110, 280, 90, 90, ButtonType::QUEEN);
	buttons[ButtonType::BISHOP] = new Button(210, 280, 90, 90, ButtonType::BISHOP);
	buttons[ButtonType::KNIGHT] = new Button(310, 280, 90, 90, ButtonType::KNIGHT);
	buttons[ButtonType::ROOK] = new Button(410, 280, 90, 90, ButtonType::ROOK);


	// ...
	// pieces' textures
	for (auto& row : board->piecesOnBoard) {
		for (auto& piece : row) {
			if (piece) piece->setTexture(renderer);
		}
	}

}

GuiManager::~GuiManager() {
	SDL_DestroyTexture(buffer);
	SDL_DestroyTexture(bgTexture);
	SDL_DestroyTexture(boardTexture);
	SDL_DestroyTexture(blackTurnTexture);
	SDL_DestroyTexture(whiteTurnTexture);
	SDL_DestroyTexture(nextMoveTexture);

	SDL_DestroyTexture(mainMenuTexture);

	SDL_DestroyTexture(settingBoardTexture);
	SDL_DestroyTexture(sliderVolumnTexture);
	SDL_DestroyTexture(dotVolumnTexture);

	SDL_DestroyTexture(whiteWinTexture);
	SDL_DestroyTexture(blackWinTexture);

	for (auto& button : buttons) if (button) delete button;
	buttons.clear();
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	IMG_Quit();
}

SDL_Renderer* GuiManager::getRenderer() const {
	return renderer;
}

void GuiManager::render(GameState* gameState) {
	if (!gameState->guiHasChanged) return;

	switch (gameState->state) {
	case State::MAIN_MENU:
		renderMainMenu(gameState);
		break;

	case State::CHOOSE_DIFFICULTY:
		renderChooseDifficulty(gameState);
		break;

	case State::CHOOSE_COLOR:
		renderChooseColor(gameState);
		break;

	case State::PLAYING:
		if (buffer) { // Clear buffer
			SDL_DestroyTexture(buffer);
			buffer = nullptr;
		}
		renderPlaying(gameState);
		break;

	case State::SETTING_MENU:
		renderSettingMenu(gameState);
		break;

	case State::PROMOTION:
		renderPromotionMenu(gameState);
		break;

	case State::MATCH_RESULT:
		renderMatchResult(gameState);
		break;

	default:
		break;
	}

	SDL_RenderPresent(renderer);

	// set flag to render -> optimizing performance
	gameState->guiHasChanged = false;

}

void GuiManager::renderMainMenu(GameState* gameState) {
	cout << "MAIN_MENU is showing ... " << endl;
	SDL_RenderClear(renderer);
	drawBackground(gameState->state);

	drawCircleButton(buttons[ButtonType::LOAD], gameState->focusedButton, false);
	drawCircleButton(buttons[ButtonType::HUMAN], gameState->focusedButton, false);
	drawCircleButton(buttons[ButtonType::COM], gameState->focusedButton, false);
	// ...
}

void GuiManager::renderChooseDifficulty(GameState* gameState) {
	cout << "CHOOOSE_DIFFICULTY is showing ... " << endl;
	// ...
}

void GuiManager::renderChooseColor(GameState* gameState) {
	cout << "CHOOOSE_COLOR is showing ... " << endl;
	// ...
}

void GuiManager::renderPlaying(GameState* gameState) {

	SDL_RenderClear(renderer);

	drawBackground(gameState->state);
	drawCircleButton(buttons[ButtonType::SETTING], gameState->focusedButton);
	drawCircleButton(buttons[ButtonType::UNDO], gameState->focusedButton);
	drawCircleButton(buttons[ButtonType::REDO], gameState->focusedButton);
	
	drawBoard();
	drawAllPieces();
	
	drawCurrentTurn(gameState->currentColor);
	
	drawHighLight(gameState->clickedPiece);

	
}

void GuiManager::renderSettingMenu(GameState* gameState) {
	// Write to buffer - first time
	if (!buffer) {
		buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
		SDL_SetRenderTarget(renderer, buffer);
		renderPlaying(gameState);
		addBlendLayer();
		SDL_SetRenderTarget(renderer, NULL);
	}
	// Load from buffer
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, buffer, NULL, NULL);

	SDL_Rect settingBoardRect = { 80, 170, 450, 300 };
	SDL_RenderCopy(renderer, settingBoardTexture, NULL, &settingBoardRect);
	SDL_Rect sliderRect = { 150, 320, 300, 50 };
	SDL_RenderCopy(renderer, sliderVolumnTexture, NULL, &sliderRect);
	
	SDL_Rect dotRect = { 160 + gameState->volumn * (sliderRect.w - 40)/100.0, 322, 20, 40};
	SDL_RenderCopy(renderer, dotVolumnTexture, NULL, &dotRect);
	
	// Buttons
	drawCircleButton(buttons[ButtonType::RESUME], gameState->focusedButton, false);
	drawCircleButton(buttons[ButtonType::RESTART], gameState->focusedButton, false);
	drawCircleButton(buttons[ButtonType::QUIT], gameState->focusedButton, false);
	
	// ...


}

void GuiManager::renderPromotionMenu(GameState* gameState) {
	// Write to buffer - first time
	if (!buffer) {
		buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
		SDL_SetRenderTarget(renderer, buffer);
		renderPlaying(gameState);
		addBlendLayer();
		SDL_SetRenderTarget(renderer, NULL);
	}
	// Load from buffer
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, buffer, NULL, NULL);

	// Buttons
	
	for (int i = ButtonType::QUEEN; i <= ButtonType::ROOK; i++) {
		SDL_DestroyTexture(buttons[i]->texture);
		buttons[i]->texture = nullptr;
	}

	if (gameState->clickedPiece->color == Color::White) {
		buttons[ButtonType::QUEEN]->texture = IMG_LoadTexture(renderer, "../Assets/Pieces/QueenWhite.png");
		buttons[ButtonType::BISHOP]->texture = IMG_LoadTexture(renderer, "../Assets/Pieces/BishopWhite.png");
		buttons[ButtonType::KNIGHT]->texture = IMG_LoadTexture(renderer, "../Assets/Pieces/KnightWhite.png");
		buttons[ButtonType::ROOK]->texture = IMG_LoadTexture(renderer, "../Assets/Pieces/RookWhite.png");
	}
	else {
		buttons[ButtonType::QUEEN]->texture = IMG_LoadTexture(renderer, "../Assets/Pieces/QueenBlack.png");
		buttons[ButtonType::BISHOP]->texture = IMG_LoadTexture(renderer, "../Assets/Pieces/BishopBlack.png");
		buttons[ButtonType::KNIGHT]->texture = IMG_LoadTexture(renderer, "../Assets/Pieces/KnightBlack.png");
		buttons[ButtonType::ROOK]->texture = IMG_LoadTexture(renderer, "../Assets/Pieces/RookBlack.png");
	}


	drawCircleButton(buttons[ButtonType::QUEEN], gameState->focusedButton, false);
	drawCircleButton(buttons[ButtonType::BISHOP], gameState->focusedButton, false);
	drawCircleButton(buttons[ButtonType::KNIGHT], gameState->focusedButton, false);
	drawCircleButton(buttons[ButtonType::ROOK], gameState->focusedButton, false);
	

	// ...



}

void GuiManager::renderMatchResult(GameState* gameState) {
	// Write to buffer - first time
	if (!buffer) {
		buffer = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, SCREEN_WIDTH, SCREEN_HEIGHT);
		SDL_SetRenderTarget(renderer, buffer);
		renderPlaying(gameState);
		addBlendLayer();
		SDL_SetRenderTarget(renderer, NULL);
	}
	// Load from buffer
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, buffer, NULL, NULL);
	SDL_Rect resBoard;
	SDL_Texture* winTexture = nullptr;

	if (gameState->matchResult == 1) {
		resBoard = { 80, 170, 450, 260 };
		winTexture = whiteWinTexture;
	}
	else if (gameState->matchResult == -1) {
		resBoard = { 80, 170, 450, 260 };
		winTexture = blackWinTexture;
	}
	else {
		throw string("I DON'T KNOW WHO WINS!");
	}

	SDL_RenderCopy(renderer, winTexture, NULL, &resBoard);


	// Buttons...
	drawCircleButton(buttons[ButtonType::RESUME], gameState->focusedButton, false);

}

void GuiManager::addBlendLayer() const {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 80);
	SDL_Rect rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

// ---------------------------------

void GuiManager::drawHighLight(Piece* clickedPiece) {
	if (!clickedPiece) return;

	int x = BOARD_OFFSET + BOARD_BORDER + clickedPiece->posX * CELL_SIZE;
	int y = BOARD_OFFSET + BOARD_BORDER + clickedPiece->posY * CELL_SIZE;
	SDL_Rect cellRect = { x, y, CELL_SIZE, CELL_SIZE };

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(nextMoveTexture, 100);
	SDL_SetTextureAlphaMod(killingMoveTexture, 80);
	
	SDL_SetRenderDrawColor(renderer, 101, 181, 85, 100);
	SDL_RenderFillRect(renderer, &cellRect);
	SDL_RenderCopy(renderer, clickedPiece->texture, NULL, &cellRect);

	if (clickedPiece->tableMove.size() > 0) {
		for (int i = 0; i < clickedPiece->tableMove.size() - 1; i += 2) {
			int X = clickedPiece->tableMove[i];
			int Y = clickedPiece->tableMove[i + 1];
			SDL_Rect cellRect = { BOARD_OFFSET + BOARD_BORDER + X * CELL_SIZE + CELL_SIZE / 4, 
									BOARD_OFFSET + BOARD_BORDER + Y * CELL_SIZE + CELL_SIZE / 4,
									CELL_SIZE / 2, CELL_SIZE / 2 };

			(board->piecesOnBoard[X][Y]) ? 
				SDL_RenderCopy(renderer, killingMoveTexture, NULL, &cellRect):
				SDL_RenderCopy(renderer, nextMoveTexture, NULL, &cellRect);
		}
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_RenderPresent(renderer);
}

// ----------------------------------
// Get specific button
Button* GuiManager::getButton(GameState* gameState, int x, int y) const {
	SDL_Rect btnRect;

	switch (gameState->state) {
	case State::MAIN_MENU:
		for (int i = ButtonType::LOAD; i <= ButtonType::COM; i++)
			if (buttons[i]
				&& x > buttons[i]->posX && x < buttons[i]->posX + buttons[i]->width
				&& y > buttons[i]->posY && y < buttons[i]->posY + buttons[i]->height)
				return buttons[i];
		return nullptr; // not implemented yet

	case State::CHOOSE_COLOR:
		return nullptr; // not implemented yet

	case State::PLAYING:
		for (int i = ButtonType::SETTING; i <= ButtonType::REDO; i++) 
			if (buttons[i] 
				&& x > buttons[i]->posX && x < buttons[i]->posX + buttons[i]->width
				&& y > buttons[i]->posY && y < buttons[i]->posY + buttons[i]->height)
				return buttons[i];
		return nullptr;

	case State::SETTING_MENU: // Not enough
		for (int i = ButtonType::VOLUMN; i <= ButtonType::RESUME; i++)
			if (buttons[i]
				&& x > buttons[i]->posX && x < buttons[i]->posX + buttons[i]->width
				&& y > buttons[i]->posY && y < buttons[i]->posY + buttons[i]->height)
				return buttons[i];
		return nullptr;

	case State::PROMOTION:
		for (int i = ButtonType::RESUME; i <= ButtonType::ROOK; i++)
			if (buttons[i]
				&& x > buttons[i]->posX && x < buttons[i]->posX + buttons[i]->width
				&& y > buttons[i]->posY && y < buttons[i]->posY + buttons[i]->height)
				return buttons[i];
		return nullptr;

	case State::MATCH_RESULT:
		for (int i = ButtonType::RESUME; i <= ButtonType::RESUME; i++)
			if (buttons[i]
				&& x > buttons[i]->posX && x < buttons[i]->posX + buttons[i]->width
				&& y > buttons[i]->posY && y < buttons[i]->posY + buttons[i]->height)
				return buttons[i];
		return nullptr;

	default:
		return nullptr;
	}

}

bool GuiManager::isOnBoard(int x, int y) const {
	int boardX = (x - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
	int boardY = (y - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
	return (x - BOARD_OFFSET - BOARD_BORDER >= 0 && y - BOARD_OFFSET - BOARD_BORDER >= 0
		&& boardX >= 0 && boardX <= 7 && boardY >= 0 && boardY <= 7);
}

// --------------------------

void GuiManager::drawAllPieces() {
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8;j++) {
			if (board->piecesOnBoard[i][j]) {
				int x = BOARD_OFFSET + BOARD_BORDER + board->piecesOnBoard[i][j]->posX * CELL_SIZE;
				int y = BOARD_OFFSET + BOARD_BORDER + board->piecesOnBoard[i][j]->posY * CELL_SIZE;
				SDL_Rect pieceRect = { x, y, CELL_SIZE, CELL_SIZE };
				SDL_RenderCopy(renderer, board->piecesOnBoard[i][j]->texture, NULL, &pieceRect);
			}
		}
	}
}

void GuiManager::drawCircleButton(Button* button, Button* focusedButton, bool drawShadow) {
	if (!button) return;
	if (focusedButton == button) {
		std::cout << "Focusing button " << *focusedButton << std::endl;
		// Zoom in the focused button
		focusedButton->posX -= 2;
		focusedButton->posY -= 2;
		focusedButton->width += 2;
		focusedButton->height += 2;
	}

	SDL_Rect btnRect = { button->posX, button->posY, button->width, button->height };
	
	// Draw shadow
	if (drawShadow) {
		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 60);

		int x = btnRect.x + btnRect.w / 2;
		int y = btnRect.y + btnRect.h / 2;
		int r = btnRect.w / 2;
		int segments = 50;
		double step = M_PI / segments;
		for (int i = 0; i < segments; i++) {
			int x1 = x + r * cos(i * step);
			int y1 = y + r * sin(i * step);
			int x2 = x + r * cos((i + 1) * step);
			int y2 = y + r * sin((i + 1) * step);

			SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
		}

		SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	}

	SDL_RenderCopy(renderer, button->texture, NULL, &btnRect);

	if (focusedButton == button) {
		// Normalize focused button
		focusedButton->posX += 2;
		focusedButton->posY += 2;
		focusedButton->width -= 2;
		focusedButton->height -= 2;
	}

}

void GuiManager::drawCurrentTurn(Color turn) {
	SDL_Rect turnRect = { TURN_POS_X, TURN_POS_Y, TURN_WIDTH, TURN_HEIGHT };
	(turn == Color::White) ?
		SDL_RenderCopy(renderer, whiteTurnTexture, NULL, &turnRect) :
		SDL_RenderCopy(renderer, blackTurnTexture, NULL, &turnRect);
}

void GuiManager::drawBoard() {
	SDL_Rect boardRect = { BOARD_OFFSET, BOARD_OFFSET, BOARD_SIZE, BOARD_SIZE };

	// Draw shadow
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 60);
	SDL_Rect shadowRect = { boardRect.x + 10, boardRect.y + 10, BOARD_SIZE, BOARD_SIZE };
	SDL_RenderFillRect(renderer, &shadowRect);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

	// Draw cells
	SDL_RenderCopy(renderer, boardTexture, NULL, &boardRect);
	
}

void GuiManager::drawBackground(State state) {
	SDL_Rect bgRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	if (state == State::MAIN_MENU) {
		SDL_RenderCopy(renderer, mainMenuTexture, NULL, &bgRect);
	}
	else {
		SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);
	}
}
