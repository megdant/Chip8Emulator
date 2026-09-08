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

        case 0x0000:
            if (opcode == 0x00E0)
            {
                // 00E0 clear the display
                // Ex: 0x00E0 -> CLS
                output << "CLS";
            }
            else if (opcode == 0x00EE)
            {
                // 00EE return from subroutine
                // Ex: 0x00EE -> RET
                output << "RET";
            }
            else
            {
                // 0NNN call machine code routine at NNN
                // Ex: 0x0123 -> SYS 0x123
                output << "SYS 0x"
                    << std::hex << std::uppercase
                    << nnn;
            }
            break;

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

        // 8XYN register
        case 0x8000:
            switch (n)
            {
                // 8XY0 set VX = VY
                // Ex: 0x8120 -> LD V1, V2
                case 0x0:
                    output << "LD V"
                        << std::hex << std::uppercase
                        << static_cast<int>(x)
                        << ", V"
                        << static_cast<int>(y);
                    break;

                // 8XY1 set VX = VX OR VY
                // Ex: 0x8121 -> OR V1, V2
                case 0x1:
                    output << "OR V"
                        << std::hex << std::uppercase
                        << static_cast<int>(x)
                        << ", V"
                        << static_cast<int>(y);
                    break;

                // 8XY2 set VX = VX AND VY
                // Ex: 0x8122 -> AND V1, V2
                case 0x2:
                    output << "AND V"
                        << std::hex << std::uppercase
                        << static_cast<int>(x)
                        << ", V"
                        << static_cast<int>(y);
                    break;

                // 8XY3 set VX = VX XOR VY
                // Ex: 0x8123 -> XOR V1, V2
                case 0x3:
                    output << "XOR V"
                        << std::hex << std::uppercase
                        << static_cast<int>(x)
                        << ", V"
                        << static_cast<int>(y);
                    break;

                // 8XY4 add VY to VX
                // Ex: 0x8124 -> ADD V1, V2
                case 0x4:
                    output << "ADD V"
                        << std::hex << std::uppercase
                        << static_cast<int>(x)
                        << ", V"
                        << static_cast<int>(y);
                    break;

                // 8XY5 subtract VY from VX
                // Ex: 0x8125 -> SUB V1, V2
                case 0x5:
                    output << "SUB V"
                        << std::hex << std::uppercase
                        << static_cast<int>(x)
                        << ", V"
                        << static_cast<int>(y);
                    break;

                // 8XY6 shift VX right by 1
                // Ex: 0x8126 -> SHR V1
                case 0x6:
                    output << "SHR V"
                        << std::hex << std::uppercase
                        << static_cast<int>(x);
                    break;

                // 8XY7 set VX = VY - VX
                // Ex: 0x8127 -> SUBN V1, V2
                case 0x7:
                    output << "SUBN V"
                        << std::hex << std::uppercase
                        << static_cast<int>(x)
                        << ", V"
                        << static_cast<int>(y);
                    break;

                // 8XYE shift VX left by 1
                // Ex: 0x812E -> SHL V1
                case 0xE:
                    output << "SHL V"
                        << std::hex << std::uppercase
                        << static_cast<int>(x);
                    break;

                default:
                    output << "UNKNOWN";
                    break;
            }
            break;

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

        // CXNN generate random byte, AND it with NN, store result in VX
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

        // EX9E skip next instruction if key stored in VX is pressed
        // EXA1 skip next instruction if key stored in VX is not pressed
        case 0xE000:
            if (nn == 0x9E)
            {
                output << "SKP V"
                    << std::hex << std::uppercase
                    << static_cast<int>(x);
            }
            else if (nn == 0xA1)
            {
                output << "SKNP V"
                    << std::hex << std::uppercase
                    << static_cast<int>(x);
            }
            else
            {
                output << "UNKNOWN";
            }
            break;

        // 0xF000 family
        case 0xF000:
            if (nn == 0x07)
            {
                output << "LD V"
                    << std::hex << std::uppercase
                    << static_cast<int>(x)
                    << ", DT";
            }
            else if (nn == 0x0A)
            {
                output << "LD V"
                    << std::hex << std::uppercase
                    << static_cast<int>(x)
                    << ", K";
            }
            else if (nn == 0x15)
            {
                output << "LD DT, V"
                    << std::hex << std::uppercase
                    << static_cast<int>(x);
            }
            else if (nn == 0x18)
            {
                output << "LD ST, V"
                    << std::hex << std::uppercase
                    << static_cast<int>(x);
            }
            else if (nn == 0x1E)
            {
                output << "ADD I, V"
                    << std::hex << std::uppercase
                    << static_cast<int>(x);
            }
            else if (nn == 0x29)
            {
                output << "LD F, V"
                    << std::hex << std::uppercase
                    << static_cast<int>(x);
            }
            else if (nn == 0x33)
            {
                output << "LD B, V"
                    << std::hex << std::uppercase
                    << static_cast<int>(x);
            }
            else if (nn == 0x55)
            {
                output << "LD [I], V"
                    << std::hex << std::uppercase
                    << static_cast<int>(x);
            }
            else if (nn == 0x65)
            {
                output << "LD V"
                    << std::hex << std::uppercase
                    << static_cast<int>(x)
                    << ", [I]";
            }
            else
            {
                output << "UNKNOWN";
            }
            break;

        // unknown if not implemented yet
        default:
            output << "UNKNOWN";
            break;
    }
    //  stringstream > std::string and return
    return output.str();
}
