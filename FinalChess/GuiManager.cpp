#include "GuiManager.h"

GuiManager::GuiManager(SDL_Window* _window, Board* _board) : window(_window), board(_board) {
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	TTF_Init();

	SDL_Point size;
	SDL_GetWindowSize(window, &size.x, &size.y);
	SCREEN_WIDTH = size.x;
	SCREEN_HEIGHT = size.y;
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
	buttonTexture[(int)Button::SETTINGS] = IMG_LoadTexture(renderer, "../Assets/Buttons/settings.png");
	buttonTexture[(int)Button::UNDO] = IMG_LoadTexture(renderer, "../Assets/Buttons/undo.png");
	buttonTexture[(int)Button::REDO] = IMG_LoadTexture(renderer, "../Assets/Buttons/redo.png");

	for (auto& row : board->piecesOnBoard) {
		for (auto& piece : row) {
			if (piece) {
				piece->setTexture(renderer);
			}
		}
	}

}

GuiManager::~GuiManager() {
	SDL_DestroyTexture(bgTexture);
	SDL_DestroyTexture(boardTexture);
	SDL_DestroyTexture(blackTurnTexture);
	SDL_DestroyTexture(whiteTurnTexture);
	SDL_DestroyTexture(nextMoveTexture);
	for (auto& texture : buttonTexture) {
		SDL_DestroyTexture(texture);
	}
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	IMG_Quit();
}

void GuiManager::render(Color turn, Piece* clickedPiece, Button focusingBtn) {
	SDL_RenderClear(renderer);
	
	// Draw background
	drawBackground();

	// Draw board
	drawBoard();
	
	// Draw other things (menu, buttons, ...)
	drawButtons();
	drawCurrentTurn(turn);
	drawButtonFocused(focusingBtn);

	// Draw pieces
	drawAllPieces();
	renderHighLight(clickedPiece);

	SDL_RenderPresent(renderer);

	std::cout << "Refresh screen" << std::endl;
}

void GuiManager::renderHighLight(Piece* piece) {
	if (!piece) return;

	int x = BOARD_OFFSET + BOARD_BORDER + piece->posX * CELL_SIZE;
	int y = BOARD_OFFSET + BOARD_BORDER + piece->posY * CELL_SIZE;
	SDL_Rect cellRect = { x, y, CELL_SIZE, CELL_SIZE };

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetTextureAlphaMod(nextMoveTexture, 100);
	SDL_SetTextureAlphaMod(killingMoveTexture, 80);
	
	SDL_SetRenderDrawColor(renderer, 101, 181, 85, 100);
	SDL_RenderFillRect(renderer, &cellRect);
	SDL_RenderCopy(renderer, piece->texture, NULL, &cellRect);

	if (piece->tableMove.size() > 0) {
		for (int i = 0; i < piece->tableMove.size() - 1; i += 2) {
			int X = piece->tableMove[i];
			int Y = piece->tableMove[i + 1];
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

void GuiManager::renderClickBtn(Button btn) {
	std::cout << "Clicked on button " << (int)btn << std::endl;
	SDL_Rect btnRect;

	switch (btn) {
	case Button::SETTINGS:
		
		break;
	}
}

Button GuiManager::getButton(int x, int y) {
	SDL_Rect btnRect;

	// BtnSettings
	btnRect = { SETTING_POS_X, SETTING_POS_Y, BUTTON_SIZE, BUTTON_SIZE };
	if (x > btnRect.x && x < btnRect.x + btnRect.w
		&& y > btnRect.y && y < btnRect.y + btnRect.h)
		return Button::SETTINGS;

	// BtnUndo
	btnRect = { UNDO_POS_X, UNDO_POS_Y, BUTTON_SIZE, BUTTON_SIZE };
	if (x > btnRect.x && x < btnRect.x + btnRect.w
		&& y > btnRect.y && y < btnRect.y + btnRect.h)
		return Button::UNDO;

	// BtnRedo
	btnRect = { REDO_POS_X, REDO_POS_Y, BUTTON_SIZE, BUTTON_SIZE };
	if (x > btnRect.x && x < btnRect.x + btnRect.w
		&& y > btnRect.y && y < btnRect.y + btnRect.h)
		return Button::REDO;

	return Button::NONE;
}

bool GuiManager::isOnBoard(int x, int y) {
	int boardX = (x - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
	int boardY = (y - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
	return (x - BOARD_OFFSET - BOARD_BORDER >= 0 && y - BOARD_OFFSET - BOARD_BORDER >= 0
		&& boardX >= 0 && boardX <= 7 && boardY >= 0 && boardY <= 7);
}

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

void GuiManager::drawButtons() {
	drawButton(buttonTexture[(int)Button::SETTINGS], BUTTON_SIZE, SETTING_POS_X, SETTING_POS_Y);
	drawButton(buttonTexture[(int)Button::UNDO], BUTTON_SIZE, UNDO_POS_X, UNDO_POS_Y);
	drawButton(buttonTexture[(int)Button::REDO], BUTTON_SIZE, REDO_POS_X, REDO_POS_Y);

}

void GuiManager::drawButton(SDL_Texture* texture, int size, int posX, int posY) {
	SDL_Rect btnRect = { posX, posY, size, size };
	SDL_RenderCopy(renderer, texture, NULL, &btnRect);

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

void GuiManager::drawButtonFocused(Button focusingBtn) {
	if (focusingBtn == Button::NONE) return;

	std::cout << "Focusing button " << (int)focusingBtn << std::endl;
	SDL_Rect btnRect = { 0,0,BUTTON_SIZE + 5,BUTTON_SIZE + 5 };
	SDL_RenderCopy(renderer, buttonTexture[(int)focusingBtn], NULL, &btnRect);

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
