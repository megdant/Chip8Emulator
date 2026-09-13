// Notes:
// argv = arg values. argc = arg count. 
// 0 = success. 1 = error.

#include <iostream>
#include "chip8.h"
#include "disassembler.h"
#include <iomanip> // formatting
#include <vector> // will list addresses of opcodes in ROM
// #include <algorithm> // will sort the addresses of opcodes in ROM

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

    std::cout << argv[1] <<" loaded successfully." << std::endl;

    // Test cases for disassembler with INVADERS opcodes
    // std::cout << disassembleOpcode(0x1225) << std::endl;
    // std::cout << disassembleOpcode(0x2456) << std::endl;
    // std::cout << disassembleOpcode(0x6A15) << std::endl;
    // std::cout << disassembleOpcode(0x7A01) << std::endl;
    // std::cout << disassembleOpcode(0xA300) << std::endl;

    // Test cases for 8XYN opcodes
    // std::cout << disassembleOpcode(0x8120) << std::endl;
    // std::cout << disassembleOpcode(0x8121) << std::endl;
    // std::cout << disassembleOpcode(0x8122) << std::endl;
    // std::cout << disassembleOpcode(0x8123) << std::endl;
    // std::cout << disassembleOpcode(0x8124) << std::endl;
    // std::cout << disassembleOpcode(0x8125) << std::endl;
    // std::cout << disassembleOpcode(0x8126) << std::endl;
    // std::cout << disassembleOpcode(0x8127) << std::endl;
    // std::cout << disassembleOpcode(0x812E) << std::endl;

    // Test cases for 0x0000 family opcodes
    // std::cout << disassembleOpcode(0x00E0) << std::endl;
    // std::cout << disassembleOpcode(0x00EE) << std::endl;
    // std::cout << disassembleOpcode(0x0123) << std::endl;

    // Test cases for 0xE000 family opcodes
    // std::cout << disassembleOpcode(0xEA9E) << std::endl;
    // std::cout << disassembleOpcode(0xEAA1) << std::endl;

    // Test cases for 0xF000 family opcodes
    // std::cout << disassembleOpcode(0xF107) << std::endl;
    // std::cout << disassembleOpcode(0xF10A) << std::endl;
    // std::cout << disassembleOpcode(0xF115) << std::endl;
    // std::cout << disassembleOpcode(0xF118) << std::endl;
    // std::cout << disassembleOpcode(0xF11E) << std::endl;
    // std::cout << disassembleOpcode(0xF129) << std::endl;
    // std::cout << disassembleOpcode(0xF133) << std::endl;
    // std::cout << disassembleOpcode(0xF155) << std::endl;
    // std::cout << disassembleOpcode(0xF165) << std::endl;

    // chip8.printMemoryPreview();

    // Test case with the hardcoded INVADERS opcode first
    // uint16_t testOpcode = 0x1225;
    // uint16_t opcode = chip8.fetchOpcode();
    // for (uint16_t address = 0x200; address < 0x220; address += 2)

    // workflow: ROM starts at 0x200, ROM size = number of bytes loaded, ROM end = 0x200 + ROM size
    // uint16_t romEnd = 0x200 + static_cast<uint16_t>(chip8.getROMSize());
    // for (uint16_t address = 0x200; address + 1 < romEnd; address += 2)
    // // address + 1 is error handle for never reading past end of ROM (if odd #)
    // {
    //     uint16_t opcode = chip8.getOpcodeAt(address);

    //     std::cout
    //         << std::hex
    //         << std::uppercase
    //         << std::setfill('0')
    //         << std::setw(4)
    //         << address
    //         << ": "
    //         << std::setw(4)
    //         << opcode
    //         << "    "
    //         << disassembleOpcode(opcode)
    //         << std::endl;
    // }

    // ROM starts at 0x200
    // ROM end = 0x200 + number of bytes loaded
    uint16_t romEnd = 0x200 + static_cast<uint16_t>(chip8.getROMSize());

    // Keeps track of addresses we already checked
    bool visited[4096] = { false };

    // Addresses that still need to be checked
    std::vector<uint16_t> addressesToCheck;

    // CHIP-8 programs begin at 0x200
    addressesToCheck.push_back(0x200);

    while (!addressesToCheck.empty())
    {
        uint16_t address = addressesToCheck.back();
        addressesToCheck.pop_back();

        // Ignore addresses outside the ROM
        if (address < 0x200 || address + 1 >= romEnd)
        {
            continue;
        }

        // Don't process the same address twice
        if (visited[address])
        {
            continue;
        }

        visited[address] = true;

        uint16_t opcode = chip8.getOpcodeAt(address);
        uint16_t nextAddress = address + 2;
        uint16_t targetAddress = opcode & 0x0FFF;

        // RET ends this path
        if (opcode == 0x00EE)
        {
            continue;
        }

        // 1NNN - jump
        if ((opcode & 0xF000) == 0x1000)
        {
            addressesToCheck.push_back(targetAddress);
            continue;
        }

        // 2NNN - call
        if ((opcode & 0xF000) == 0x2000)
        {
            addressesToCheck.push_back(nextAddress);
            addressesToCheck.push_back(targetAddress);
            continue;
        }

        // Instructions that may skip the next instruction
        if ((opcode & 0xF000) == 0x3000 ||
            (opcode & 0xF000) == 0x4000 ||
            ((opcode & 0xF000) == 0x5000 && (opcode & 0x000F) == 0) ||
            ((opcode & 0xF000) == 0x9000 && (opcode & 0x000F) == 0) ||
            ((opcode & 0xF0FF) == 0xE09E) ||
            ((opcode & 0xF0FF) == 0xE0A1))
        {
            addressesToCheck.push_back(nextAddress);
            addressesToCheck.push_back(nextAddress + 2);
            continue;
        }

        // BNNN depends on V0, which we don't know during disassembly
        if ((opcode & 0xF000) == 0xB000)
        {
            continue;
        }

        // Normal instruction
        addressesToCheck.push_back(nextAddress);
    }
    // Print reachable instructions in memory order
    for (uint16_t address = 0x200; address + 1 < romEnd; address++)
    {
        if (visited[address])
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
    }
    // Test case for checking opcode 225 in the INVADERS ROM
    // uint16_t entryOpcode = chip8.getOpcodeAt(0x225);
    // std::cout
    //     << "Opcode at 0x225: "
    //     << std::hex
    //     << std::uppercase
    //     << std::setfill('0')
    //     << std::setw(4)
    //     << entryOpcode
    //     << "    "
    //     << disassembleOpcode(entryOpcode)
    //     << std::endl;
    return 0;
}