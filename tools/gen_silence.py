"""Generate silent placeholder WAV files for all expected audio assets."""
import struct
import os

def make_silent_wav(path, duration_ms=100, sample_rate=22050):
    """Create a minimal silent WAV file."""
    num_samples = int(sample_rate * duration_ms / 1000)
    data_size = num_samples  # 8-bit mono = 1 byte per sample
    file_size = 36 + data_size

    os.makedirs(os.path.dirname(path), exist_ok=True)
    with open(path, 'wb') as f:
        # RIFF header
        f.write(b'RIFF')
        f.write(struct.pack('<I', file_size))
        f.write(b'WAVE')
        # fmt chunk
        f.write(b'fmt ')
        f.write(struct.pack('<I', 16))       # chunk size
        f.write(struct.pack('<H', 1))        # PCM format
        f.write(struct.pack('<H', 1))        # mono
        f.write(struct.pack('<I', sample_rate))
        f.write(struct.pack('<I', sample_rate))  # byte rate
        f.write(struct.pack('<H', 1))        # block align
        f.write(struct.pack('<H', 8))        # bits per sample
        # data chunk
        f.write(b'data')
        f.write(struct.pack('<I', data_size))
        f.write(b'\x80' * num_samples)  # silence for 8-bit audio is 128

base = os.path.join(os.path.dirname(__file__), '..')

sounds = [
    'assets/sounds/jump.wav',
    'assets/sounds/stomp.wav',
    'assets/sounds/coin.wav',
    'assets/sounds/bump.wav',
    'assets/sounds/powerup_collect.wav',
    'assets/sounds/death.wav',
]

for s in sounds:
    path = os.path.join(base, s)
    make_silent_wav(path)
    print(f"Created {s}")

# OGG placeholder is trickier — create a tiny valid OGG
# SDL_mixer will just fail gracefully on missing music, so skip for now
print("Note: assets/music/overworld.ogg not generated (SDL_mixer handles missing music gracefully)")
