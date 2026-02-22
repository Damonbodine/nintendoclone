"""Generate small Mario sprite sheet (16x16, 7 frames)."""
import sys, os
sys.path.insert(0, os.path.dirname(__file__))
from nes_palette import *

# Color map: R=red, S=skin, B=brown, .=transparent
CM = {'R': MR, 'S': MS, 'B': MB}

# Frame 0: Idle (standing, facing right)
idle = sprite_from_rows([
    '......RRRRR.....',
    '.....RRRRRRRRR..',
    '.....BBBSSBS....',
    '....BSBSSSBSSS..',
    '....BSBSSSBS.SS.',
    '....BBSSSSBB....',
    '......SSSSSS....',
    '....RRSRRRSR....',
    '...SRRRSRRRSSS..',
    '...SRRRSSRRRSSS.',
    '...SS.RRRRRR.S..',
    '......RRRRRR....',
    '.....RR..RR.....',
    '....BBB..BBB....',
    '...BBBB..BBBB...',
    '................',
], CM)

# Frame 1: Walk 1 (right foot forward)
walk1 = sprite_from_rows([
    '......RRRRR.....',
    '.....RRRRRRRRR..',
    '.....BBBSSBS....',
    '....BSBSSSBSSS..',
    '....BSBSSSBS.SS.',
    '....BBSSSSBB....',
    '......RRSRRR....',
    '.....RRRRRRRR...',
    '....SSRRRSRRRS..',
    '....SSSRRRRRRS..',
    '....SS.RRRRRR...',
    '......RRRR......',
    '.....RRRBB......',
    '....BBB.BBBB....',
    '....BBBB........',
    '................',
], CM)

# Frame 2: Walk 2 (mid stride)
walk2 = sprite_from_rows([
    '......RRRRR.....',
    '.....RRRRRRRRR..',
    '.....BBBSSBS....',
    '....BSBSSSBSSS..',
    '....BSBSSSBS.SS.',
    '....BBSSSSBB....',
    '......SSSSSS....',
    '....RRSRRRSR....',
    '...SRRRSRRRSSS..',
    '...SRRRSSRRRSSS.',
    '...SS.RRRRRR.S..',
    '......RRRRRR....',
    '.....BBB.BBB....',
    '....BBB..BBB....',
    '................',
    '................',
], CM)

# Frame 3: Walk 3 (left foot forward)
walk3 = sprite_from_rows([
    '......RRRRR.....',
    '.....RRRRRRRRR..',
    '.....BBBSSBS....',
    '....BSBSSSBSSS..',
    '....BSBSSSBS.SS.',
    '....BBSSSSBB....',
    '.....RRRSRR.....',
    '....RRRRRRRRS...',
    '...SRRRRSRRRSS..',
    '...SRRRRRRSSS...',
    '....RRRRRR.SS...',
    '.......RRRR.....',
    '......BB.RRR....',
    '....BBBB.BBB....',
    '........BBBB....',
    '................',
], CM)

# Frame 4: Jump (arms up, legs tucked)
jump = sprite_from_rows([
    '......RRRRR.....',
    '.....RRRRRRRRR..',
    '.....BBBSSBS....',
    '....BSBSSSBSSS..',
    '....BSBSSSBS.SS.',
    '....BBSSSSBB....',
    '......SSSSSS....',
    '...SRRSRRRSR....',
    '..SSRRRRRRRRSSS.',
    '..BSRRRRRRRRSB..',
    '..BB.RRRRRR.BB..',
    '.....RRRRRR.....',
    '....RR....RR....',
    '...BBB....BBB...',
    '................',
    '................',
], CM)

# Frame 5: Skid (turning around)
skid = sprite_from_rows([
    '......RRRRR.....',
    '.....RRRRRRRRR..',
    '.....BBBSSBS....',
    '....BSBSSSBSSS..',
    '....BSBSSSBS.SS.',
    '....BBSSSSBB....',
    '......SSSSSS....',
    '...RRSRRRRS.....',
    '..SRRRRRRRRRR...',
    '..SSRRRRRRRRS...',
    '..SS.RRRRRR.S...',
    '......RRRRRR....',
    '......BB..RR....',
    '.....BBB.BBB....',
    '...BBBB.........',
    '................',
], CM)

# Frame 6: Die (face-up falling)
die = sprite_from_rows([
    '......RRRRR.....',
    '.....RRRRRRRRR..',
    '.....BBBSSBS....',
    '....BSBSSSBSSS..',
    '....BSBSSSBS.SS.',
    '....BBSSSSBB....',
    '...SSRRRRRRSS...',
    '..SSSRRSRRRSSS..',
    '..SSSRRSRRRSSS..',
    '..SS.RRRRRR.SS..',
    '......RRRRRR....',
    '....RR....RR....',
    '...BBB....BBB...',
    '..BBBB....BBBB..',
    '................',
    '................',
], CM)

frames = [idle, walk1, walk2, walk3, jump, skid, die]
# Pad all to 16x16
frames = [pad_frame(f, 16, 16) for f in frames]

img = create_sheet(frames, 16, 16)
out = os.path.join(os.path.dirname(__file__), '..', 'assets', 'sprites', 'mario_small.png')
img.save(out)
print(f"Saved {out} ({img.size[0]}x{img.size[1]})")
