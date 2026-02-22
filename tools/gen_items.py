"""Generate items sprite sheet (16x16).
Layout: 0=mushroom, 1=fire_flower, 2=starman, 3=coin,
        4=1up_mushroom, 5-8=fireball_rotation
"""
import sys, os
sys.path.insert(0, os.path.dirname(__file__))
from nes_palette import *

# === SUPER MUSHROOM ===
MCM = {'R': IR, 'W': IW, 'T': IT, 'K': BK}
mushroom = sprite_from_rows([
    '................',
    '......RRRR......',
    '.....RRRRRR.....',
    '....RRRWRRWRR...',
    '...RRWWRRWWRRR..',
    '...RRWWRRWWRRR..',
    '..RRRRRRRRRRRRR.',
    '..RRRRRRRRRRRRR.',
    '..RRRRRRRRRRRRR.',
    '...TTTTTTTTTTT..',
    '..TTTTTTTTTTTTT.',
    '..TKKTTTTTTKKTT.',
    '..TKKTTTTTTKKTT.',
    '..TTTTTTTTTTTTT.',
    '...TTTTTTTTTTT..',
    '................',
], MCM)

# === FIRE FLOWER ===
FCM = {'R': FF, 'O': FO, 'Y': FY, 'G': PG, 'W': WH}
fire_flower = sprite_from_rows([
    '................',
    '.......YY.......',
    '......YYYY......',
    '...OO.YYYY.OO...',
    '..OOOO.OO.OOOO..',
    '..OOOOOOOOOOO...',
    '...OOOOOOOOO....',
    '...RROOOOORR....',
    '....RRRRRRR.....',
    '......GGG.......',
    '.....GGGGG......',
    '.....GGGGG......',
    '.....GGGGG......',
    '....GGG.GGG.....',
    '...GGG...GGG....',
    '................',
], FCM)

# === STARMAN ===
SCM = {'Y': SY, 'D': SD, 'K': BK}
starman = sprite_from_rows([
    '................',
    '.......YY.......',
    '......YYYY......',
    '......YYYY......',
    '...YYYYYYYYYY...',
    '..YYYYYYYYYYYY..',
    '..YYDYYYYYYDYY..',
    '..YYDDYYYYDDYY..',
    '...YYYYYYYYYY...',
    '...YYYYYYYYYY...',
    '....YYYYYYYY....',
    '....YYYYYYYY....',
    '...YYY....YYY...',
    '..YYYY....YYYY..',
    '..YYY......YYY..',
    '................',
], SCM)

# === COIN ===
CCM = {'G': CO, 'D': CD, 'M': CG}
coin = sprite_from_rows([
    '................',
    '.......GG.......',
    '......GGGG......',
    '.....GGDDGG.....',
    '.....GDDDGG.....',
    '.....GDDDGG.....',
    '.....GDDDGG.....',
    '.....GDDDGG.....',
    '.....GDDDGG.....',
    '.....GDDDGG.....',
    '.....GDDDGG.....',
    '.....GGDDGG.....',
    '......GGGG......',
    '.......GG.......',
    '................',
    '................',
], CCM)

# === 1-UP MUSHROOM ===
LCM = {'G': LG, 'W': LW, 'T': LT, 'K': BK}
oneup = sprite_from_rows([
    '................',
    '......GGGG......',
    '.....GGGGGG.....',
    '....GGGWGGWGG...',
    '...GGWWGGWWGGG..',
    '...GGWWGGWWGGG..',
    '..GGGGGGGGGGGGG.',
    '..GGGGGGGGGGGGG.',
    '..GGGGGGGGGGGGG.',
    '...TTTTTTTTTTT..',
    '..TTTTTTTTTTTTT.',
    '..TKKTTTTTTKKTT.',
    '..TKKTTTTTTKKTT.',
    '..TTTTTTTTTTTTT.',
    '...TTTTTTTTTTT..',
    '................',
], LCM)

# === FIREBALL (4 rotation frames) ===
FBCM = {'R': FF, 'O': FO, 'Y': FY}

fireball1 = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '.....YYYY.......',
    '....YYYYYY......',
    '....YYOOYY......',
    '....YYOOYY......',
    '....YYYYYY......',
    '.....YYYY.......',
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
], FBCM)

fireball2 = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '......YYYY......',
    '.....YYYYYY.....',
    '.....YYOOYY.....',
    '.....YYOOYY.....',
    '.....YYYYYY.....',
    '......YYYY......',
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
], FBCM)

fireball3 = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '.......YYYY.....',
    '......YYYYYY....',
    '......YYOOYY....',
    '......YYOOYY....',
    '......YYYYYY....',
    '.......YYYY.....',
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
], FBCM)

fireball4 = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '......YYYY......',
    '.....YYYYYY.....',
    '.....YRROYYY....',
    '.....YRROYYY....',
    '.....YYYYYY.....',
    '......YYYY......',
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
], FBCM)

frames = [
    mushroom,         # 0
    fire_flower,      # 1
    starman,          # 2
    coin,             # 3
    oneup,            # 4
    fireball1,        # 5
    fireball2,        # 6
    fireball3,        # 7
    fireball4,        # 8
]
frames = [pad_frame(f, 16, 16) for f in frames]

img = create_sheet(frames, 16, 16)
out = os.path.join(os.path.dirname(__file__), '..', 'assets', 'sprites', 'items.png')
img.save(out)
print(f"Saved {out} ({img.size[0]}x{img.size[1]})")
