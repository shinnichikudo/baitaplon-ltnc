#ifndef SOUND_H
#define SOUND_H

#include <SDL_mixer.h>
#include <iostream>

class SoundManager {
private:
    Mix_Music* backgroundMusic = nullptr;  // Nh?c n?n khi v�o game
    Mix_Chunk* startSound = nullptr;       // �m thanh khi ?n n�t Start
    Mix_Chunk* winSound = nullptr;         // �m thanh khi chi?n th?ng

public:
    // Kh?i t?o �m thanh
    bool init() {
        if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
            std::cout << "? L?i kh?i t?o SDL_mixer: " << Mix_GetError() << std::endl;
            return false;
        }

        // T?i t?ng �m thanh ri�ng bi?t
        backgroundMusic = Mix_LoadMUS("D:/sound sdl/sound1.mp3");
        startSound = Mix_LoadWAV("D:/sound sdl/sound2.wav");
        winSound = Mix_LoadWAV("D:/sound sdl/sound2.mp3");

        if (!backgroundMusic || !startSound || !winSound) {
            std::cout << "? L?i t?i �m thanh: " << Mix_GetError() << std::endl;
            return false;
        }
        return true;
    }

    // Ph�t nh?c n?n
    void playBackgroundMusic() {
        if (backgroundMusic) Mix_PlayMusic(backgroundMusic, -1);
    }

    // Ph�t �m thanh khi ?n n�t Start
    void playStartSound() {
        if (startSound) Mix_PlayChannel(-1, startSound, 0);
    }

    // Ph�t �m thanh khi chi?n th?ng
    void playWinSound() {
        if (winSound) Mix_PlayChannel(-1, winSound, 0);
    }

    // D?ng nh?c n?n
    void stopBackgroundMusic() {
        Mix_HaltMusic();
    }

    // Gi?i ph�ng t�i nguy�n �m thanh
    void cleanUp() {
        if (backgroundMusic) Mix_FreeMusic(backgroundMusic);
        if (startSound) Mix_FreeChunk(startSound);
        if (winSound) Mix_FreeChunk(winSound);
        Mix_CloseAudio();
    }
};

#endif

