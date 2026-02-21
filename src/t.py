import struct

# Liste des instructions (en 16-bit word)
instructions = [
    0xE00A,
    0xE01A,
    0x1701,
    0xF409,
    0xE001
]

# Écriture little-endian (ATmega328P)
with open("test.bin", "wb") as f:
    f.write(struct.pack(f"<{len(instructions)}H", *instructions))

print("Fichier test.bin généré !")
