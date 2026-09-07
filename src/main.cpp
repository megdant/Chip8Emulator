// Notes:
// argv = arg values. argc = arg count. 
// 0 = success. 1 = error.

#include <iostream>
#include "chip8.h"
#include "disassembler.h"
// formatting
#include <iomanip>

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
    // uint16_t opcode = chip8.fetchOpcode();
    // for (uint16_t address = 0x200; address < 0x220; address += 2)

    // workflow: ROM starts at 0x200, ROM size = number of bytes loaded, ROM end = 0x200 + ROM size
    uint16_t romEnd = 0x200 + static_cast<uint16_t>(chip8.getROMSize());
    for (uint16_t address = 0x200; address + 1 < romEnd; address += 2)
    // address + 1 is error handle for never reading past end of ROM (if odd #)
    {
        uint16_t opcode = chip8.getOpcodeAt(address);

        std::cout
            << std::hex
            << std::uppercase
            << std::setfill('0')
            << std::setw(4)
            << address
            << ": "
            << std::setw(4)
            << opcode
            << "    "
            << disassembleOpcode(opcode)
            << std::endl;
    }
    uint16_t entryOpcode = chip8.getOpcodeAt(0x225);

    std::cout
        << "Opcode at 0x225: "
        << std::hex
        << std::uppercase
        << std::setfill('0')
        << std::setw(4)
        << entryOpcode
        << "    "
        << disassembleOpcode(entryOpcode)
        << std::endl;
    return 0;
}