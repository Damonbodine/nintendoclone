#include "core/Audio.h"
#include <cstdio>

Audio::Audio() {}

Audio::~Audio() {
    shutdown();
}

bool Audio::init() {
    if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
        std::fprintf(stderr, "SDL_mixer init failed: %s\n", Mix_GetError());
        return false;
    }

    // Allocate 8 mixing channels for sound effects
    Mix_AllocateChannels(8);
    m_initialized = true;
    return true;
}

void Audio::shutdown() {
    if (!m_initialized) return;

    for (auto& [name, chunk] : m_sounds) {
        Mix_FreeChunk(chunk);
    }
    m_sounds.clear();

    for (auto& [name, music] : m_music) {
        Mix_FreeMusic(music);
    }
    m_music.clear();

    Mix_CloseAudio();
    m_initialized = false;
}

void Audio::loadSound(const std::string& name, const std::string& path) {
    Mix_Chunk* chunk = Mix_LoadWAV(path.c_str());
    if (!chunk) {
        std::fprintf(stderr, "Failed to load sound '%s': %s\n", path.c_str(), Mix_GetError());
        return;
    }
    m_sounds[name] = chunk;
}

void Audio::playSound(const std::string& name) {
    auto it = m_sounds.find(name);
    if (it != m_sounds.end()) {
        Mix_PlayChannel(-1, it->second, 0);
    }
}

void Audio::loadMusic(const std::string& name, const std::string& path) {
    Mix_Music* music = Mix_LoadMUS(path.c_str());
    if (!music) {
        std::fprintf(stderr, "Failed to load music '%s': %s\n", path.c_str(), Mix_GetError());
        return;
    }
    m_music[name] = music;
}

void Audio::playMusic(const std::string& name, bool loop) {
    auto it = m_music.find(name);
    if (it != m_music.end()) {
        Mix_PlayMusic(it->second, loop ? -1 : 1);
    }
}

void Audio::stopMusic() {
    Mix_HaltMusic();
}

void Audio::pauseMusic() {
    Mix_PauseMusic();
}

void Audio::resumeMusic() {
    Mix_ResumeMusic();
}

bool Audio::isMusicPlaying() const {
    return Mix_PlayingMusic() != 0;
}

void Audio::setSoundVolume(int volume) {
    Mix_Volume(-1, volume);
}

void Audio::setMusicVolume(int volume) {
    Mix_VolumeMusic(volume);
}
