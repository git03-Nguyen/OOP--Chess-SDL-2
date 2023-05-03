#include "GuiManager.h"

GuiManager::GuiManager(SDL_Window* _window, Board* _board) : window(_window), board(_board) {
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);
	TTF_Init();

	SDL_Point size;
	SDL_GetWindowSize(window, &size.x, &size.y);
	SCREEN_WIDTH = size.x;
	SCREEN_HEIGHT = size.y;
	CELL_SIZE = (SCREEN_HEIGHT - BOARD_OFFSET_X * 2) / 8;
	SIDEBAR_WIDTH = SCREEN_WIDTH - BOARD_OFFSET_X - CELL_SIZE * 8;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer) std::cout << "Renderer created!" << std::endl; else throw "Cannot create renderer!";

	background = IMG_LoadTexture(renderer, "../Assets/Background/background.jpg");
	settingsButton = IMG_LoadTexture(renderer, "../Assets/Buttons/settings.png");
	undoButton = IMG_LoadTexture(renderer, "../Assets/Buttons/undo.png");
	redoButton = IMG_LoadTexture(renderer, "../Assets/Buttons/redo.png");

	for (auto& row : board->piecesOnBoard) {
		for (auto& piece : row) {
			if (piece) {
				piece->setTexture(renderer);
			}
		}
	}

}

GuiManager::~GuiManager() {
	SDL_DestroyTexture(background);
	SDL_DestroyTexture(settingsButton);
	SDL_DestroyTexture(undoButton);
	SDL_DestroyTexture(redoButton);
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

	int x = BOARD_OFFSET_X + piece->posX * CELL_SIZE;
	int y = BOARD_OFFSET_Y + piece->posY * CELL_SIZE;
	((piece->posX + piece->posY) % 2 == 0) ? 
		SDL_SetRenderDrawColor(renderer, 255, 255, 204, 10) : SDL_SetRenderDrawColor(renderer, 200, 77, 0, 10);
	SDL_Rect cellRect = { x, y, CELL_SIZE, CELL_SIZE };
	SDL_RenderFillRect(renderer, &cellRect);

	SDL_RenderCopy(renderer, piece->texture, NULL, &cellRect);

	if (piece->tableMove.size() > 0) {
		for (int i = 0; i < piece->tableMove.size() - 1; i += 2) {
			int X = piece->tableMove[i];
			int Y = piece->tableMove[i + 1];
			((X + Y) % 2 == 0) ?
				SDL_SetRenderDrawColor(renderer, 230, 230, 230, 10) : SDL_SetRenderDrawColor(renderer, 200, 100, 0, 10);
			SDL_Rect cellRect = { BOARD_OFFSET_X + X * CELL_SIZE, BOARD_OFFSET_Y + Y * CELL_SIZE, CELL_SIZE, CELL_SIZE };
			SDL_RenderFillRect(renderer, &cellRect);

			if (board->piecesOnBoard[X][Y]) {
				SDL_RenderCopy(renderer, board->piecesOnBoard[X][Y]->texture, NULL, &cellRect);
			}

			/*SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
			SDL_RenderDrawRect(renderer, &cellRect);*/
		}
	}
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
	int boardX = (x - BOARD_OFFSET_X) / CELL_SIZE;
	int boardY = (y - BOARD_OFFSET_Y) / CELL_SIZE;
	return (x - BOARD_OFFSET_X >= 0 && y - BOARD_OFFSET_Y >= 0
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
				int x = BOARD_OFFSET_X + p->posX * CELL_SIZE;
				int y = BOARD_OFFSET_Y + p->posY * CELL_SIZE;
				SDL_Rect pieceRect = { x, y, CELL_SIZE, CELL_SIZE };
				SDL_RenderCopy(renderer, p->texture, NULL, &pieceRect);
			}
		}
	}
}

void GuiManager::drawButtonMenu() {
	drawButton(settingsButton, 60, 625, 500);
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
	SDL_Rect boardRect = { BOARD_OFFSET_X, BOARD_OFFSET_Y, CELL_SIZE * 8, CELL_SIZE * 8 };

	// Draw shadow
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 60);
	SDL_Rect shadowRect = { boardRect.x + 10, boardRect.y + 10, CELL_SIZE * 8, CELL_SIZE * 8 };
	SDL_RenderFillRect(renderer, &shadowRect);
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

	// Draw cells
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			SDL_Rect cellRect = { BOARD_OFFSET_X + j * CELL_SIZE, BOARD_OFFSET_Y + i * CELL_SIZE, CELL_SIZE, CELL_SIZE };
			if ((i + j) % 2 == 0) {
				SDL_SetRenderDrawColor(renderer, 255, 230, 204, 255);
			}
			else {
				SDL_SetRenderDrawColor(renderer, 153, 77, 0, 255);
			}
			SDL_RenderFillRect(renderer, &cellRect);

		}
	}
	
}

void GuiManager::drawBackground() {
	SDL_Rect bgRect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	SDL_RenderCopy(renderer, background, NULL, &bgRect);
}
