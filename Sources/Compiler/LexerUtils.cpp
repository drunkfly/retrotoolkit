#include "LexerUtils.h"

bool isDigit(char ch)
{
    switch (ch) {
        case DIGIT:
            return true;
        default:
            return false;
    }
}

bool isBinaryDigit(char ch)
{
    switch (ch) {
        case '0':
        case '1':
            return true;
        default:
            return false;
    }
}

bool isHexDigit(char ch)
{
    switch (ch) {
        case HEXDIGIT:
            return true;
        default:
            return false;
    }
}

bool isIdentifier(char ch)
{
    switch (ch) {
        case DIGIT:
        case LETTER:
        case '_':
            return true;
        default:
            return false;
    }
}

unsigned charToInt(char ch)
{
    switch (ch) {
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'a': case 'A': return 10;
        case 'b': case 'B': return 11;
        case 'c': case 'C': return 12;
        case 'd': case 'D': return 13;
        case 'e': case 'E': return 14;
        case 'f': case 'F': return 15;
        default: return 0;
    }
}
