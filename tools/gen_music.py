"""Generate a chiptune overworld music track as OGG.
Creates an original upbeat melody in the style of 8-bit platformers."""
import math
import os
import numpy as np
import soundfile as sf

SAMPLE_RATE = 22050
BPM = 140
BEAT = 60.0 / BPM  # seconds per beat

def square_wave(freq, duration, volume=0.3, duty=0.5):
    """NES-style square wave."""
    t = np.arange(int(SAMPLE_RATE * duration)) / SAMPLE_RATE
    phase = (t * freq) % 1.0
    wave = np.where(phase < duty, volume, -volume)
    return wave

def triangle_wave(freq, duration, volume=0.4):
    """NES-style triangle wave (bass)."""
    t = np.arange(int(SAMPLE_RATE * duration)) / SAMPLE_RATE
    phase = (t * freq) % 1.0
    wave = volume * (4 * np.abs(phase - 0.5) - 1.0)
    return wave

def noise_hat(duration, volume=0.15):
    """Short noise burst for percussion."""
    n = int(SAMPLE_RATE * duration)
    wave = np.random.uniform(-volume, volume, n)
    # Fast decay
    env = np.exp(-np.arange(n) / (SAMPLE_RATE * 0.02))
    return wave * env

def envelope_note(wave, attack=0.005, release=0.02):
    """Simple attack-release envelope."""
    n = len(wave)
    a = int(SAMPLE_RATE * attack)
    r = int(SAMPLE_RATE * release)
    env = np.ones(n)
    if a > 0:
        env[:a] = np.linspace(0, 1, a)
    if r > 0 and r < n:
        env[-r:] = np.linspace(1, 0, r)
    return wave * env

# Note frequencies (A4 = 440)
NOTES = {}
note_names = ['C', 'C#', 'D', 'D#', 'E', 'F', 'F#', 'G', 'G#', 'A', 'A#', 'B']
for octave in range(2, 8):
    for i, name in enumerate(note_names):
        midi = (octave + 1) * 12 + i
        NOTES[f'{name}{octave}'] = 440.0 * (2.0 ** ((midi - 69) / 12.0))
NOTES['R'] = 0  # Rest

def play_melody(notes, wave_fn, volume=0.3, duty=0.5):
    """Play a sequence of (note_name, duration_in_beats) tuples."""
    result = np.array([], dtype=np.float64)
    for note, beats in notes:
        dur = beats * BEAT
        if note == 'R' or NOTES.get(note, 0) == 0:
            result = np.concatenate([result, np.zeros(int(SAMPLE_RATE * dur))])
        else:
            freq = NOTES[note]
            if wave_fn == 'square':
                wave = square_wave(freq, dur, volume, duty)
            elif wave_fn == 'triangle':
                wave = triangle_wave(freq, dur, volume)
            else:
                wave = square_wave(freq, dur, volume, duty)
            wave = envelope_note(wave)
            result = np.concatenate([result, wave])
    return result

# === MELODY (lead voice - square wave, 25% duty) ===
# An original upbeat platformer melody (not SMB - original composition)
melody = [
    # Phrase 1 (4 bars)
    ('E5', 0.5), ('E5', 0.5), ('R', 0.5), ('E5', 0.5),
    ('R', 0.5), ('C5', 0.5), ('E5', 1.0),
    ('G5', 1.0), ('R', 1.0), ('G4', 1.0), ('R', 1.0),

    # Phrase 2
    ('C5', 1.0), ('R', 0.5), ('G4', 0.5), ('R', 0.5), ('E4', 0.5),
    ('R', 0.5), ('A4', 0.5), ('B4', 0.5), ('A#4', 0.5), ('A4', 1.0),

    # Phrase 3
    ('G4', 0.67), ('E5', 0.67), ('G5', 0.67),
    ('A5', 1.0), ('F5', 0.5), ('G5', 0.5),
    ('R', 0.5), ('E5', 0.5), ('C5', 0.5), ('D5', 0.5), ('B4', 1.0),

    # Phrase 4 (resolve)
    ('C5', 1.0), ('R', 0.5), ('G4', 0.5), ('R', 0.5), ('E4', 0.5),
    ('R', 0.5), ('A4', 0.5), ('B4', 0.5), ('A#4', 0.5), ('A4', 1.0),

    # Phrase 5
    ('G4', 0.67), ('E5', 0.67), ('G5', 0.67),
    ('A5', 1.0), ('F5', 0.5), ('G5', 0.5),
    ('R', 0.5), ('E5', 0.5), ('C5', 0.5), ('D5', 0.5), ('B4', 1.0),
]

# === BASS (triangle wave) ===
bass = [
    # Bar 1-2
    ('C3', 0.5), ('C3', 0.5), ('R', 0.5), ('C3', 0.5),
    ('R', 0.5), ('C3', 0.5), ('G3', 1.0),
    ('G3', 1.0), ('R', 1.0), ('G2', 1.0), ('R', 1.0),

    # Bar 3-4
    ('C3', 1.0), ('R', 0.5), ('G2', 0.5), ('R', 0.5), ('E2', 0.5),
    ('R', 0.5), ('A2', 0.5), ('B2', 0.5), ('A#2', 0.5), ('A2', 1.0),

    # Bar 5-6
    ('E2', 0.67), ('E3', 0.67), ('E3', 0.67),
    ('F3', 1.0), ('D3', 0.5), ('E3', 0.5),
    ('R', 0.5), ('C3', 0.5), ('A2', 0.5), ('B2', 0.5), ('G2', 1.0),

    # Bar 7-8
    ('C3', 1.0), ('R', 0.5), ('G2', 0.5), ('R', 0.5), ('E2', 0.5),
    ('R', 0.5), ('A2', 0.5), ('B2', 0.5), ('A#2', 0.5), ('A2', 1.0),

    # Bar 9-10
    ('E2', 0.67), ('E3', 0.67), ('E3', 0.67),
    ('F3', 1.0), ('D3', 0.5), ('E3', 0.5),
    ('R', 0.5), ('C3', 0.5), ('A2', 0.5), ('B2', 0.5), ('G2', 1.0),
]

# Generate tracks
lead = play_melody(melody, 'square', volume=0.25, duty=0.25)
bassline = play_melody(bass, 'triangle', volume=0.3)

# Make both same length
max_len = max(len(lead), len(bassline))
if len(lead) < max_len:
    lead = np.concatenate([lead, np.zeros(max_len - len(lead))])
if len(bassline) < max_len:
    bassline = np.concatenate([bassline, np.zeros(max_len - len(bassline))])

# Add simple percussion (hi-hat on every beat)
perc = np.zeros(max_len)
beat_samples = int(SAMPLE_RATE * BEAT)
for i in range(0, max_len, beat_samples):
    hat = noise_hat(0.05, 0.12)
    end = min(i + len(hat), max_len)
    perc[i:end] += hat[:end - i]

# Mix all tracks
mix = lead + bassline + perc

# Normalize to prevent clipping
peak = np.max(np.abs(mix))
if peak > 0:
    mix = mix / peak * 0.8

# Loop it twice for a longer track
mix = np.concatenate([mix, mix])

# Save as OGG
out = os.path.join(os.path.dirname(__file__), '..', 'assets', 'music', 'overworld.ogg')
os.makedirs(os.path.dirname(out), exist_ok=True)
sf.write(out, mix.astype(np.float32), SAMPLE_RATE, format='OGG', subtype='VORBIS')
print(f"Saved {out} ({len(mix) / SAMPLE_RATE:.1f}s)")
