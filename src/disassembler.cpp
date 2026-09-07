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

        // TODO add 0NNN and 00E0, 00EE cases
        // 1NNN jump to NNN

        // keep in order!

        // Ex: 0x1225 -> JP 0x225
        case 0x1000:
            output << "JP 0x" << std::hex << std::uppercase << nnn;
            break;

        // 2NNN call subroutine at NNN
        // Ex: 0x2456 -> CALL 0x456
        case 0x2000:
            output << "CALL 0x" << std::hex << std::uppercase << nnn;
            break;

        // 3XNN skip next instruction if VX == NN
        // Ex: 0x3A10 -> SE VA, 0x10
        case 0x3000:
            output << "SE V"
                << std::hex << std::uppercase
                << static_cast<int>(x)
                << ", 0x"
                << static_cast<int>(nn);
            break;
        
        // 4XNN skip next instruction if VX != NN
        // Ex: 0x4A10 -> SNE VA, 0x10
        case 0x4000:
            output << "SNE V"
                << std::hex << std::uppercase
                << static_cast<int>(x)
                << ", 0x"
                << static_cast<int>(nn);
            break;

        // 5XY0 skip next instruction if VX == VY
        // Ex: 0x5AB0 -> SE VA, VB
        case 0x5000:
            if (n == 0)
            {
                output << "SE V"
                    << std::hex << std::uppercase
                    << static_cast<int>(x)
                    << ", V"
                    << static_cast<int>(y);
            }
            else
            {
                output << "UNKNOWN";
            }
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
        // Ex: 0x7A01 -> ADD VA, 0x1
        case 0x7000:
            output << "ADD V" << std::hex << std::uppercase
                << static_cast<int>(x)
                << ", 0x"
                << static_cast<int>(nn);
            break;

        // TODO add 0x8000

        // 9XY0 skip next instruction if VX != VY
        // Ex: 0x9AB0 -> SNE VA, VB
        case 0x9000:
            if (n == 0)
            {
                output << "SNE V"
                    << std::hex << std::uppercase
                    << static_cast<int>(x)
                    << ", V"
                    << static_cast<int>(y);
            }
            else
            {
                output << "UNKNOWN";
            }
            break;

        // ANNN set I to NNN
        // Ex: 0xA300 -> LD I, 0x300
        case 0xA000:
            output << "LD I, 0x" << std::hex << std::uppercase << nnn;
            break;

        // BNNN jump to address NNN + V0
        // Ex: 0xB300 -> JP V0, 0x300
        case 0xB000:
            output << "JP V0, 0x"
                << std::hex << std::uppercase
                << nnn;
            break;

        // CXNN random byte AND NN, store in VX
        // Ex: 0xCA0F -> RND VA, 0x0F
        case 0xC000:
            output << "RND V"
                << std::hex << std::uppercase
                << static_cast<int>(x)
                << ", 0x"
                << static_cast<int>(nn);
            break;

        // DXYN draw N-byte sprite at (VX, VY)
        // Ex: 0xDAB5 -> DRW VA, VB, 0x5
        case 0xD000:
            output << "DRW V"
                << std::hex << std::uppercase
                << static_cast<int>(x)
                << ", V"
                << static_cast<int>(y)
                << ", 0x"
                << static_cast<int>(n);
            break;

        // TODO add 0xE000 and 0xF000

        // unknown if not implemented yet
        default:
            output << "UNKNOWN";
            break;
    }
    //  stringstream > std::string and return
    return output.str();
}
