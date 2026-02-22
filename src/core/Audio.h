#pragma once

#include <SDL2/SDL_mixer.h>
#include <string>
#include <unordered_map>

class Audio {
public:
    Audio();
    ~Audio();

    bool init();
    void shutdown();

    // Sound effects
    void loadSound(const std::string& name, const std::string& path);
    void playSound(const std::string& name);

    // Music
    void loadMusic(const std::string& name, const std::string& path);
    void playMusic(const std::string& name, bool loop = true);
    void stopMusic();
    void pauseMusic();
    void resumeMusic();
    bool isMusicPlaying() const;

    // Volume control (0-128)
    void setSoundVolume(int volume);
    void setMusicVolume(int volume);

private:
    std::unordered_map<std::string, Mix_Chunk*> m_sounds;
    std::unordered_map<std::string, Mix_Music*> m_music;
    bool m_initialized = false;
};
