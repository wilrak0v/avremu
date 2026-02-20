import struct

# Liste des instructions (en 16-bit word)
instructions = [
    0xE002,
    0xE0FF,
    0x0F01,
    0xF7F1
]

# Écriture little-endian (ATmega328P)
with open("test.bin", "wb") as f:
    f.write(struct.pack(f"<{len(instructions)}H", *instructions))

print("Fichier test.bin généré !")
