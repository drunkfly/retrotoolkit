#ifndef COMPILER_LEXERUTILS_H
#define COMPILER_LEXERUTILS_H

#define DIGIT_1_9 \
         '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9'
#define DIGIT \
         '0': case DIGIT_1_9

#define HEXDIGIT \
         'a': case 'b': case 'c': case 'd': case 'e': case 'f': \
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': \
    case DIGIT

#define LETTER \
         'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h': case 'i': case 'j': case 'k': \
    case 'l': case 'm': case 'n': case 'o': case 'p': case 'q': case 'r': case 's': case 't': case 'u': case 'v': \
    case 'w': case 'x': case 'y': case 'z': \
    case 'A': case 'B': case 'C': case 'D': case 'E': case 'F': case 'G': case 'H': case 'I': case 'J': case 'K': \
    case 'L': case 'M': case 'N': case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V': \
    case 'W': case 'X': case 'Y': case 'Z'

bool isDigit(char ch);
bool isBinaryDigit(char ch);
bool isHexDigit(char ch);
bool isIdentifier(char ch);

unsigned charToInt(char ch);

int asciiCode(char ch);

#endif
