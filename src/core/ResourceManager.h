#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <unordered_map>

class ResourceManager {
public:
    ResourceManager();
    ~ResourceManager();

    bool init(SDL_Renderer* renderer);
    void shutdown();

    // Load a texture from a PNG file. Returns nullptr on failure.
    // Textures are cached — loading the same path twice returns the same texture.
    SDL_Texture* loadTexture(const std::string& path);

    // Get a previously loaded texture
    SDL_Texture* getTexture(const std::string& path) const;

    // Free a specific texture
    void freeTexture(const std::string& path);

    // Free all textures
    void freeAll();

private:
    SDL_Renderer* m_renderer = nullptr;
    std::unordered_map<std::string, SDL_Texture*> m_textures;
};
