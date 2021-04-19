#include "GameData.h"
#include "Emulator/Snapshot.h"

SnapshotState initState = { 'R', 'E', 'G', 'I', 'S', 'T', 'E', 'R', 'S', '\x1A', '\x1A' };
uint8_t initMemory[Z80Memory::BankCount * Z80Memory::BankSize] = {'M','E','M','O','R','Y','\x1A','\x1A'};
