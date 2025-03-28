#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>
#include "sound.h"  // Thư viện xử lý âm thanh riêng
#include <fstream>
#include <SDL_ttf.h>

using namespace std;

// Khai báo các hằng số cho game
const int MAX_TIME = 60;  // Giới hạn thời gian chơi (giây)
Uint32 startTime = 0;     // Lưu thời điểm bắt đầu game
const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int ROWS = 4, COLS = 4;  // Số hàng và cột của bàn cờ
const int CARD_SIZE = 150;    // Kích thước mỗi ô bài
const int TOTAL_CARDS = ROWS * COLS / 2;  // Tổng số cặp bài
const int BUTTON_WIDTH = 200;
const int BUTTON_HEIGHT = 100;

// Biến trạng thái của game
int matchedCount = 0;
bool cardShown[ROWS * COLS] = {false};
bool cardMatched[ROWS * COLS] = {false};
bool replayRequested = false;
bool justStarted = false;
SDL_Texture* pauseTexture = nullptr;
bool isPaused = false;
Uint32 pausedTime = 0;
SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
SDL_Texture* startButtonImage = nullptr;
SDL_Texture* winnerImage = nullptr;
SDL_Texture* backImage = nullptr;

vector<SDL_Texture*> images;
vector<int> board(ROWS * COLS, -1);  // Lưu thứ tự bài đã xáo
vector<bool> flipped(ROWS * COLS, false);  // Trạng thái lật của các lá bài

TTF_Font* font = nullptr;  // Font chữ
SDL_Color textColor = {255, 0, 0, 255}; // Màu chữ đỏ

int firstCard = -1, secondCard = -1;
bool waiting = false;
Uint32 waitStart = 0;
bool gameStarted = false;
bool gameWon = false;
int flipCount = 0;  // Biến đếm số lần lật bài

SoundManager sound;  // Đối tượng quản lý âm thanh
// Hàm tải ảnh và tạo texture
SDL_Texture* loadTexture(const string& path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        cout << "Loi tai anh tu " << path << ": " << IMG_GetError() << endl;
        exit(-1);
    }
    return texture;
}

// Hiển thị màn hình bắt đầu với nút Start
void renderStartScreen() {
    SDL_Texture* background = loadTexture("background.jpg");
    SDL_Texture* startButton = loadTexture("image7.png");

    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect startButtonRect = {
        (SCREEN_WIDTH - 200) / 2,
        (SCREEN_HEIGHT - 100) / 2 + 80,
        200,
        100
    };

    SDL_RenderCopy(renderer, background, nullptr, &bgRect);
    SDL_RenderCopy(renderer, startButton, nullptr, &startButtonRect);
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(startButton);
}

// Hàm vẽ chữ lên màn hình
void renderText(const string& text, int x, int y) {
    if (!font) font = TTF_OpenFont("arial.ttf", 24);
    if (!font) return;

    SDL_Surface* textSurface = TTF_RenderText_Solid(font, text.c_str(), textColor);
    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {x, y, textSurface->w, textSurface->h};
    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

// Vẽ thời gian đếm ngược ở góc màn hình
void renderTime(Uint32 startTime) {
    if (!font) {
        font = TTF_OpenFont("arial.ttf", 24);
        if (!font) {
            cout << "Loi tai font Arial: " << TTF_GetError() << endl;
            return;
        }
    }

    if (!gameStarted) return;

    Uint32 elapsedTime = (SDL_GetTicks() - startTime) / 1000;
    int countdown = MAX_TIME - elapsedTime;
    if (countdown <= 0) countdown = 0;

    string timeText = "Time: " + to_string(countdown) + "s";
    renderText(timeText, 10, 10);
}

// Xáo trộn thứ tự các cặp bài
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

// Vẽ giao diện bàn chơi
void renderGame() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int index = i * COLS + j;
            SDL_Rect rect = { j * CARD_SIZE, i * CARD_SIZE, CARD_SIZE, CARD_SIZE };

            if (flipped[index] && board[index] >= 0 && images[board[index]]) {
                // Hiển thị mặt trước của thẻ
                SDL_RenderCopy(renderer, images[board[index]], nullptr, &rect);
            } else {
                // Hiển thị mặt sau (backImage) nếu có, nếu không thì vẽ hình chữ nhật xám
                if (backImage) {
                    SDL_RenderCopy(renderer, backImage, nullptr, &rect);
                } else {
                    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 200);
                    SDL_RenderFillRect(renderer, &rect);
                }
            }

            // Viền đen cho mỗi thẻ
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }

    renderTime(startTime);
    SDL_RenderPresent(renderer);
}


// Kiểm tra xem hai lá bài có khớp không
void checkMatch() {
    if (board[firstCard] != board[secondCard]) {
        flipped[firstCard] = false;
        flipped[secondCard] = false;
    } else {
        sound.playFlipCorrectSound(); // 🔊 Phát âm thanh khi ghép đúng
    }
    firstCard = secondCard = -1;
    waiting = false;
}

// Kiểm tra xem người chơi đã thắng chưa
bool checkWin() {
    for (bool flip : flipped) {
        if (!flip) return false;
    }
    return true;
}

// Đọc điểm cao nhất từ file
int readHighScore() {
    ifstream file("highscore.txt");
    int highScore = 9999;
    if (file >> highScore) {
        file.close();
    }
    return highScore;
}

// Ghi điểm cao mới nếu ít lượt lật hơn
void saveHighScore(int flips) {
    int highScore = readHighScore();
    if (flips < highScore) {
        ofstream file("highscore.txt");
        file << flips;
        file.close();
    }
}

// Vẽ màn hình chiến thắng
void renderWinScreen() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Rect rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, winnerImage, nullptr, &rect);

    saveHighScore(flipCount);
    int bestRecord = readHighScore();

    string flipText = "So lan lat: " + to_string(flipCount);
    string recordText = "Best Record: " + to_string(bestRecord) + " flips";

    int centerX = SCREEN_WIDTH / 2;
    int centerY = SCREEN_HEIGHT / 2;

    renderText(flipText, centerX - 100, centerY - 50);
    renderText(recordText, centerX - 120, centerY + 10);
    renderText("Nhan R de choi lai", centerX - 100, centerY + 50);
    SDL_RenderPresent(renderer);
    SDL_Delay(3000);
}

// Xử lý khi người dùng nhấn chuột vào bàn chơi
void handleMouseClick(int x, int y) {
    if (justStarted) {
        justStarted = false;
        return;
    }
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
        flipCount++;
    }
}

// Vẽ màn hình thua cuộc khi hết giờ
void renderGameOverScreen() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    SDL_Texture* gameOverImage = loadTexture("gameover.jpg");
    SDL_Rect rect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_RenderCopy(renderer, gameOverImage, nullptr, &rect);
    SDL_RenderPresent(renderer);

    SDL_Delay(3000);
    SDL_DestroyTexture(gameOverImage);
}

// Đặt lại trạng thái để chơi lại game
void resetGame() {
    flipCount = 0;
    matchedCount = 0;
    waiting = false;
    gameStarted = true;
    justStarted = true;
    startTime = SDL_GetTicks();

    for (int i = 0; i < TOTAL_CARDS; ++i) {
        cardShown[i] = false;
        cardMatched[i] = false;
    }

    shuffleBoard();
    sound.playBackgroundMusic();
}

void togglePause() {
    if (!isPaused) {
        isPaused = true;
        pausedTime = SDL_GetTicks() - startTime;
    } else {
        startTime = SDL_GetTicks() - pausedTime;
        isPaused = false;
    }
}

void renderPauseScreen() {
    SDL_Texture* pauseTexture = IMG_LoadTexture(renderer, "pause.jpg");
    if (!pauseTexture) {
        std::cerr << "Failed to load pause image: " << IMG_GetError() << std::endl;
        return;
    }

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, pauseTexture, NULL, NULL); // Hiển thị ảnh toàn màn hình
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(pauseTexture);
}

// Hàm main của chương trình
int main(int argc, char* argv[]) {
    // Khởi tạo SDL và SDL_image
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 || !(IMG_Init(IMG_INIT_JPG) & IMG_INIT_JPG)) {
        cout << "Loi khoi tao SDL hoac SDL_image!" << endl;
        return -1;
    }

    // Khởi tạo hệ thống âm thanh
    if (!sound.init()) {
        cout << "Loi khoi tao am thanh!" << endl;
        return -1;
    }

    // Khởi tạo SDL_ttf
    if (TTF_Init() == -1) {
        cout << "SDL_ttf khong the khoi tao! Loi: " << TTF_GetError() << endl;
        return 1;
    }

    // Load font chữ
    TTF_Font* font = TTF_OpenFont("arial.ttf", 28);
    if (!font) {
        cout << "Khong the tai font! Loi: " << TTF_GetError() << endl;
        return 1;
    }

    sound.playBackgroundMusic();

    window = SDL_CreateWindow("Memory Card Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    startButtonImage = loadTexture("image7.png");
    winnerImage = loadTexture("winner.png");
    backImage = loadTexture("back.jpg");
    pauseTexture = loadTexture("pause.jpg");
    for (int i = 0; i < TOTAL_CARDS; i++) {
        string path = "image" + to_string(i + 1) + ".jpg";
        images.push_back(loadTexture(path));
    }

    shuffleBoard();
    bool running = true;
    SDL_Event e;

   while (running) {
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT) {
            running = false;
        }

        if (e.type == SDL_KEYDOWN) {
            if (e.key.keysym.sym == SDLK_r) {
                resetGame();
            }

            if (e.key.keysym.sym == SDLK_p) {
                 togglePause();
            }
        }

        // Nếu đang tạm dừng, bỏ qua các sự kiện khác
        if (isPaused) {
        renderPauseScreen();
        SDL_RenderPresent(renderer);

        // Đợi sự kiện mới (chặn vòng lặp để tránh nháy)
        SDL_Event pauseEvent;
        while (SDL_WaitEvent(&pauseEvent)) {
            if (pauseEvent.type == SDL_KEYDOWN && pauseEvent.key.keysym.sym == SDLK_p) {
               isPaused = false;
                startTime = SDL_GetTicks() - pausedTime; // Tiếp tục thời gian
                break;
            }
            if (pauseEvent.type == SDL_QUIT) {
                running = false;
                break;
            }
        }
    }


            if (!gameStarted && e.type == SDL_MOUSEBUTTONDOWN) {
                int x = e.button.x;
                int y = e.button.y;

                int btnX = (SCREEN_WIDTH - BUTTON_WIDTH) / 2;
                int btnY = (SCREEN_HEIGHT - BUTTON_HEIGHT) / 2 + 100;
                if (x >= btnX && x <= btnX + BUTTON_WIDTH && y >= btnY && y <= btnY + BUTTON_HEIGHT) {
                    gameStarted = true;
                    flipCount = 0;
                    startTime = SDL_GetTicks();
                    justStarted = true;
                    sound.playBackgroundMusic();
                    SDL_FlushEvent(SDL_MOUSEBUTTONDOWN);
                }
            }

            if (gameStarted && e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
                handleMouseClick(e.button.x, e.button.y);
            }
        }

        if (!gameStarted) {
            renderStartScreen();
        } else {
            Uint32 elapsedTime = (SDL_GetTicks() - startTime) / 1000;
            int countdown = MAX_TIME - elapsedTime;

            if (countdown <= 0) {
                    sound.playLoseSound();
                renderGameOverScreen();
                SDL_Delay(2000);
                gameStarted = false;
                flipCount = 0;
                fill(flipped.begin(), flipped.end(), false);
            }

            if (waiting && SDL_GetTicks() - waitStart > 1000) {
            checkMatch();
}
            if (checkWin()) {
                sound.stopBackgroundMusic();
                sound.playWinSound();
                renderWinScreen();
                saveHighScore(flipCount);
                SDL_Delay(2000);
                gameStarted = false;
                flipCount = 0;
                fill(flipped.begin(), flipped.end(), false);
            } else {
                renderGame();
            }
        }

        SDL_Delay(16);
    }

    // Giải phóng tài nguyên
    sound.cleanUp();
    SDL_DestroyTexture(startButtonImage);
    SDL_DestroyTexture(winnerImage);
    for (SDL_Texture* tex : images) {
        SDL_DestroyTexture(tex);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_DestroyTexture(pauseTexture);
    IMG_Quit();
    SDL_Quit();
    return 0;
}
