#include <SDL.h>
#include <SDL_image.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <ctime>

using namespace std;
SDL_Texture* startScreen = nullptr;  // Ảnh màn hình chờ
bool gameStarted = false;  // Kiểm tra trạng thái game
SDL_Texture* startButtonImage = nullptr;

const int SCREEN_WIDTH = 600;
const int SCREEN_HEIGHT = 600;
const int ROWS = 4, COLS = 4;
const int CARD_SIZE = 150;  // Kích thước của mỗi card
const int TOTAL_CARDS = ROWS * COLS / 2;  // Số cặp card

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;
vector<SDL_Texture*> images;         // Danh sách texture của các ảnh
vector<int> board(ROWS * COLS, -1);    // Mảng lưu ID ảnh cho mỗi ô
vector<bool> flipped(ROWS * COLS, false);  // Trạng thái của các card (đã mở hay chưa)

int firstCard = -1, secondCard = -1;  // Hai ô được chọn
bool waiting = false;
Uint32 waitStart = 0;

// Khai báo trước để tránh lỗi chưa được khai báo
SDL_Texture* loadTexture(const std::string& path);

void renderStartScreen() {
    // Tải hình nền
    SDL_Texture* background = loadTexture("D:\\ảnh sdl\\background.jpg");
    SDL_Texture* startButtonImage = loadTexture("D:\\ảnh sdl\\image7.png");

    if (!background || !startButtonImage) {
        cout << "Lỗi: Không thể tải hình nền hoặc nút Start!" << endl;
        return;
    }

    SDL_Rect bgRect = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
    SDL_Rect startButton = {(SCREEN_WIDTH - 200) / 2, (SCREEN_HEIGHT - 100) / 2, 200, 100};

    SDL_RenderCopy(renderer, background, nullptr, &bgRect);
    SDL_RenderCopy(renderer, startButtonImage, nullptr, &startButton);
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(background);
    SDL_DestroyTexture(startButtonImage);
}

// Hàm tải ảnh từ file sử dụng SDL_image
SDL_Texture* loadTexture(const string& path) {
    SDL_Texture* texture = IMG_LoadTexture(renderer, path.c_str());
    if (!texture) {
        cout << "Lỗi tải ảnh từ " << path << " : " << IMG_GetError() << endl;
    }
    return texture;
}

// Hàm trộn các cặp ảnh vào bảng chơi
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

// Hàm vẽ giao diện game, bao gồm lưới với đường viền cho mỗi ô
void renderGame() {
    // Xóa màn hình với màu trắng
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);

    // Vẽ các ô của lưới
    for (int i = 0; i < ROWS; i++) {
        for (int j = 0; j < COLS; j++) {
            int index = i * COLS + j;
            SDL_Rect rect = { j * CARD_SIZE, i * CARD_SIZE, CARD_SIZE, CARD_SIZE };

            // Nếu card đã được lật, hiển thị ảnh
            if (flipped[index]) {
                SDL_RenderCopy(renderer, images[board[index]], nullptr, &rect);
            } else {
                // Nếu chưa lật, vẽ ô màu xanh
                SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);
                SDL_RenderFillRect(renderer, &rect);
            }

            // Vẽ đường viền cho ô (màu đen)
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderDrawRect(renderer, &rect);
        }
    }
    SDL_RenderPresent(renderer);
}

// Hàm xử lý khi người chơi click chuột
void handleMouseClick(int x, int y) {
   if (waiting) return;

    int col = x / CARD_SIZE;
    int row = y / CARD_SIZE;
    int index = row * COLS + col;

    // Nếu card đã được lật thì không xử lý
    if (flipped[index]) return;

    // Lật card được chọn
    flipped[index] = true;
    if (firstCard == -1) {
        firstCard = index;
    } else {
        secondCard = index;
        waiting = true;
        waitStart = SDL_GetTicks();
    }
}

// Hàm kiểm tra xem hai card đã chọn có khớp hay không
void checkMatch() {

}

int main(int argc, char* argv[]) {
    // Khởi tạo SDL và SDL_image
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG);

    window = SDL_CreateWindow("Memory Card Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Tải ảnh màn hình chờ
    startButtonImage = loadTexture("D:\\ảnh sdl\\image7.png");
    if (!startButtonImage) {
        cout << "Lỗi: Không thể tải ảnh nút Start!" << endl;
        return -1;
    }
}
