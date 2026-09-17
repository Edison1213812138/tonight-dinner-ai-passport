#!/usr/bin/env python3
"""Fail packaging if either installable image reaches the strict 3 MB limit."""
from pathlib import Path
import sys
root=Path(sys.argv[1])
for name in ('FoloToy-AI-Passport.bin','FoloToy-AI-Passport-full.bin'):
    size=(root/name).stat().st_size
    if not 0<size<3_000_000:raise SystemExit(f'{name}: {size} bytes violates <3 MB')
    print(f'Dinner size: {name}: {size} bytes < 3,000,000 PASS')
