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
	nextMoveTexture = IMG_LoadTexture(renderer, "../Assets/Buttons/next_move.png");
	killingMoveTexture = IMG_LoadTexture(renderer, "../Assets/Buttons/killing_move.png");
	settingsTexture = IMG_LoadTexture(renderer, "../Assets/Buttons/settings.png");
	undoTexture = IMG_LoadTexture(renderer, "../Assets/Buttons/undo.png");
	redoTexture = IMG_LoadTexture(renderer, "../Assets/Buttons/redo.png");

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
	SDL_DestroyTexture(nextMoveTexture);
	SDL_DestroyTexture(settingsTexture);
	SDL_DestroyTexture(undoTexture);
	SDL_DestroyTexture(redoTexture);
	SDL_DestroyRenderer(renderer);
	TTF_Quit();
	IMG_Quit();
}

void GuiManager::render(Color turn) {
	SDL_RenderClear(renderer);
	
	// Draw background
	drawBackground();
	// Draw board
	drawBoard();
	// Draw other things (menu, buttons, ...)
	drawButtonMenu();
	drawCurrentTurn(turn);

	// Draw pieces
	drawPieces();

	SDL_RenderPresent(renderer);

	//std::cout << "Refresh screen" << std::endl;
}

void GuiManager::renderHighLight(Piece* piece) {
	std::cout << "Hearing next move\n";

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
	SDL_Rect btnRect;

	switch (btn) {
	case Button::SETTINGS:
		
		break;
	}
}

bool GuiManager::isOverButton(int x, int y) {
	// BtnSettings
	SDL_Rect btnRect = { 625, 500, 60, 60 };
	if (x > btnRect.x && x < btnRect.x + btnRect.w
		&& y > btnRect.y && y < btnRect.y + btnRect.h)
		return true;

	// BtnUndo

	// BtnRedo

	return false;
}

bool GuiManager::isOverBoard(int x, int y) {
	int boardX = (x - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
	int boardY = (y - BOARD_OFFSET - BOARD_BORDER) / CELL_SIZE;
	return (x - BOARD_OFFSET - BOARD_BORDER >= 0 && y - BOARD_OFFSET - BOARD_BORDER >= 0
		&& boardX >= 0 && boardX <= 7 && boardY >= 0 && boardY <= 7);
}

Button GuiManager::getButton(int x, int y) {
	// BtnSettings
	SDL_Rect btnRect = { 625,500,60, 60 };
	if (x > btnRect.x && x < btnRect.x + btnRect.w
		&& y > btnRect.y && y < btnRect.y + btnRect.h)
		return Button::SETTINGS;

	// BtnUndo

	// BtnRedo
}

void GuiManager::drawPieces() {
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

void GuiManager::drawButtonMenu() {
	drawButton(settingsTexture, 60, 625, 500);
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
