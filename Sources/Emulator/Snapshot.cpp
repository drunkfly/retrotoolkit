#include "Snapshot.h"
#include "Emulator/Util/LargeBuffer.h"
#include "Emulator/Z80Memory.h"

static void writeByte(uint8_t* memory, int bank, size_t& addr, uint8_t value)
{
    if (addr > 0xffff)
        throw std::runtime_error("Invalid snapshot file.");

    size_t off = Z80Memory::resolveAddress(bank, addr);
    memory[off] = value;

    ++addr;
}

void loadSnapshot(const char* path, SnapshotState& state, uint8_t* memory)
{
    memset(memory, 0, Z80Memory::BankCount * Z80Memory::BankSize);

    LargeBuffer buffer(1048576);
    buffer.readFile(path);

    // Because of compatibility, if byte 12 is 255, it has to be regarded as being 1.
    if (buffer[12] == 0xff)
        buffer[12] = 1;

    state.a = buffer[0];
    state.f = buffer[1];
    state.c = buffer[2];
    state.b = buffer[3];
    state.l = buffer[4];
    state.h = buffer[5];
    state.pcL = buffer[6];
    state.pcH = buffer[7];
    state.spL = buffer[8];
    state.spH = buffer[9];
    state.i = buffer[10];
    state.r = (buffer[11] & 0x7f) | ((buffer[12] & 1) << 7);
    state.borderColor = (buffer[12] >> 1) & 7;
    bool compressed = (buffer[12] & 0x20) != 0;
    state.e = buffer[13];
    state.d = buffer[14];
    state.shadowC = buffer[15];
    state.shadowB = buffer[16];
    state.shadowE = buffer[17];
    state.shadowD = buffer[18];
    state.shadowL = buffer[19];
    state.shadowH = buffer[20];
    state.shadowA = buffer[21];
    state.shadowF = buffer[22];
    state.iyL = buffer[23];
    state.iyH = buffer[24];
    state.ixL = buffer[25];
    state.ixH = buffer[26];
    state.intDisabled = (buffer[27] == 0);
    state.intMode = buffer[29] & 3;

    if (state.pcL != 0 || state.pcH != 0) {
        // version 1
        if (!compressed) {
            buffer.readAt(30 + 0 * Z80Memory::BankSize, &memory[5 * Z80Memory::BankSize], Z80Memory::BankSize);
            buffer.readAt(30 + 1 * Z80Memory::BankSize, &memory[2 * Z80Memory::BankSize], Z80Memory::BankSize);
            buffer.readAt(30 + 2 * Z80Memory::BankSize, &memory[0 * Z80Memory::BankSize], Z80Memory::BankSize);
        } else {
            size_t off = 30;
            size_t addr = 0x4000;
            for (;;) {
                if (buffer[off] == 0 && buffer[off + 1] == 0xED && buffer[off + 2] == 0xED && buffer[off + 3] == 0)
                    break;
                else if (buffer[off] != 0xED || buffer[off + 1] != 0xED)
                    writeByte(memory, 0, addr, buffer[off++]);
                else {
                    off += 2;
                    size_t count = buffer[off++];
                    uint8_t byte = buffer[off++];
                    for (size_t i = 0; i < count; i++)
                        writeByte(memory, 0, addr, byte);
                }
            }

            memcpy(&memory[2 * Z80Memory::BankSize], &memory[6 * Z80Memory::BankSize], Z80Memory::BankSize);
            memcpy(&memory[0 * Z80Memory::BankSize], &memory[7 * Z80Memory::BankSize], Z80Memory::BankSize);
            memset(&memory[6 * Z80Memory::BankSize], 0, Z80Memory::BankSize);
            memset(&memory[7 * Z80Memory::BankSize], 0, Z80Memory::BankSize);
        }
    } else {
        // version 2 or 3
        uint8_t v;
        uint16_t length = buffer[30] | (buffer[31] << 8);
        if (length == 23)
            v = 2;
        else if (length == 54 || length == 55)
            v = 3;
        else
            throw std::runtime_error("Unsupported z80 file version.");

        state.pcL = buffer[32];
        state.pcH = buffer[33];

        uint8_t machine = buffer[34];
        bool is48k = false, isTimex = false;
        switch (machine) {
            case 0:
            case 1:
                is48k = true;
                break;
            case 3:
                if (v == 3)
                    is48k = true;
                break;
            case 14:
            case 15:
            case 128:
                is48k = true;
                isTimex = true;
                break;
            default:
                throw std::runtime_error("Unsupported z80 hardware mode.");
        }

        state.port7FFD = 0;
        if (isTimex)
            ; // FIXME: [35] last OUT to 0xf4
        else if (!is48k)
            state.port7FFD = buffer[35];

        if (isTimex)
            ; // FIXME: [36] last OUT to 0xff

        state.portFFFD = buffer[38];
        state.port1FFD = (length == 55 ? buffer[86] : 0);

        size_t off = 32 + length;
        while (off < buffer.size()) {
            length = buffer[off] | (buffer[off + 1] << 8);
            uint8_t bank = buffer[off + 2];
            off += 3;

            if (!is48k) {
                if (bank < 3 || bank > 10) {
                    off += length;
                    continue;
                }
                bank -= 3;
            } else {
                switch (bank) {
                    case 4: bank = 2; break;
                    case 5: bank = 0; break;
                    case 8: bank = 5; break;
                    default: off += length; continue;
                }
            }

            if (length == 0xffff) {
                buffer.readAt(off, &memory[bank * Z80Memory::BankSize], Z80Memory::BankSize);
                off += Z80Memory::BankSize;
            } else {
                size_t addr = 0xc000;
                while (length > 0) {
                    if (length < 4 || buffer[off] != 0xED || buffer[off + 1] != 0xED)
                        writeByte(memory, bank, addr, buffer[off++]);
                    else {
                        off += 2;
                        size_t count = buffer[off++];
                        uint8_t byte = buffer[off++];
                        for (size_t i = 0; i < count; i++)
                            writeByte(memory, bank, addr, byte);
                    }
                }
            }
        }
    }
}
