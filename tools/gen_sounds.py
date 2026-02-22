"""Generate NES-style chiptune sound effects as WAV files.
Uses square waves, noise, and frequency sweeps to mimic the NES APU."""
import struct
import math
import os
import random

SAMPLE_RATE = 22050

def make_wav(path, samples, sample_rate=SAMPLE_RATE):
    """Write 8-bit mono WAV from float samples (-1.0 to 1.0)."""
    os.makedirs(os.path.dirname(path), exist_ok=True)
    # Convert to 8-bit unsigned
    data = bytearray()
    for s in samples:
        val = int((s * 0.5 + 0.5) * 255)
        val = max(0, min(255, val))
        data.append(val)

    data_size = len(data)
    file_size = 36 + data_size

    with open(path, 'wb') as f:
        f.write(b'RIFF')
        f.write(struct.pack('<I', file_size))
        f.write(b'WAVE')
        f.write(b'fmt ')
        f.write(struct.pack('<I', 16))
        f.write(struct.pack('<H', 1))           # PCM
        f.write(struct.pack('<H', 1))           # mono
        f.write(struct.pack('<I', sample_rate))
        f.write(struct.pack('<I', sample_rate))  # byte rate
        f.write(struct.pack('<H', 1))           # block align
        f.write(struct.pack('<H', 8))           # bits per sample
        f.write(b'data')
        f.write(struct.pack('<I', data_size))
        f.write(data)

def square_wave(freq, duration, volume=0.5, duty=0.5, sample_rate=SAMPLE_RATE):
    """Generate a square wave (NES pulse channel)."""
    samples = []
    num_samples = int(sample_rate * duration)
    for i in range(num_samples):
        t = i / sample_rate
        phase = (t * freq) % 1.0
        val = volume if phase < duty else -volume
        samples.append(val)
    return samples

def noise(duration, volume=0.3, sample_rate=SAMPLE_RATE):
    """Generate pseudo-random noise (NES noise channel)."""
    samples = []
    num_samples = int(sample_rate * duration)
    # NES-style LFSR noise
    lfsr = 1
    for _ in range(num_samples):
        bit = ((lfsr >> 0) ^ (lfsr >> 1)) & 1
        lfsr = (lfsr >> 1) | (bit << 14)
        val = volume if (lfsr & 1) else -volume
        samples.append(val)
    return samples

def freq_sweep(start_freq, end_freq, duration, volume=0.5, duty=0.5, sample_rate=SAMPLE_RATE):
    """Square wave with frequency sweep."""
    samples = []
    num_samples = int(sample_rate * duration)
    phase = 0.0
    for i in range(num_samples):
        t = i / num_samples
        freq = start_freq + (end_freq - start_freq) * t
        phase += freq / sample_rate
        val = volume if (phase % 1.0) < duty else -volume
        samples.append(val)
    return samples

def envelope(samples, attack=0.01, decay=0.0, sustain=1.0, release=0.05):
    """Apply ADSR envelope to samples."""
    total = len(samples)
    sr = SAMPLE_RATE
    attack_samples = int(attack * sr)
    release_samples = int(release * sr)
    result = []
    for i, s in enumerate(samples):
        if i < attack_samples:
            env = i / max(attack_samples, 1)
        elif i >= total - release_samples:
            env = (total - i) / max(release_samples, 1)
        else:
            env = sustain
        result.append(s * env)
    return result

def mix(*sample_lists):
    """Mix multiple sample lists together."""
    max_len = max(len(s) for s in sample_lists)
    result = [0.0] * max_len
    for samples in sample_lists:
        for i, s in enumerate(samples):
            result[i] += s
    # Normalize
    peak = max(abs(s) for s in result) if result else 1.0
    if peak > 1.0:
        result = [s / peak for s in result]
    return result

def concat(*sample_lists):
    """Concatenate sample lists."""
    result = []
    for samples in sample_lists:
        result.extend(samples)
    return result

base = os.path.join(os.path.dirname(__file__), '..', 'assets', 'sounds')

# === JUMP ===
# Rising frequency sweep, short and snappy
jump = freq_sweep(350, 900, 0.15, volume=0.6, duty=0.25)
jump = envelope(jump, attack=0.005, release=0.02)
make_wav(os.path.join(base, 'jump.wav'), jump)
print("Generated jump.wav")

# === STOMP ===
# Quick descending thump
stomp = concat(
    freq_sweep(600, 100, 0.08, volume=0.6, duty=0.5),
    noise(0.03, volume=0.2)
)
stomp = envelope(stomp, attack=0.002, release=0.02)
make_wav(os.path.join(base, 'stomp.wav'), stomp)
print("Generated stomp.wav")

# === COIN ===
# Two quick high-pitched notes (classic ding-ding)
coin_note1 = square_wave(988, 0.05, volume=0.5, duty=0.25)  # B5
coin_note2 = square_wave(1319, 0.15, volume=0.5, duty=0.25)  # E6
coin = concat(coin_note1, coin_note2)
coin = envelope(coin, attack=0.002, release=0.05)
make_wav(os.path.join(base, 'coin.wav'), coin)
print("Generated coin.wav")

# === BUMP ===
# Low thud when hitting a block
bump = freq_sweep(200, 80, 0.1, volume=0.6, duty=0.5)
bump = envelope(bump, attack=0.002, release=0.03)
make_wav(os.path.join(base, 'bump.wav'), bump)
print("Generated bump.wav")

# === POWERUP ===
# Rising arpeggio (ascending notes)
powerup = concat(
    square_wave(523, 0.06, volume=0.5, duty=0.25),  # C5
    square_wave(659, 0.06, volume=0.5, duty=0.25),  # E5
    square_wave(784, 0.06, volume=0.5, duty=0.25),  # G5
    square_wave(1047, 0.06, volume=0.5, duty=0.25), # C6
    square_wave(1319, 0.06, volume=0.5, duty=0.25), # E6
    square_wave(1568, 0.06, volume=0.5, duty=0.25), # G6
    square_wave(2093, 0.15, volume=0.5, duty=0.25), # C7
)
powerup = envelope(powerup, attack=0.002, release=0.05)
make_wav(os.path.join(base, 'powerup_collect.wav'), powerup)
print("Generated powerup_collect.wav")

# === DEATH ===
# Descending slide then silence
death = concat(
    freq_sweep(800, 200, 0.3, volume=0.5, duty=0.5),
    [0.0] * int(SAMPLE_RATE * 0.1),  # pause
    freq_sweep(400, 80, 0.5, volume=0.4, duty=0.5),
)
death = envelope(death, attack=0.005, release=0.1)
make_wav(os.path.join(base, 'death.wav'), death)
print("Generated death.wav")
