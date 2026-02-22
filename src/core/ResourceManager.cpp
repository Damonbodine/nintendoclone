#include "core/ResourceManager.h"
#include <cstdio>

ResourceManager::ResourceManager() {}

ResourceManager::~ResourceManager() {
    shutdown();
}

bool ResourceManager::init(SDL_Renderer* renderer) {
    m_renderer = renderer;

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
        std::fprintf(stderr, "SDL_image init failed: %s\n", IMG_GetError());
        return false;
    }

    return true;
}

void ResourceManager::shutdown() {
    freeAll();
    IMG_Quit();
}

SDL_Texture* ResourceManager::loadTexture(const std::string& path) {
    // Check cache first
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }

    SDL_Surface* surface = IMG_Load(path.c_str());
    if (!surface) {
        std::fprintf(stderr, "Failed to load image '%s': %s\n", path.c_str(), IMG_GetError());
        return nullptr;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(m_renderer, surface);
    SDL_FreeSurface(surface);

    if (!texture) {
        std::fprintf(stderr, "Failed to create texture from '%s': %s\n", path.c_str(), SDL_GetError());
        return nullptr;
    }

    // Enable alpha blending
    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);

    m_textures[path] = texture;
    return texture;
}

SDL_Texture* ResourceManager::getTexture(const std::string& path) const {
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        return it->second;
    }
    return nullptr;
}

void ResourceManager::freeTexture(const std::string& path) {
    auto it = m_textures.find(path);
    if (it != m_textures.end()) {
        SDL_DestroyTexture(it->second);
        m_textures.erase(it);
    }
}

void ResourceManager::freeAll() {
    for (auto& [path, texture] : m_textures) {
        SDL_DestroyTexture(texture);
    }
    m_textures.clear();
}
