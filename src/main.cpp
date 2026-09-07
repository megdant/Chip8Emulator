// Notes:
// arcv = arg values. argc = arg count. 
// 0 = success. 1 = error.


#include <iostream>
#include "chip8.h"
#include "disassembler.h"

// Compiler Test:
// int main()
// {
//     std::cout << "CHIP8 Emulator test" << std::endl;

//     return 0;
// }

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: CHIP8 <ROM file>" << std::endl;
        return 1;
    }

    std::cout << "ROM: " << argv[1] << std::endl;

    Chip8 chip8;
    
    if (!chip8.loadROM(argv[1]))
    {
        std::cout << argv[1] << " loaded unsuccessfully." << std::endl;
        return 1;
    }

    std::cout << "ROM loaded successfully." << std::endl;
    // Test cases for disassembler with INVADERS opcodes
    // std::cout << disassembleOpcode(0x1225) << std::endl;
    // std::cout << disassembleOpcode(0x2456) << std::endl;
    // std::cout << disassembleOpcode(0x6A15) << std::endl;
    // std::cout << disassembleOpcode(0x7A01) << std::endl;
    // std::cout << disassembleOpcode(0xA300) << std::endl;

    chip8.printMemoryPreview();

    // Test case with the hardcoded INVADERS opcode first
    // uint16_t testOpcode = 0x1225;
    uint16_t opcode = chip8.fetchOpcode();
    std::cout
        // << "0x1225 ->"
        << "First instruction is: "
        << disassembleOpcode(opcode)
        << std::endl;

    return 0;
}