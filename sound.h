#ifndef SOUND_H
#define SOUND_H

#include <SDL_mixer.h>
#include <iostream>

class SoundManager {
private:
    Mix_Music* backgroundMusic = nullptr;  // Nhạc nền khi vào game
    Mix_Chunk* flipCorrectSound = nullptr; // Âm thanh khi lật bài đúng
    Mix_Chunk* loseSound = nullptr;        // Âm thanh khi thua cuộc
    Mix_Chunk* startSound = nullptr;       // Âm thanh khi ấn nút Start
    Mix_Chunk* winSound = nullptr;         // Âm thanh khi chiến thắng

public:
    // Khởi tạo âm thanh
    bool init() {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            std::cout << "⚠ Lỗi khởi tạo SDL_mixer: " << Mix_GetError() << std::endl;
            return false;
        }

        // Tải nhạc nền và hiệu ứng âm thanh
        backgroundMusic = Mix_LoadMUS("sound sdl/sound1.mp3");
        flipCorrectSound = Mix_LoadWAV("sound sdl/flip_correct.wav");
        loseSound = Mix_LoadWAV("sound sdl/lose.wav");
        startSound = Mix_LoadWAV("sound sdl/sound1.mp3");
        winSound = Mix_LoadWAV("sound sdl/sound2.wav");

        if (!backgroundMusic || !flipCorrectSound || !loseSound || !startSound || !winSound) {
            std::cout << "⚠ Lỗi tải âm thanh: " << Mix_GetError() << std::endl;
            return false;
        }
        return true;
    }

    // Phát nhạc nền
    void playBackgroundMusic() {
        if (backgroundMusic) Mix_PlayMusic(backgroundMusic, -1);
    }

    // Phát âm thanh khi lật bài đúng
    void playFlipCorrectSound() {
        if (flipCorrectSound) Mix_PlayChannel(-1, flipCorrectSound, 0);
    }

    // Phát âm thanh khi thua cuộc
    void playLoseSound() {
        if (loseSound) Mix_PlayChannel(-1, loseSound, 0);
    }

    // Phát âm thanh khi ấn nút Start
    void playStartSound() {
        if (startSound) Mix_PlayChannel(-1, startSound, 0);
    }

    // Phát âm thanh khi chiến thắng
    void playWinSound() {
        if (winSound) Mix_PlayChannel(-1, winSound, 0);
    }

    // Dừng nhạc nền
    void stopBackgroundMusic() {
        Mix_HaltMusic();
    }

    // Giải phóng tài nguyên âm thanh
    void cleanUp() {
        if (backgroundMusic) Mix_FreeMusic(backgroundMusic);
        if (flipCorrectSound) Mix_FreeChunk(flipCorrectSound);
        if (loseSound) Mix_FreeChunk(loseSound);
        if (startSound) Mix_FreeChunk(startSound);
        if (winSound) Mix_FreeChunk(winSound);
        Mix_CloseAudio();
    }
};

#endif
