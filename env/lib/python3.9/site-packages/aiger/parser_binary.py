def decode(byte_stream) -> int:
    literal = 0
    for i, val in enumerate(map(ord, byte_stream)):
        literal |= (val & 0x7f) << (7 * i)

        if val & 0x80:
            break
    return literal


def encode(literal: int):
    while (literal & ~0x7f) > 0:
        yield chr((literal & 0x7f) | 0x80)
        literal >>= 7
    yield chr(literal)
