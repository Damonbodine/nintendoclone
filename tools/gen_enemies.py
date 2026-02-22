"""Generate enemy sprite sheet (16x16, 12 frames).
Layout: Goomba(0-2), Koopa(3-9), Piranha(10-11)
"""
import sys, os
sys.path.insert(0, os.path.dirname(__file__))
from nes_palette import *

# === GOOMBA ===
# Colors: B=brown body, D=dark feet, S=skin/tan face, K=black eyes
GCM = {'B': GB, 'D': GD, 'S': GT, 'K': GK}

# Goomba walk frame 1 (feet apart)
goomba_walk1 = sprite_from_rows([
    '................',
    '......BBBB......',
    '.....BBBBBB.....',
    '....BBBBBBBB....',
    '...BSSBBBSSB....',
    '...SSSBBBSSS....',
    '..SSKBBBBKSS....',
    '..SKKBBBBKKS....',
    '..SSBBBBBBSS....',
    '...BBBBBBBBB....',
    '....BBBBBBBB....',
    '.....DDDDDD.....',
    '....DDDDDDDD....',
    '...DDD....DDD...',
    '..DDDD....DDDD..',
    '................',
], GCM)

# Goomba walk frame 2 (feet together-ish)
goomba_walk2 = sprite_from_rows([
    '................',
    '......BBBB......',
    '.....BBBBBB.....',
    '....BBBBBBBB....',
    '...BSSBBBSSB....',
    '...SSSBBBSSS....',
    '..SSKBBBBKSS....',
    '..SKKBBBBKKS....',
    '..SSBBBBBBSS....',
    '...BBBBBBBBB....',
    '....BBBBBBBB....',
    '.....DDDDDD.....',
    '....DDDDDDDD....',
    '....DDDDDDDD....',
    '...DDDD..DDDD...',
    '................',
], GCM)

# Goomba squished (flat)
goomba_squish = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
    '..BSSBBBSSB.....',
    '..SSSBBBSSS.....',
    '.SSKBBBBKSS.....',
    '.SKKBBBBKKS.....',
    '................',
], GCM)

# === KOOPA TROOPA ===
KCM = {'G': KG, 'D': KD, 'S': KS, 'W': KW, 'K': BK}

# Koopa walk frame 1
koopa_walk1 = sprite_from_rows([
    '......GGGG......',
    '.....GGGGGG.....',
    '....GGGGGGGG....',
    '....GDGGDGGG....',
    '...GGDGGDGGGG...',
    '...GGWWWWGGGG...',
    '...GGGGGGGGG....',
    '....GGGGGGGG....',
    '.....SSSS.......',
    '.....SSSSS......',
    '....SSKSSS......',
    '....SSKSS.......',
    '.....SSS........',
    '.....SS.........',
    '....DDDD........',
    '...DDDDD........',
], KCM)

# Koopa walk frame 2
koopa_walk2 = sprite_from_rows([
    '......GGGG......',
    '.....GGGGGG.....',
    '....GGGGGGGG....',
    '....GDGGDGGG....',
    '...GGDGGDGGGG...',
    '...GGWWWWGGGG...',
    '...GGGGGGGGG....',
    '....GGGGGGGG....',
    '.....SSSS.......',
    '.....SSSSS......',
    '....SSKSSS......',
    '....SSKSS.......',
    '.....SSS........',
    '......SS........',
    '......DDDD......',
    '.......DDDDD....',
], KCM)

# Koopa shell (static, no legs)
koopa_shell = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '................',
    '......GGGG......',
    '.....GGGGGG.....',
    '....GGGGGGGG....',
    '...GGGGGGGGGG...',
    '...GGWGGGWGGG...',
    '...GGWGGGWGGG...',
    '...GGGGGGGGGG...',
    '....GGGGGGGG....',
    '.....GGGGGG.....',
    '......GGGG......',
    '................',
], KCM)

# Koopa shell spin frames (4 rotation frames)
koopa_spin1 = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '................',
    '......GGGG......',
    '.....GGGGGG.....',
    '....GGGGGGGG....',
    '...GGGGGGGGGG...',
    '...GGWGGGWGGG...',
    '...GGWGGGWGGG...',
    '...GGGGGGGGGG...',
    '....GGGGGGGG....',
    '.....GGGGGG.....',
    '......GGGG......',
    '................',
], KCM)

koopa_spin2 = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '................',
    '......GGGG......',
    '.....GGGGGG.....',
    '....GWGGGGGG....',
    '...GGWGGGGGGG...',
    '...GGGGGGGWGG...',
    '...GGGGGGGWGG...',
    '...GGGGGGGGGG...',
    '....GGGGGGGG....',
    '.....GGGGGG.....',
    '......GGGG......',
    '................',
], KCM)

koopa_spin3 = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '................',
    '......GGGG......',
    '.....GGGGGG.....',
    '....GGGGGGGG....',
    '...GGGGGGGGGG...',
    '...GGGWGGGWGG...',
    '...GGGWGGGWGG...',
    '...GGGGGGGGGG...',
    '....GGGGGGGG....',
    '.....GGGGGG.....',
    '......GGGG......',
    '................',
], KCM)

koopa_spin4 = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '................',
    '......GGGG......',
    '.....GGGGGG.....',
    '....GGGGGGWG....',
    '...GGGGGGGWGGG..',
    '...GGWGGGGGGG...',
    '...GGWGGGGGGG...',
    '...GGGGGGGGGG...',
    '....GGGGGGGG....',
    '.....GGGGGG.....',
    '......GGGG......',
    '................',
], KCM)

# === PIRANHA PLANT ===
PCM = {'G': PG, 'D': PD, 'W': PW, 'R': PR}

# Piranha mouth closed
piranha_closed = sprite_from_rows([
    '................',
    '....GGRRRRGG....',
    '...GGRRRRRRRG...',
    '..GGRRRRRRRRGG..',
    '..GRRWRRRRWRRG..',
    '..GRRWRRRRWRRG..',
    '..GRRRRRRRRRRG..',
    '...GRRRRRRRRRG..',
    '....GGGGGGGG....',
    '.....GGDDGG.....',
    '.....GGDDGG.....',
    '.....GGDDGG.....',
    '.....GGDDGG.....',
    '....GGGGGGGG....',
    '................',
    '................',
], PCM)

# Piranha mouth open
piranha_open = sprite_from_rows([
    '..GG........GG..',
    '..GRG......GRG..',
    '..GRRG....GRRG..',
    '..GRRRGGGGRRRG..',
    '..GRRRRRRRRRRG..',
    '...GRRRRRRRRG...',
    '...GRRWRRWRRG...',
    '...GRRWRRWRRG...',
    '....GGGGGGGG....',
    '.....GGDDGG.....',
    '.....GGDDGG.....',
    '.....GGDDGG.....',
    '.....GGDDGG.....',
    '....GGGGGGGG....',
    '................',
    '................',
], PCM)

frames = [
    goomba_walk1, goomba_walk2, goomba_squish,         # 0-2
    koopa_walk1, koopa_walk2,                            # 3-4
    koopa_shell, koopa_spin1, koopa_spin2, koopa_spin3, koopa_spin4,  # 5-9
    piranha_closed, piranha_open,                        # 10-11
]
frames = [pad_frame(f, 16, 16) for f in frames]

img = create_sheet(frames, 16, 16)
out = os.path.join(os.path.dirname(__file__), '..', 'assets', 'sprites', 'enemies.png')
img.save(out)
print(f"Saved {out} ({img.size[0]}x{img.size[1]})")
