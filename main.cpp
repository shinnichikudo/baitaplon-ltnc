#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include "sound.h"  // Đã sửa lỗi và cập nhật sound.h

using namespace std;

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int ROWS = 4, COLS = 4;
const int CARD_SIZE = 150;
const int TOTAL_CARDS = ROWS * COLS / 2;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* startButtonImage = nullptr;
SDL_Texture* winnerImage = nullptr;
vector<SDL_Texture*> images;
vector<int> board(ROWS * COLS, -1);
vector<bool> flipped(ROWS * COLS, false);

int firstCard = -1, secondCard = -1;
bool waiting = false;
Uint32 waitStart = 0;
bool gameStarted = false;
bool gameWon = false;

SoundManager sound;  // Đối tượng quản lý âm thanh

SDL_Texture* loadTexture(const string& path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        cout << "❌ Lỗi tải ảnh từ " << path << ": " << IMG_GetError() << endl;
        exit(-1);
    }
    return texture;
}

void renderStartScreen() {
    SDL_Texture* background = loadTexture("D:\\ảnh sdl\\background.jpg");
    SDL_Texture* startButton = loadTexture("D:\\ảnh sdl\\image7.png");

    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect startButtonRect = {(SCREEN_WIDTH - 200) / 2, (SCREEN_HEIGHT - 100) / 2, 200, 100};

    SDL_RenderCopy(renderer, background, nullptr, &bgRect);
    SDL_RenderCopy(renderer, startButton, nullptr, &startButtonRect);
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(startButton);
}

void shuffleBoard() {
    vector<int> pairs;
    for (int i = 0; i < TOTAL_CARDS; i++) {
        pairs.push_back(i);
        pairs.push_back(i);
    }
    random_shuffle(pairs.begin(), pairs.end());
    for (int i = 0; i < ROWS * COLS; i++) {
        board[i] = pairs[i];
    }
}

void renderGame() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int index = i * COLS + j;
            SDL_Rect rect = { j * CARD_SIZE, i * CARD_SIZE, CARD_SIZE, CARD_SIZE };

            if (flipped[index] && board[index] >= 0 && images[board[index]]) {
                SDL_RenderCopy(renderer, images[board[index]], nullptr, &rect);
            } else {
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &rect);
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}

void handleMouseClick(int x, int y) {
    if (waiting) return;

    int col = x / CARD_SIZE;
    int row = y / CARD_SIZE;
    int index = row * COLS + col;

    if (index < 0 || index >= ROWS * COLS || flipped[index] || board[index] < 0) return;

    flipped[index] = true;
    if (firstCard == -1) {
        firstCard = index;
    } else {
        secondCard = index;
        waiting = true;
        waitStart = SDL_GetTicks();
    }
}

void checkMatch() {
    if (board[firstCard] != board[secondCard]) {
        flipped[firstCard] = false;
        flipped[secondCard] = false;
    }
    firstCard = secondCard = -1;
    waiting = false;
}

bool checkWin() {
    for (bool flip : flipped) {
        if (!flip) return false;
    }
    return true;
}

void renderWinScreen() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Rect rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, winnerImage, nullptr, &rect);
    SDL_RenderPresent(renderer);

    SDL_Delay(2000);
    cout << "🎉 Chúc mừng! Bạn đã chiến thắng!" << endl;
}

int main(int argc, char* argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 || !(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
        cout << "❌ Lỗi khởi tạo SDL hoặc SDL_image!" << endl;
        return -1;
    }

    if (!sound.init()) {
        cout << "❌ Lỗi khởi tạo âm thanh!" << endl;
        return -1;
    }

    sound.playBackgroundMusic();  // Phát nhạc nền khi vào game

    window = SDL_CreateWindow("Memory Card Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    startButtonImage = loadTexture("D:\\ảnh sdl\\image7.png");
    winnerImage = loadTexture("D:\\ảnh sdl\\winner.png");

    for (int i = 0; i < TOTAL_CARDS; i++) {
        string path = "D:\\ảnh sdl\\image" + to_string(i + 1) + ".jpg";
        images.push_back(loadTexture(path));
    }

    shuffleBoard();
    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;

            if (!gameStarted && e.type == SDL_MOUSEBUTTONDOWN) {
                gameStarted = true;
                shuffleBoard();
                sound.playStartSound();  // Phát âm thanh khi ấn nút Start
            }

            if (gameStarted && e.type == SDL_MOUSEBUTTONDOWN) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                handleMouseClick(x, y);
            }
        }

        if (!gameStarted) {
            renderStartScreen();
        } else {
            if (waiting && SDL_GetTicks() - waitStart > 1000) {
                checkMatch();
            }

            if (checkWin()) {
                sound.stopBackgroundMusic();  // Dừng nhạc nền
                sound.playWinSound();         // Phát âm thanh khi chiến thắng
                renderWinScreen();
                running = false;
            } else {
                renderGame();
            }
        }

        SDL_Delay(16);
    }

    sound.cleanUp();  // Giải phóng tài nguyên âm thanh

    SDL_DestroyTexture(startButtonImage);
    SDL_DestroyTexture(winnerImage);
    for (SDL_Texture* tex : images) {
        SDL_DestroyTexture(tex);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
