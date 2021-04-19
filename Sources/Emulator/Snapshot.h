#ifndef EMULATOR_SNAPSHOT_H
#define EMULATOR_SNAPSHOT_H

#include "Emulator/Common.h"

#pragma pack(push, 1)

struct SnapshotState
{
    uint8_t a;
    uint8_t f;
    uint8_t b;
    uint8_t c;
    uint8_t d;
    uint8_t e;
    uint8_t l;
    uint8_t h;
    uint8_t shadowA;
    uint8_t shadowF;
    uint8_t shadowB;
    uint8_t shadowC;
    uint8_t shadowD;
    uint8_t shadowE;
    uint8_t shadowL;
    uint8_t shadowH;
    uint8_t ixH;
    uint8_t ixL;
    uint8_t iyH;
    uint8_t iyL;
    uint8_t i;
    uint8_t r;
    uint8_t intMode;
    uint8_t intDisabled;
    uint8_t pcL;
    uint8_t pcH;
    uint8_t spL;
    uint8_t spH;
    uint8_t port1FFD;
    uint8_t port7FFD;
    uint8_t portFFFD;
    uint8_t borderColor;
};

#pragma pack(pop)

void loadSnapshot(const char* path, SnapshotState& state, uint8_t* memory);

#endif
