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

	bgTexture = IMG_LoadTexture(renderer, "../Assets/Background/background.jpg");
	boardTexture = IMG_LoadTexture(renderer, "../Assets/Background/brown_board.jpg");
	blackTurnTexture = IMG_LoadTexture(renderer, "../Assets/Background/black_move.png");
	whiteTurnTexture = IMG_LoadTexture(renderer, "../Assets/Background/white_move.png");
	nextMoveTexture = IMG_LoadTexture(renderer, "../Assets/Buttons/next_move.png");
	killingMoveTexture = IMG_LoadTexture(renderer, "../Assets/Buttons/killing_move.png");
	
	// new buttons
	buttons.resize(15);
	for (auto& button : buttons) button = nullptr;
	buttons[ButtonType::SETTING] = new Button(SETTING_POS_X, SETTING_POS_Y, BUTTON_SIZE, ButtonType::SETTING);
	buttons[ButtonType::SETTING]->texture = IMG_LoadTexture(renderer, "../Assets/Buttons/settings.png");
	buttons[ButtonType::UNDO] = new Button(UNDO_POS_X, UNDO_POS_Y, BUTTON_SIZE, ButtonType::UNDO);
	buttons[ButtonType::UNDO]->texture = IMG_LoadTexture(renderer, "../Assets/Buttons/undo.png");
	buttons[ButtonType::REDO] = new Button(REDO_POS_X, REDO_POS_Y, BUTTON_SIZE, ButtonType::REDO);
	buttons[ButtonType::REDO]->texture = IMG_LoadTexture(renderer, "../Assets/Buttons/redo.png");
	// ...

	// pieces' textures
	for (auto& row : board->piecesOnBoard) {
		for (auto& piece : row) {
			if (piece) piece->setTexture(renderer);
		}
	}

}

GuiManager::~GuiManager() {
	SDL_DestroyTexture(bgTexture);
	SDL_DestroyTexture(boardTexture);
	SDL_DestroyTexture(blackTurnTexture);
	SDL_DestroyTexture(whiteTurnTexture);
	SDL_DestroyTexture(nextMoveTexture);
	for (auto& button : buttons) if (button) delete button;
	buttons.clear();
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	IMG_Quit();
}

void GuiManager::render(GameState* gameState) {
	if (!gameState->guiHasChanged) return;

	SDL_RenderClear(renderer);
	drawBackground();

	switch (gameState->state) {
	case State::MAIN_MENU:
		renderMainMenu(gameState);
		break;

	case State::CHOOSE_OPPONENT:
		renderChooseOpponent(gameState);
		break;

	case State::CHOOSE_DIFFICULTY:
		renderChooseDifficulty(gameState);
		break;

	case State::CHOOSE_COLOR:
		renderChooseColor(gameState);
		break;

	case State::PLAYING:
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
	cout << "SETTING_MENU is showing ... " << endl;
	// ...
}

void GuiManager::renderChooseOpponent(GameState* gameState) {
	cout << "CHOOSE_OPPONENT is showing ... " << endl;
	// ...
}

void GuiManager::renderChooseDifficulty(GameState* gameState) {
	renderChooseOpponent(gameState);
	cout << "CHOOOSE_DIFFICULTY is showing ... " << endl;
	// ...
}

void GuiManager::renderChooseColor(GameState* gameState) {
	cout << "CHOOOSE_COLOR is showing ... " << endl;
	// ...
}

void GuiManager::renderPlaying(GameState* gameState) {
	if (gameState->focusedButton) {
		std::cout << "Focusing button " << *gameState->focusedButton << std::endl;
		// Zoom out the focused button
		gameState->focusedButton->posX -= 2;
		gameState->focusedButton->posY -= 2;
		gameState->focusedButton->size += 2;
	}

	drawCircleButton(buttons[ButtonType::SETTING]);
	drawCircleButton(buttons[ButtonType::UNDO]);
	drawCircleButton(buttons[ButtonType::REDO]);

	if (gameState->focusedButton) {
		gameState->focusedButton->posX += 2;
		gameState->focusedButton->posY += 2;
		gameState->focusedButton->size -= 2;
	}
	
	drawBoard();
	drawAllPieces();
	
	drawCurrentTurn(gameState->currentColor);
	
	drawHighLight(gameState->clickedPiece);

}

void GuiManager::renderSettingMenu(GameState* gameState) {
	renderPlaying(gameState);
	addBlendLayer();
	cout << "SETTING_MENU is showing ... " << endl;
	// ...
}

void GuiManager::renderPromotionMenu(GameState* gameState) {
	renderPlaying(gameState);
	addBlendLayer();
	cout << "PROMOTION is showing ... " << endl;
	// ...
}

void GuiManager::renderMatchResult(GameState* gameState) {
	renderPlaying(gameState);
	addBlendLayer();
	cout << "MATCH_RESULT is showing ... " << endl;
	// Result ....
	if (gameState->matchResult == 1) {
		cout << "WHITE WINS!!!!!!!!!!!!" << endl;
	}
	else if (gameState->matchResult == -1) {
		cout << "BLACK WINS!!!!!!!!!!!!!" << endl;
	}
	else {
		cout << "I DON'T KNOW WHO WINS!!!!" << endl;
	}
}

void GuiManager::addBlendLayer() const {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
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
		return nullptr; // not implemented yet
		
	case State::CHOOSE_OPPONENT:
		return nullptr; // not implemented yet

	case State::CHOOSE_COLOR:
		return nullptr; // not implemented yet

	case State::PLAYING:
		for (int i = ButtonType::SETTING; i <= ButtonType::REDO; i++) 
			if (buttons[i] 
				&& x > buttons[i]->posX && x < buttons[i]->posX + buttons[i]->size
				&& y > buttons[i]->posY && y < buttons[i]->posY + buttons[i]->size)
				return buttons[i];
		return nullptr;

	case State::SETTING_MENU: // Not enough
		for (int i = ButtonType::BACK; i <= ButtonType::BACK; i++)
			if (buttons[i]
				&& x > buttons[i]->posX && x < buttons[i]->posX + buttons[i]->size
				&& y > buttons[i]->posY && y < buttons[i]->posY + buttons[i]->size)
				return buttons[i];
		return nullptr;

	case State::PROMOTION:
		for (int i = ButtonType::QUEEN; i <= ButtonType::ROOK; i++)
			if (buttons[i]
				&& x > buttons[i]->posX && x < buttons[i]->posX + buttons[i]->size
				&& y > buttons[i]->posY && y < buttons[i]->posY + buttons[i]->size)
				return buttons[i];
		return nullptr;

	case State::MATCH_RESULT:
		return nullptr; // not implemented yet

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

void GuiManager::drawCircleButton(Button* button) {
	if (!button) return;
	SDL_Rect btnRect = { button->posX, button->posY, button->size, button->size };
	SDL_RenderCopy(renderer, button->texture, NULL, &btnRect);

	// Draw shadow
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

void GuiManager::drawBackground() {
	SDL_Rect bgRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	SDL_RenderCopy(renderer, bgTexture, NULL, &bgRect);
}
