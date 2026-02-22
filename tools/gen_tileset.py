"""Generate overworld tileset (16x16, 32 tiles in a single row).
Tile indices:
 0=ground, 1=brick, 2-4=question(3 anim frames), 5=used_block,
 6=pipe_top_left, 7=pipe_top_right, 8=pipe_body_left, 9=pipe_body_right,
 10=flagpole_top, 11=flagpole_shaft, 12=castle_block, 13=castle_battlement,
 14=castle_door, 15=castle_window, 16=stair_block, 17=coin_tile,
 18-23=cloud(6 parts), 24-26=bush(3 parts), 27-31=hill(5 parts)
"""
import sys, os
sys.path.insert(0, os.path.dirname(__file__))
from nes_palette import *

# ============================================================
# TILE 0: GROUND
# ============================================================
g_cm = {'1': G1, '2': G2, '3': G3}
ground = sprite_from_rows([
    '2222222222222221',
    '2111111111111131',
    '2111111111111131',
    '2111311111311131',
    '2111311111311131',
    '2111111111111131',
    '2111111111111131',
    '2133333333333331',
    '2111111111111131',
    '2111111111111131',
    '2111131111131131',
    '2111131111131131',
    '2111111111111131',
    '2111111111111131',
    '2133333333333331',
    '3333333333333333',
], g_cm)

# ============================================================
# TILE 1: BRICK
# ============================================================
brick = sprite_from_rows([
    '2222222222222222',
    '2111112111111213',
    '2111112111111213',
    '3333333333333333',
    '2222222222222222',
    '1112111111211113',
    '1112111111211113',
    '3333333333333333',
    '2222222222222222',
    '2111112111111213',
    '2111112111111213',
    '3333333333333333',
    '2222222222222222',
    '1112111111211113',
    '1112111111211113',
    '3333333333333333',
], g_cm)

# ============================================================
# TILES 2-4: QUESTION BLOCK (3 animation frames)
# ============================================================
q_cm = {'O': Q1, 'Y': Q2, 'D': Q3, 'K': BK}

question1 = sprite_from_rows([
    'DDDDDDDDDDDDDDDD',
    'DYYYYYYYYYYYYYYY.',
    'DYOOOOOOOOOOOOO..',
    'DYOOOOO..OOOOO..',
    'DYOOOO.YY.OOOO..',
    'DYOOOO.YY.OOOO..',
    'DYOOOOOOO.OOOOO.',
    'DYOOOOOO.OOOOOO.',
    'DYOOOOO.OOOOOOO.',
    'DYOOOOO.OOOOOOO.',
    'DYOOOOOOOOOOOO...',
    'DYOOOOO.OOOOOOO.',
    'DYOOOOO.OOOOOOO.',
    'DY...............',
    'D................',
    '.................',
], q_cm)[:16]

# Fix to 16 wide
q_cm2 = {'O': Q1, 'Y': Q2, 'D': Q3, 'K': BK}

question1 = sprite_from_rows([
    'KKKKKKKKKKKKKKKK',
    'KYYYYYYYYYYYYYY.',
    'KYOOOOOOOOOOOO..',
    'KYOOOO..OOOOO...',
    'KYOOO.YY.OOOO...',
    'KYOOO.YY.OOOO...',
    'KYOOOOOO.OOOOO..',
    'KYOOOOO.OOOOOO..',
    'KYOOOO.OOOOOOO..',
    'KYOOOO.OOOOOOO..',
    'KYOOOOOOOOOOO...',
    'KYOOOO.OOOOOOO..',
    'KYOOOO.OOOOOOO..',
    'KY..............',
    'K...............',
    '................',
], q_cm2)

# Question frame 2 (slightly different shimmer)
question2 = sprite_from_rows([
    'KKKKKKKKKKKKKKKK',
    'KYYYYYYYYYYYYYY.',
    'KYOOOOOOOOOOOO..',
    'KYOOOOO.OOOOO...',
    'KYOOOO.YY.OOO...',
    'KYOOOO.YY.OOO...',
    'KYOOOOOOO.OOOO..',
    'KYOOOOOO.OOOOO..',
    'KYOOOOO.OOOOOO..',
    'KYOOOOO.OOOOOO..',
    'KYOOOOOOOOOOOO..',
    'KYOOOOO.OOOOOO..',
    'KYOOOOO.OOOOOO..',
    'KY..............',
    'K...............',
    '................',
], q_cm2)

# Question frame 3 (shimmer shifted)
question3 = sprite_from_rows([
    'KKKKKKKKKKKKKKKK',
    'KYYYYYYYYYYYYYY.',
    'KYOOOOOOOOOOOO..',
    'KYOOOOOO.OOOO...',
    'KYOOOOO.YY.OO...',
    'KYOOOOO.YY.OO...',
    'KYOOOOOOOO.OOO..',
    'KYOOOOOOO.OOOO..',
    'KYOOOOOO.OOOOO..',
    'KYOOOOOO.OOOOO..',
    'KYOOOOOOOOOOO...',
    'KYOOOOOO.OOOOO..',
    'KYOOOOOO.OOOOO..',
    'KY..............',
    'K...............',
    '................',
], q_cm2)

# ============================================================
# TILE 5: USED BLOCK
# ============================================================
u_cm = {'1': U1, '2': U2}
used_block = sprite_from_rows([
    '2222222222222222',
    '2111111111111112',
    '2111111111111112',
    '2111111111111112',
    '2111111111111112',
    '2111111111111112',
    '2111111111111112',
    '2111111111111112',
    '2111111111111112',
    '2111111111111112',
    '2111111111111112',
    '2111111111111112',
    '2111111111111112',
    '2111111111111112',
    '2222222222222222',
    '................',
], u_cm)

# ============================================================
# TILES 6-9: PIPE
# ============================================================
p_cm = {'G': P1, 'D': P2, 'L': P3}

pipe_top_left = sprite_from_rows([
    'DDGGGGGGGGGGGGGG',
    'DLLLLLLLLLLLLLLG',
    'DLGGGGGGGGGGGGDG',
    'DLGGGGGGGGGGGGDG',
    'DDGGGGGGGGGGGGGG',
    'DDGGGGGGGGGGGGGG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
], p_cm)

pipe_top_right = sprite_from_rows([
    'GGGGGGGGGGGGGGDD',
    'GLLLLLLLLLLLLLDD',
    'GDGGGGGGGGGGGLDD',
    'GDGGGGGGGGGGGLDD',
    'GGGGGGGGGGGGGGDD',
    'GGGGGGGGGGGGGGDD',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
], p_cm)

pipe_body_left = sprite_from_rows([
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
    '.DLGGGGGGGGGGGDG',
], p_cm)

pipe_body_right = sprite_from_rows([
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
    'GDGGGGGGGGGGGLD.',
], p_cm)

# ============================================================
# TILES 10-11: FLAGPOLE
# ============================================================
f_cm = {'G': FG, 'D': FD, 'L': FL}

flagpole_top = sprite_from_rows([
    '......LLLL......',
    '.....LLLLLL.....',
    '.....LLLLLL.....',
    '.....LLLLLL.....',
    '......LLLL......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
], f_cm)

flagpole_shaft = sprite_from_rows([
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
    '.......GG.......',
], f_cm)

# ============================================================
# TILES 12-15: CASTLE
# ============================================================
c_cm = {'1': C1, '2': C2, '3': C3}

castle_block = sprite_from_rows([
    '1111111111111111',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1111111111111111',
    '3333333333333333',
], c_cm)

castle_battlement = sprite_from_rows([
    '111..........111',
    '121..........121',
    '121..........121',
    '121..........121',
    '111..........111',
    '1111111111111111',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1111111111111111',
    '3333333333333333',
], c_cm)

castle_door = sprite_from_rows([
    '1111111111111111',
    '1222222222222221',
    '1222233333322221',
    '1222333333332221',
    '1223333333333221',
    '1223333333333221',
    '1233333333333321',
    '1233333333333321',
    '1233333333333321',
    '1233333333333321',
    '1233333333333321',
    '1233333333333321',
    '1233333333333321',
    '1233333333333321',
    '1233333333333321',
    '1233333333333321',
], c_cm)

castle_window = sprite_from_rows([
    '1111111111111111',
    '1222222222222221',
    '1222222222222221',
    '1222233333322221',
    '1222333333332221',
    '1223333333333221',
    '1223333333333221',
    '1223333333333221',
    '1222333333332221',
    '1222233333322221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1222222222222221',
    '1111111111111111',
    '3333333333333333',
], c_cm)

# ============================================================
# TILE 16: STAIR BLOCK
# ============================================================
s_cm = {'1': S1, '2': S2, '3': G3}
stair_block = sprite_from_rows([
    '2222222222222221',
    '2111111111111131',
    '2111111111111131',
    '2111311111311131',
    '2111311111311131',
    '2111111111111131',
    '2111111111111131',
    '2133333333333331',
    '2111111111111131',
    '2111111111111131',
    '2111131111131131',
    '2111131111131131',
    '2111111111111131',
    '2111111111111131',
    '2133333333333331',
    '3333333333333333',
], s_cm)

# ============================================================
# TILE 17: COIN (small in-world coin)
# ============================================================
cn_cm = {'G': CO, 'D': CD}
coin_tile = sprite_from_rows([
    '................',
    '......GGGG......',
    '.....GGGGGG.....',
    '....GGDDDDGG....',
    '....GDDDDDGG....',
    '....GDDDDDGG....',
    '....GDDDDDGG....',
    '....GDDDDDGG....',
    '....GDDDDDGG....',
    '....GDDDDDGG....',
    '....GDDDDDGG....',
    '....GGDDDDGG....',
    '.....GGGGGG.....',
    '......GGGG......',
    '................',
    '................',
], cn_cm)

# ============================================================
# TILES 18-23: CLOUD (6 parts: top-left/mid/right, bot-left/mid/right)
# ============================================================
cl_cm = {'W': CW, 'L': CL, 'E': CE}

cloud_left = sprite_from_rows([
    '................',
    '................',
    '................',
    '............LLLL',
    '..........LLWWWW',
    '........LLWWWWWW',
    '.......LWWWWWWWW',
    '......LWWWWWWWWW',
    '.....LWWWWWWWWWW',
    '....LWWWWWWWWWWW',
    '...LWWWWWEWWWWWW',
    '..LWWWWWEEWWWWWW',
    '..LWWWWWWWWWWWWW',
    '...LLWWWWWWWWWWW',
    '....LLLLLLLLLLLL',
    '................',
], cl_cm)

cloud_mid = sprite_from_rows([
    '................',
    '................',
    '......LLLL......',
    'LLLLLLWWWWLLLLLL',
    'WWWWWWWWWWWWWWWW',
    'WWWWWWWWWWWWWWWW',
    'WWWWWWWWWWWWWWWW',
    'WWWWWWWWWWWWWWWW',
    'WWWWWWWWWWWWWWWW',
    'WWWWWWWWWWWWWWWW',
    'WWWWWWWWWWWWWWWW',
    'WWWWWWWWWWWWWWWW',
    'WWWWWWWWWWWWWWWW',
    'WWWWWWWWWWWWWWWW',
    'LLLLLLLLLLLLLLLL',
    '................',
], cl_cm)

cloud_right = sprite_from_rows([
    '................',
    '................',
    '................',
    'LLLL............',
    'WWWWLL..........',
    'WWWWWWLL........',
    'WWWWWWWL........',
    'WWWWWWWWL.......',
    'WWWWWWWWWL......',
    'WWWWWWWWWWL.....',
    'WWWWWEWWWWWL....',
    'WWWWWEEWWWWL....',
    'WWWWWWWWWWWL....',
    'WWWWWWWWWLL.....',
    'LLLLLLLLLL......',
    '................',
], cl_cm)

cloud_bottom_left = sprite_from_rows([
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
    '................',
    '................',
    '................',
    '................',
    '................',
], cl_cm)  # Empty - clouds are just the top 3 tiles

cloud_bottom_mid = sprite_from_rows([
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
    '................',
    '................',
    '................',
    '................',
    '................',
], cl_cm)

cloud_bottom_right = sprite_from_rows([
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
    '................',
    '................',
    '................',
    '................',
    '................',
], cl_cm)

# ============================================================
# TILES 24-26: BUSH (left, mid, right)
# ============================================================
b_cm = {'G': BG, 'D': BD, 'L': BL}

bush_left = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
    '............GGGG',
    '..........GGGGGG',
    '.........GGLGGGG',
    '........GGLLGGGG',
    '.......GGGGGGGGG',
    '......GGGGGGGGGG',
    '.....GGGGGGGGGGG',
    '....DDDDDDDDDDDD',
], b_cm)

bush_mid = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '......GGGG......',
    '.....GGGGGG.....',
    '....GGLGGGGG....',
    '...GGLLGGGGGG...',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'DDDDDDDDDDDDDDDD',
], b_cm)

bush_right = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
    'GGGG............',
    'GGGGGG..........',
    'GGGGLGG.........',
    'GGGGLLGG........',
    'GGGGGGGGG.......',
    'GGGGGGGGGG......',
    'GGGGGGGGGGG.....',
    'DDDDDDDDDDDD....',
], b_cm)

# ============================================================
# TILES 27-31: HILL (top, left, right, fill, spot)
# ============================================================
h_cm = {'G': HG, 'D': HD, 'L': HL}

hill_top = sprite_from_rows([
    '................',
    '................',
    '................',
    '................',
    '................',
    '................',
    '......DDDD......',
    '.....DGGGGD.....',
    '....DGGGGGGD....',
    '...DGLGGGGGGD...',
    '..DGGLLGGGGGD...',
    '.DGGGGGGGGGGGGD.',
    'DGGGGGGGGLGGGGD.',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
], h_cm)

hill_left = sprite_from_rows([
    'DGGGGGGGGGGGGGG.',
    'DGGGGLGGGGGGGGG.',
    'DGGGGLLGGGGGGGG.',
    'DGGGGGGGGGGGGGGG',
    'DGGGGGGGGGGGGGGG',
    'DGGGGGGGGGGGGGGG',
    'DGGGGGGGGLGGGGGG',
    'DGGGGGGGGLLGGGGG',
    'DGGGGGGGGGGGGGGG',
    'DGGGGGGGGGGGGGGG',
    'DGGGGLGGGGGGGGG.',
    'DGGGGLLGGGGGGGG.',
    'DGGGGGGGGGGGGGGG',
    'DGGGGGGGGGGGGGGG',
    'DGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
], h_cm)

hill_right = sprite_from_rows([
    '.GGGGGGGGGGGGGGD',
    '.GGGGGGGGGLGGGGD',
    '.GGGGGGGGGLLGGGD',
    'GGGGGGGGGGGGGGGD',
    'GGGGGGGGGGGGGGGD',
    'GGGGGGGGGGGGGGGD',
    'GGGGGLGGGGGGGGGD',
    'GGGGGLLGGGGGGGGD',
    'GGGGGGGGGGGGGGGD',
    'GGGGGGGGGGGGGGGD',
    '.GGGGGGGGGLGGGGD',
    '.GGGGGGGGGLLGGGD',
    'GGGGGGGGGGGGGGGD',
    'GGGGGGGGGGGGGGGD',
    'GGGGGGGGGGGGGGGD',
    'GGGGGGGGGGGGGGGG',
], h_cm)

hill_fill = sprite_from_rows([
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
], h_cm)

hill_spot = sprite_from_rows([
    'GGGGGGGGGGGGGGGG',
    'GGGGLGGGGGGLGGGG',
    'GGGGLLGGGGGGLLGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGLGGGGGGGLGGGG',
    'GGGLLGGGGGGGLLGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGLGGGGGGLGGGG',
    'GGGGLLGGGGGGLLGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
    'GGGGGGGGGGGGGGGG',
], h_cm)

# ============================================================
# ASSEMBLE ALL 32 TILES
# ============================================================
all_tiles = [
    ground,            # 0
    brick,             # 1
    question1,         # 2
    question2,         # 3
    question3,         # 4
    used_block,        # 5
    pipe_top_left,     # 6
    pipe_top_right,    # 7
    pipe_body_left,    # 8
    pipe_body_right,   # 9
    flagpole_top,      # 10
    flagpole_shaft,    # 11
    castle_block,      # 12
    castle_battlement, # 13
    castle_door,       # 14
    castle_window,     # 15
    stair_block,       # 16
    coin_tile,         # 17
    cloud_left,        # 18
    cloud_mid,         # 19
    cloud_right,       # 20
    cloud_bottom_left, # 21
    cloud_bottom_mid,  # 22
    cloud_bottom_right,# 23
    bush_left,         # 24
    bush_mid,          # 25
    bush_right,        # 26
    hill_top,          # 27
    hill_left,         # 28
    hill_right,        # 29
    hill_fill,         # 30
    hill_spot,         # 31
]

all_tiles = [pad_frame(t, 16, 16) for t in all_tiles]

img = create_sheet(all_tiles, 16, 16)
out = os.path.join(os.path.dirname(__file__), '..', 'assets', 'tiles', 'overworld_tileset.png')
img.save(out)
print(f"Saved {out} ({img.size[0]}x{img.size[1]}, {len(all_tiles)} tiles)")
