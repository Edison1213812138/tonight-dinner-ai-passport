#!/usr/bin/env python3
"""Dependency-free inventory drift check; actual descriptors tested by LVGL host UI."""
from pathlib import Path
import json,re
root=Path(__file__).resolve().parents[1]
needed=set(range(32,127))
for p in (root/'main').glob('dinner_*.c'):
    for s in re.findall(r'"((?:[^"\\]|\\.)*)"',p.read_text()):needed.update(ord(c) for c in s if ord(c)>=32)
manifest=json.loads((root/'assets/fonts/inventory.json').read_text())
assert needed==set(manifest['codepoints']), 'Font inventory drift: regenerate all sizes'
for size in manifest['sizes']:
    text=(root/f'assets/fonts/dinner_font_{size}.c').read_text()
    values=re.search(r'unicode_list\[\]=\{([^}]+)',text)[1]
    assert {int(x)+32 for x in values.split(',')}==needed
print(f'Dinner fonts: {len(needed)} codepoints x 3 sizes; inventory PASS')
