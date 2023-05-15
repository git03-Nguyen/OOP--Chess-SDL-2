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

	switch (gameState->state) {

	case State::MAIN_MENU:
		// Showing main menu
		cout << "MAIN_MENU is showing ..." << endl;
		break;

	case State::CHOOSE_COLOR:
		// Showing choose player (white/black)
		cout << "CHOOSE_PLAYER is showing ..." << endl;
		break;

	case State::CHOOSE_MODE:
		// Showing choose mode (random/hard)
		cout << "CHOOSE_MODE is showing ..." << endl;
		break;

	// GUI when playing game -> default behind is PLAYING_CHOOSE & PLAYING_MOVE
	default: 
		// Draw background
		drawBackground();
		// Draw board
		drawBoard();
		// Draw other things (menu, buttons, ...)
		drawButtons(gameState->state, gameState->focusedButton);
		drawCurrentTurn(gameState->currentColor);
		// Draw pieces
		drawAllPieces();
		renderHighLight(gameState->clickedPiece);

		switch (gameState->state) {
		case State::SETTING_MENU:
			// Showing menu settings over playing GUI
			renderSettingMenu();
			break;

		case State::PROMOTION:
			// Showing promotion over playing GUI
			cout << "PROMOTION is showing ..." << endl;
			break;

		case State::MATCH_RESULT:
			// Showing who wins
			renderMatchResult(gameState->matchResult);
			break;

		default:
			break;
		}

		break;

	}

	SDL_RenderPresent(renderer);

	// set flag to render -> optimizing performance
	gameState->guiHasChanged = false;

}

void GuiManager::renderHighLight(Piece* clickedPiece) {
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

void GuiManager::renderClickBtn(Button* clickedButton) {
	if (!clickedButton) return;
	std::cout << "Clicked on button " << *clickedButton << std::endl;
	SDL_Rect btnRect;

	switch (clickedButton->type) {
	case ButtonType::SETTING:
		
		break;
	}
}

Button* GuiManager::getButton(GameState* gameState, int x, int y) const {

	switch (gameState->state) {
	case State::MAIN_MENU:
		return getButtonMainMenu(x, y);
		
	case State::CHOOSE_MODE:
		return getButtonChooseMode(x, y);

	case State::CHOOSE_COLOR:
		return getButtonChooseColor(x, y);

	case State::PLAYING:
		return getButtonPlaying(x, y);

	case State::SETTING_MENU:
		return getButtonSetting(x, y);

	case State::PROMOTION:
		return getButtonPromotion(x, y);

	case State::MATCH_RESULT:
		return getButtonMatchResult(x, y);

	default:
		return nullptr;
	}

}

bool GuiManager::isOnBoard(int x, int y) {
	int boardX = (x - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
	int boardY = (y - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
	return (x - BOARD_OFFSET - BOARD_BORDER >= 0 && y - BOARD_OFFSET - BOARD_BORDER >= 0
		&& boardX >= 0 && boardX <= 7 && boardY >= 0 && boardY <= 7);
}

// ----------------------------------
// Get specific button

Button* GuiManager::getButtonMainMenu(int x, int y) const {
	return nullptr;
}

Button* GuiManager::getButtonChooseMode(int x, int y) const {
	return nullptr;
}

Button* GuiManager::getButtonChooseColor(int x, int y) const {
	return nullptr;
}

Button* GuiManager::getButtonPlaying(int x, int y) const {
	SDL_Rect btnRect;

	for (auto& button : buttons) {
		if (!button) continue;
		btnRect = { button->posX, button->posY, button->size, button->size };
		if (x > btnRect.x && x < btnRect.x + btnRect.w
			&& y > btnRect.y && y < btnRect.y + btnRect.h)
			return button;
	}

	return nullptr;
}

Button* GuiManager::getButtonSetting(int x, int y) const {
	return nullptr;
}

Button* GuiManager::getButtonPromotion(int x, int y) const {
	return nullptr;
}

Button* GuiManager::getButtonMatchResult(int x, int y) const {
	return nullptr;
}

// ----------------------

void GuiManager::addBlendLayer() const {
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
	SDL_Rect rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	SDL_RenderFillRect(renderer, &rect);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void GuiManager::renderMatchResult(int matchResult) const {
	cout << "MATCH_RESULT is showing ... " << endl;
	if (matchResult == 1) {
		cout << "WHITE WINS!!!!!!!!!!!!" << endl;
	}
	else if (matchResult == -1) {
		cout << "BLACK WINS!!!!!!!!!!!!!" << endl;
	}
	else {
		cout << "I DON'T KNOW WHO WINS!!!!" << endl;
	}

	addBlendLayer();

	// Result ....

}

void GuiManager::renderSettingMenu() const {
	cout << "SETTING_MENU is showing ... " << endl;
	
	addBlendLayer();

	// Settings menu UI: UI, buttons, ...



}

// --------------------------

void GuiManager::drawAllPieces() {
	for (auto& row : board->piecesOnBoard) {
		for (auto& p : row) {
			if (p) {
				int x = BOARD_OFFSET + BOARD_BORDER + p->posX * CELL_SIZE;
				int y = BOARD_OFFSET + BOARD_BORDER + p->posY * CELL_SIZE;
				SDL_Rect pieceRect = { x, y, CELL_SIZE, CELL_SIZE };
				SDL_RenderCopy(renderer, p->texture, NULL, &pieceRect);
			}
		}
	}
}

void GuiManager::drawButtons(State gameState, Button* focusedButton) {
	if (focusedButton) {
		std::cout << "Focusing button " << *focusedButton << std::endl;
		// Zoom out the focused button
		focusedButton->posX -= 2;
		focusedButton->posY -= 2;
		focusedButton->size += 2;
	}
	
	drawCircleButton(buttons[ButtonType::SETTING]);
	drawCircleButton(buttons[ButtonType::UNDO]);
	drawCircleButton(buttons[ButtonType::REDO]);

	if (focusedButton) {
		focusedButton->posX += 2;
		focusedButton->posY += 2;
		focusedButton->size -= 2;
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
