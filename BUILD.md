# Building Super Mario Bros Clone

## Prerequisites

### Ubuntu/Debian
```bash
sudo apt-get install build-essential cmake libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev
```

### macOS (Homebrew)
```bash
brew install cmake sdl2 sdl2_image sdl2_mixer
```

### Windows (vcpkg)
```bash
vcpkg install sdl2 sdl2-image sdl2-mixer
```

## Building

```bash
mkdir build && cd build
cmake ..
make -j$(nproc)
```

## Running

```bash
./build/SuperMarioBrosClone
```

The game expects an `assets/` directory alongside the executable containing sprite sheets,
tilesets, sounds, and music files. See `PLAN.md` for the full asset list.

## Controls

| Key | Action |
|-----|--------|
| Arrow keys / WASD | Move |
| Z / Space | Jump (A button) |
| X / Left Shift | Run / Fireball (B button) |
| Enter | Start / Pause |
| Escape | Quit |
