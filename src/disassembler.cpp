#include "disassembler.h"
#include <sstream>
#include <iomanip>

// 16bit opcode > readable assembly instruction
std::string disassembleOpcode(uint16_t opcode)
{
    std::stringstream output;

    // Extract X: the second hex digit / register
    uint8_t x = (opcode & 0x0F00) >> 8;

    // Extract Y: the third hex digit / register
    uint8_t y = (opcode & 0x00F0) >> 4;

    // Extract N: the final nibble/4 bits
    uint8_t n = opcode & 0x000F;

    // Extract NN: the final byte/8 bits
    uint8_t nn = opcode & 0x00FF;

    // Extract NNN: the final 12 bits (common memory address)
    uint16_t nnn = opcode & 0x0FFF;

    switch (opcode & 0xF000)
    {
        // 1NNN jump to NNN
        // Ex: 0x1225 -> JP 0x225
        case 0x1000:
            output << "JP 0x" << std::hex << std::uppercase << nnn;
            break;

        // 2NNN call subroutine at NNN
        // Ex: 0x2456 -> CALL 0x456
        case 0x2000:
            output << "CALL 0x" << std::hex << std::uppercase << nnn;
            break;

        // 6XNN load NN into VX
        // Ex: 0x6A15 -> LD VA, 0x15
        case 0x6000:
            output << "LD V" << std::hex << std::uppercase
                   << static_cast<int>(x)
                   << ", 0x"
                   << static_cast<int>(nn);
            break;

        // 7XNN add NN to VX
        // Example: 0x7A01 -> ADD VA, 0x1
        case 0x7000:
            output << "ADD V" << std::hex << std::uppercase
                   << static_cast<int>(x)
                   << ", 0x"
                   << static_cast<int>(nn);
            break;

        // ANNN set I to NNN
        // Example: 0xA300 -> LD I, 0x300
        case 0xA000:
            output << "LD I, 0x" << std::hex << std::uppercase << nnn;
            break;

        // unknown if not implemented yet
        default:
            output << "UNKNOWN";
            break;
    }
    //  stringstream > std::string and return
    return output.str();
}
