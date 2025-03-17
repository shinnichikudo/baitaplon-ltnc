# Memory Card Game (SDL2)  
# 📖 Giới thiệu
Memory Card Game là một trò chơi lật hình được xây dựng bằng C++ và SDL2, nơi người chơi cần tìm ra tất cả các cặp hình giống nhau trong khoảng thời gian giới hạn. Trò chơi bao gồm hiệu ứng hình ảnh, âm thanh, hệ thống điểm số cao và một giao diện trực quan giúp người chơi có trải nghiệm tốt nhất.  

# 🛠️ Cài đặt
## 1. Yêu cầu hệ thống  
Hệ điều hành: Windows / Linux / macOS  
Thư viện cần thiết:  
SDL2  
SDL2_image  
SDL2_ttf  
SDL2_mixer  
## 2. Cài đặt SDL2 và các thư viện liên quan  
Trên Windows:
Tải SDL2, SDL2_image, SDL2_ttf, SDL2_mixer từ trang chủ: https://github.com/libsdl-org/SDL
Giải nén và đặt thư viện vào thư mục dự án.
Cấu hình trình biên dịch để liên kết với các thư viện SDL2.
Trên Linux (Ubuntu):
sh
sudo apt update
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-ttf-dev libsdl2-mixer-dev
Trên macOS:
sh
Sao chép
Chỉnh sửa
brew install sdl2 sdl2_image sdl2_ttf sdl2_mixer
## 3. Biên dịch và chạy game
sh
Sao chép
Chỉnh sửa
g++ -o memory_game main.cpp -lSDL2 -lSDL2_image -lSDL2_ttf -lSDL2_mixer
./memory_game
#🚀 Bắt đầu chơi
Chạy game bằng cách mở ứng dụng hoặc sử dụng dòng lệnh như trên.

Màn hình bắt đầu sẽ hiển thị với nút "Start".
Nhấn vào "Start" để bắt đầu trò chơi.


# 🎮 Cách chơi
## 🎯 Mục tiêu
Lật các thẻ bài để tìm ra các cặp hình giống nhau.
Hoàn thành tất cả các cặp trong vòng 60 giây để chiến thắng.

## 🖱️ Điều khiển
Click chuột trái để lật một thẻ.
Nhấn phím R để chơi lại khi kết thúc trò chơi.
## 🔄 Cách hoạt động
Click vào một ô để lật hình đầu tiên.
Click vào một ô khác để lật hình thứ hai.
Nếu hai hình giống nhau, chúng sẽ giữ nguyên.
Nếu khác nhau, chúng sẽ úp lại sau 1 giây.
Khi tất cả cặp hình được ghép xong, bạn chiến thắng.
## 🏆 Chiến thắng và Thất bại
### 🥳 Chiến thắng
Ghép đúng tất cả cặp hình trước khi hết giờ.
Hiển thị màn hình chúc mừng và số lần lật bài.
Nếu đạt số lần lật thấp nhất, điểm sẽ được lưu vào highscore.txt.

### 😢 Thất bại
Hết 60 giây mà chưa ghép xong tất cả cặp hình.
Hiển thị màn hình "Game Over" và tùy chọn chơi lại.

##🎨 Đồ họa game
Game sử dụng hình ảnh được tải bằng SDL2_image.
Màn hình nền, các thẻ bài, nút bấm đều được hiển thị bằng SDL_Texture.
Hiệu ứng chuyển đổi giữa các màn hình giúp tăng trải nghiệm mượt mà.
# 📝 Giới thiệu source code
Source code được tổ chức đơn giản, dễ hiểu, phù hợp với cả người mới học lập trình game bằng C++ và SDL2. Dưới đây là tổng quan các thành phần và hàm chính:
## 📁 Cấu trúc thư mục dự án

| File / Thư mục                | Mô tả                                                                 |
|------------------------------|----------------------------------------------------------------------|
| `main.cpp`                   | File mã nguồn chính, chứa toàn bộ logic và vòng lặp game.            |
| `sound.h`                    | Khai báo lớp `SoundManager` để quản lý âm thanh.                     |
| `sound.cpp`                  | Cài đặt chi tiết các hàm xử lý âm thanh (nhạc nền, âm thắng,...).   |
| `highscore.txt`             | File lưu kỷ lục số lần lật bài ít nhất.                              |
| `arial.ttf`                 | Font chữ dùng để hiển thị văn bản trong game.                        |
| `/ảnh sdl/`               | Thư mục chứa hình ảnh của game  |
| ├── `image1.jpg` → `image8.jpg` | Các hình ảnh thẻ bài (8 cặp ảnh).                                   |
| ├── `background.jpg`        | Hình nền màn hình start.                                             |
| ├── `image7.png`            | Hình ảnh nút Start.                                                  |
| ├── `winner.png`            | Hình ảnh khi chiến thắng.                                            |
| ├── `gameover.jpg`          | Hình ảnh khi thua (hết thời gian).                                   |
| `README.md`                 | Tài liệu mô tả game, cách chạy, điều khiển, v.v.                     |
| `Makefile` / `.vcxproj`     | (nếu có) Dùng để biên dịch trên Linux hoặc Visual Studio.           |

## 🎮 Chức năng các hàm trong game

---

### 🖼️ Nhóm hiển thị (Rendering)

| Hàm                       | Tham số                             | Chức năng                                                                 |
|--------------------------|--------------------------------------|--------------------------------------------------------------------------|
| `renderStartScreen()`    | –                                    | Hiển thị màn hình bắt đầu với nút "Start".                               |
| `renderGame()`           | –                                    | Vẽ toàn bộ lưới thẻ bài lên màn hình.                                    |
| `renderText()`           | `const string& text, int x, int y`   | Hiển thị văn bản tại vị trí x, y bằng SDL_ttf.                           |
| `renderTime()`           | `Uint32 startTime`                   | Hiển thị đồng hồ đếm ngược (giới hạn thời gian chơi).                    |
| `renderWinScreen()`      | –                                    | Hiển thị màn hình chiến thắng và số lần lật, best record.                |
| `renderGameOverScreen()` | –                                    | Hiển thị màn hình "Game Over" khi hết thời gian.                         |

---

### 🧠 Nhóm logic game

| Hàm                       | Tham số          | Chức năng                                                                 |
|--------------------------|------------------|--------------------------------------------------------------------------|
| `shuffleBoard()`         | –                | Trộn ngẫu nhiên các cặp thẻ bài (16 thẻ → 8 cặp).                        |
| `checkMatch()`           | –                | Kiểm tra xem hai thẻ vừa lật có trùng nhau không.                        |
| `checkWin()`             | –                | Kiểm tra xem người chơi đã lật đúng toàn bộ thẻ hay chưa.               |
| `resetGame()`            | –                | Đặt lại trạng thái game để chơi lại.                                     |
| `handleMouseClick()`     | `int x, int y`   | Xử lý khi người chơi click chuột vào vị trí (x, y).                      |

---

### 🔊 Nhóm âm thanh (`sound.h`)

| Hàm                            | Tham số      | Chức năng                                                        |
|-------------------------------|--------------|------------------------------------------------------------------|
| `sound.init()`                | –            | Khởi tạo hệ thống âm thanh.                                      |
| `sound.playBackgroundMusic()`| –            | Phát nhạc nền vòng lặp.                                          |
| `sound.stopBackgroundMusic()`| –            | Dừng nhạc nền.                                                   |
| `sound.playWinSound()`        | –            | Phát âm thanh khi chiến thắng.                                   |
| `sound.cleanUp()`             | –            | Giải phóng bộ nhớ âm thanh khi thoát game.                       |

---

### 💾 Nhóm lưu điểm cao (High Score)

| Hàm                  | Tham số         | Chức năng                                                            |
|---------------------|-----------------|----------------------------------------------------------------------|
| `readHighScore()`   | –               | Đọc số lần lật ít nhất từ file `highscore.txt`.                      |
| `saveHighScore()`   | `int flips`     | Lưu số lần lật nếu thấp hơn kỷ lục cũ.                               |

---

### 📦 Nhóm hỗ trợ

| Hàm              | Tham số              | Chức năng                                                       |
|------------------|----------------------|-----------------------------------------------------------------|
| `loadTexture()`  | `const string& path` | Tải hình ảnh từ file và trả về `SDL_Texture*`.                 |

