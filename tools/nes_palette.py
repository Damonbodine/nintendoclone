"""NES Super Mario Bros color palette and sprite generation helpers."""
from PIL import Image

# Transparent
T = (0, 0, 0, 0)

# Mario colors
MR = (181, 49, 32, 255)      # Mario Red - hat, shirt
MS = (228, 166, 114, 255)    # Mario Skin - face, hands
MB = (107, 66, 12, 255)      # Mario Brown - hair, shoes, outline

# Goomba colors
GB = (172, 124, 0, 255)      # Goomba Brown - body
GD = (107, 66, 12, 255)      # Goomba Dark - feet
GT = (228, 166, 114, 255)    # Goomba Tan - face
GK = (0, 0, 0, 255)          # Goomba Black - eyes

# Koopa colors
KG = (0, 168, 0, 255)        # Koopa Green - shell
KD = (0, 104, 0, 255)        # Koopa Dark green
KS = (228, 166, 114, 255)    # Koopa Skin
KW = (252, 252, 252, 255)    # Koopa White

# Piranha Plant
PG = (0, 168, 0, 255)        # Piranha Green
PD = (0, 104, 0, 255)        # Piranha Dark
PW = (252, 252, 252, 255)    # Piranha White/spots
PR = (181, 49, 32, 255)      # Piranha Red - head

# Tile colors
BK = (0, 0, 0, 255)          # Black
WH = (252, 252, 252, 255)    # White

# Ground/Brick
G1 = (200, 76, 12, 255)      # Ground main brown
G2 = (228, 148, 92, 255)     # Ground highlight
G3 = (136, 20, 0, 255)       # Ground dark / mortar

# Question block
Q1 = (228, 148, 0, 255)      # Question orange
Q2 = (252, 188, 60, 255)     # Question yellow
Q3 = (200, 76, 12, 255)      # Question dark

# Used block
U1 = (168, 124, 80, 255)     # Used block tan
U2 = (104, 68, 36, 255)      # Used block dark

# Pipe
P1 = (0, 168, 0, 255)        # Pipe green
P2 = (0, 104, 56, 255)       # Pipe dark
P3 = (128, 208, 16, 255)     # Pipe highlight

# Flagpole
FG = (168, 168, 168, 255)    # Flag gray
FD = (104, 104, 104, 255)    # Flag dark
FL = (0, 168, 0, 255)        # Flag green (banner)

# Castle
C1 = (168, 168, 168, 255)    # Castle gray
C2 = (104, 104, 104, 255)    # Castle dark
C3 = (60, 60, 60, 255)       # Castle very dark

# Stair
S1 = (200, 76, 12, 255)      # Stair orange/brown
S2 = (228, 148, 92, 255)     # Stair highlight

# Decorations
CW = (252, 252, 252, 255)    # Cloud white
CL = (172, 220, 252, 255)    # Cloud light blue
CE = (132, 196, 252, 255)    # Cloud eye outline

BG = (0, 168, 0, 255)        # Bush green
BD = (0, 120, 0, 255)        # Bush dark
BL = (128, 208, 16, 255)     # Bush light

HG = (0, 168, 68, 255)       # Hill green
HD = (0, 120, 0, 255)        # Hill dark
HL = (128, 208, 16, 255)     # Hill light/spots

# Coin
CO = (252, 188, 60, 255)     # Coin gold
CD = (200, 120, 0, 255)      # Coin dark
CG = (228, 148, 0, 255)      # Coin mid

# Items
IR = (181, 49, 32, 255)      # Item red (mushroom cap)
IW = (252, 252, 252, 255)    # Item white
IT = (228, 166, 114, 255)    # Item tan

SY = (252, 188, 60, 255)     # Star yellow
SD = (200, 76, 12, 255)      # Star dark
SB = (0, 0, 0, 255)          # Star black (eyes)

FF = (252, 56, 0, 255)       # Fire red
FO = (252, 152, 56, 255)     # Fire orange
FY = (252, 224, 108, 255)    # Fire yellow

# 1-up mushroom
LG = (0, 168, 0, 255)        # 1up green cap
LW = (252, 252, 252, 255)    # 1up white spots
LT = (228, 166, 114, 255)    # 1up tan stem


def sprite_from_rows(rows, color_map):
    """Convert a list of string rows into a 2D pixel array.
    Each character in the string maps to a color via color_map.
    '.' is always transparent."""
    pixels = []
    for row in rows:
        pixel_row = []
        for ch in row:
            if ch == '.':
                pixel_row.append(T)
            else:
                pixel_row.append(color_map[ch])
        pixels.append(pixel_row)
    return pixels


def create_sheet(frames, frame_w, frame_h):
    """Create a sprite sheet image from a list of pixel arrays.
    Each frame is placed side-by-side horizontally."""
    width = frame_w * len(frames)
    height = frame_h
    img = Image.new('RGBA', (width, height), T)

    for i, frame in enumerate(frames):
        for y, row in enumerate(frame):
            for x, color in enumerate(row):
                if color != T:
                    img.putpixel((i * frame_w + x, y), color)
    return img


def create_grid_sheet(frames, frame_w, frame_h, cols):
    """Create a sprite sheet as a grid with given column count."""
    rows_count = (len(frames) + cols - 1) // cols
    width = frame_w * cols
    height = frame_h * rows_count
    img = Image.new('RGBA', (width, height), T)

    for i, frame in enumerate(frames):
        col = i % cols
        row = i // cols
        for y, prow in enumerate(frame):
            for x, color in enumerate(prow):
                if color != T:
                    img.putpixel((col * frame_w + x, row * frame_h + y), color)
    return img


def pad_frame(rows, width, height):
    """Pad a frame to exact width x height with transparent pixels."""
    result = []
    for y in range(height):
        if y < len(rows):
            row = list(rows[y])
            # Pad width
            while len(row) < width:
                row.append(T)
            result.append(row[:width])
        else:
            result.append([T] * width)
    return result
