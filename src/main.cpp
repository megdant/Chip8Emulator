// 0 = success. 1 = error.
#include <iostream> // for input/output
#include "chip8.h"
#include "disassembler.h"
#include <iomanip> // formatting
#include <vector> // addresses to check
#include <string> // for strings

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

    // chip8.printMemoryPreview();

    // romEnd = 0x200 + number of bytes loaded
    uint16_t romEnd = 0x200 + static_cast<uint16_t>(chip8.getROMSize());

    // Track visited addresses and jumps/calls
    bool visited[4096] = { false };
    bool label[4096] = { false };
    
    // Addresses to be checked
    std::vector<uint16_t> addressesToCheck;
    addressesToCheck.push_back(0x200);
    label[0x200] = true;

    while (!addressesToCheck.empty())
    {
        uint16_t address = addressesToCheck.back();
        addressesToCheck.pop_back();

        // Ignore addresses outside the ROM
        if (address < 0x200 || address + 1 >= romEnd)
        {
            continue;
        }

        // If visited already, continue
        if (visited[address])
        {
            continue;
        }
        
        // Mark the address as visited
        visited[address] = true;
        // Get opcode at the current address
        uint16_t opcode = chip8.getOpcodeAt(address);
        // Address of next instruction (2 bytes)
        uint16_t nextAddress = address + 2;
        // Extract bottom 12 bits of opcode for jump/call instructions
        uint16_t targetAddress = opcode & 0x0FFF;

        // RET ends this path
        if (opcode == 0x00EE)
        {
            continue;
        }

        // 1NNN - jump
        if ((opcode & 0xF000) == 0x1000)
        {
            label[targetAddress] = true;
            addressesToCheck.push_back(targetAddress);
            continue;
        }

        // 2NNN - call
        if ((opcode & 0xF000) == 0x2000)
        {
            label[targetAddress] = true;
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

        // Normal, continue to next address
        addressesToCheck.push_back(nextAddress);
    }
    
    std::cout << std::endl;
    std::cout << std::string(50, '*') << std::endl;
    std::cout << "; CHIP8 Disassembler" << std::endl;
    std::cout << "; ROM: " << argv[1] << std::endl;
    std::cout << "; ROM Size: " << chip8.getROMSize() << " Bytes" << std::endl;
    std::cout << std::string(50, '*') << std::endl;
    std::cout << std::endl;

    // Print reachable instructions in memory order
    for (uint16_t address = 0x200; address + 1 < romEnd; address++)
    {
        if (visited[address])
        {
            if (label[address])
            {
                std::cout
                    << "L"
                    << std::hex
                    << std::uppercase
                    << address
                    << ":"
                    << std::endl;
            }
                uint16_t opcode = chip8.getOpcodeAt(address);
                std::cout
                    << "    "
                    << std::left
                    << std::setfill(' ')
                    << std::setw(28)
                    << disassembleOpcode(opcode)
                    << "; "
                    << std::right
                    << std::hex
                    << std::uppercase
                    << std::setfill('0')
                    << std::setw(4)
                    << opcode
                    << std::endl;
        }
    }
    return 0;
}