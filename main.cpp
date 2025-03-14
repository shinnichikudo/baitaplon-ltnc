#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include "sound.h"  // Đã sửa lỗi và cập nhật sound.h
#include <fstream>
#include <SDL_ttf.h>
using namespace std;
const int MAX_TIME = 60;  // ⏳ Giới hạn thời gian (60 giây)
Uint32 startTime = 0;     // Lưu thời điểm bắt đầu game
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
TTF_Font* font = nullptr; // Khai báo biến font
SDL_Color textColor = {0, 0, 0, 255}; // Màu chữ (đen)
int firstCard = -1, secondCard = -1;
bool waiting = false;
Uint32 waitStart = 0;
bool gameStarted = false;
bool gameWon = false;
int flipCount = 0;  // Đếm số lần lật bài
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
    SDL_Texture* background = loadTexture("background.jpg");
    SDL_Texture* startButton = loadTexture("image7.png");

    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect startButtonRect = {(SCREEN_WIDTH - 200) / 2, (SCREEN_HEIGHT - 100) / 2, 200, 100};

    SDL_RenderCopy(renderer, background, nullptr, &bgRect);
    SDL_RenderCopy(renderer, startButton, nullptr, &startButtonRect);
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(startButton);
}
void renderText(const string& text, int x, int y) {
    if (!font) font = TTF_OpenFont("arial.ttf", 24); // Load font Arial
    if (!font) return;

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}
void renderTime(Uint32 startTime) {
    if (!font) {
        font = TTF_OpenFont("arial.ttf", 24);
        if (!font) {
            cout << "Lỗi tải font Arial: " << TTF_GetError() << endl;
            return;
        }
    }

    if (!gameStarted) return;  // Không hiển thị nếu game chưa bắt đầu

    // Tính thời gian còn lại
    Uint32 elapsedTime = (SDL_GetTicks() - startTime) / 1000;
    int countdown = MAX_TIME - elapsedTime;

    if (countdown <= 0) countdown = 0;  // Không hiển thị số âm

    string timeText = "Time: " + to_string(countdown) + "s";
    renderText(timeText, 10, 10);
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
                SDL_SetRenderDrawColor(renderer, 200, 200, 200, 200);
                SDL_RenderFillRect(renderer, &rect);
            }

            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }
       renderTime(startTime);
    SDL_RenderPresent(renderer);
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

int readHighScore() {
    ifstream file("highscore.txt");
    int highScore = 9999;  // Mặc định số lớn để dễ cập nhật kỷ lục mới
    if (file >> highScore) {
        file.close();
    }
    return highScore;
}

void saveHighScore(int flips) {
    int highScore = readHighScore();
    if (flips < highScore) {  // Chỉ lưu nếu số lần lật ít hơn
        ofstream file("highscore.txt");
        file << flips;
        file.close();
    }
}
void renderWinScreen() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Rect rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, winnerImage, nullptr, &rect);

    saveHighScore(flipCount);
    int bestRecord = readHighScore();

    string flipText = "So lan lat: " + to_string(flipCount);
    string recordText = "Best Record: " + to_string(bestRecord) + " flips";

    // Căn giữa màn hình
    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;



    // Đảm bảo text được hiển thị
    renderText(flipText, centerX - 100, centerY - 50);
    renderText(recordText, centerX - 120, centerY + 10);

    SDL_RenderPresent(renderer);
    SDL_Delay(3000);
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
        flipCount++;  // ✅ Tăng số lần lật khi chọn đủ 1 cặp
    }
}
void renderGameOverScreen() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // 📌 Load ảnh Game Over
    SDL_Texture* gameOverImage = loadTexture("D:\\ảnh sdl\\gameover.jpg");

    // Hiển thị ảnh toàn màn hình
    SDL_Rect rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, gameOverImage, nullptr, &rect);
    SDL_RenderPresent(renderer);

    // Giữ màn hình Game Over trong 3 giây
    SDL_Delay(3000);

    // Giải phóng bộ nhớ
    SDL_DestroyTexture(gameOverImage);
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
    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) {
        cout << "SDL_ttf could not initialize! SDL_ttf Error: " << TTF_GetError() << endl;
        return 1;
    }

    // Load font
    TTF_Font* font = TTF_OpenFont("arial.ttf", 28);  // Cần đúng đường dẫn file
    if (!font) {
        cout << "Failed to load font! SDL_ttf Error: " << TTF_GetError() << endl;
        return 1;
    }

    sound.playBackgroundMusic();  // Phát nhạc nền khi vào game

    window = SDL_CreateWindow("Memory Card Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    startButtonImage = loadTexture("image7.png");
    winnerImage = loadTexture("winner.png");

    for (int i = 0; i < TOTAL_CARDS; i++) {
        string path = "image" + to_string(i + 1) + ".jpg";
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
            startTime = SDL_GetTicks();  // ⏳ Bắt đầu đếm thời gian
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
        // ⏳ Kiểm tra thời gian còn lại
        Uint32 elapsedTime = (SDL_GetTicks() - startTime) / 1000;
        int countdown = MAX_TIME - elapsedTime;

        if (countdown <= 0) {
            // ⏳ Hết giờ -> Thua game
            renderGameOverScreen();
            SDL_Delay(2000);  // Chờ 2 giây trước khi thoát game
            running = false;
        }

        if (waiting && SDL_GetTicks() - waitStart > 1000) {
            checkMatch();
        }

        if (checkWin()) {
            sound.stopBackgroundMusic();  // Dừng nhạc nền
            sound.playWinSound();         // Phát âm thanh khi chiến thắng
            renderWinScreen();

            saveHighScore(flipCount);  // ✅ Lưu kỷ lục nếu có

            SDL_Delay(2000);  // ✅ Chờ 2 giây trước khi thoát game
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
