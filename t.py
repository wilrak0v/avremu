import struct

# Liste des instructions (en 16-bit word)
instructions = [
    0xE005,  # ldi r16,5
    0xE103,  # ldi r17,3
    0x3011,  # cp r16,r17
    0xCFFF   # rjmp -1
]

# Écriture little-endian (ATmega328P)
with open("test.bin", "wb") as f:
    f.write(struct.pack("<4H", *instructions))

print("Fichier cp_test.bin généré !")
