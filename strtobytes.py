import sys


def char_to_byte_str(c):
    return format(ord(c), 'x')

if len(sys.argv) == 2 and isinstance(sys.argv[1], str):
    print "".join([char_to_byte_str(c) for c in sys.argv[1]])
