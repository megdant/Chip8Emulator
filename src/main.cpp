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

    // CPU Emulation Test Cases
    std::cout << std::endl;
    std::cout << "CPU Test" << std::endl;

    // 600A LD V0, 0x0A
    chip8.executeOpcode(0x600A);

    std::cout
        << "After 600A, V0 = 0x"
        << std::hex
        << std::uppercase
        << static_cast<int>(chip8.getRegister(0))
        << std::endl;

    // 7005 ADD V0, 0x05
    chip8.executeOpcode(0x7005);

    std::cout
        << "After 7005, V0 = 0x"
        << static_cast<int>(chip8.getRegister(0))
        << std::endl;

    // A300 LD I, 0x300
    chip8.executeOpcode(0xA300);

    std::cout
        << "After A300, I = 0x"
        << chip8.getI()
        << std::endl;

    // 1250 JP 0x250
    chip8.executeOpcode(0x1250);

    std::cout
        << "After 1250, PC = 0x"
        << chip8.getPC()
        << std::endl;
    

    // CALL and RET test
    std::cout << std::endl;
    std::cout << "CALL/RET Test" << std::endl;

    // Set PC to 0x250
    chip8.executeOpcode(0x1250);

    std::cout
        << "PC before CALL = 0x"
        << chip8.getPC()
        << std::endl;

    // Set PC to the return address
    chip8.executeOpcode(0x1252);

    // 2387 CALL subroutine at 0x387
    chip8.executeOpcode(0x2387);

    std::cout
        << "PC after CALL = 0x"
        << chip8.getPC()
        << std::endl;

    // 00EE RET
    chip8.executeOpcode(0x00EE);

    std::cout
        << "PC after RET = 0x"
        << chip8.getPC()
        << std::endl;

    // ********************************** Conditional skip tests **********************************
    std::cout << std::endl;
    std::cout << "Conditional Skip Test" << std::endl;
    // Set V0 = 0x0A
    chip8.executeOpcode(0x600A);
    // Set PC = 0x300
    chip8.executeOpcode(0x1300);
    // 300A SE V0, 0x0A
    // V0 equals 0x0A, so PC should skip ahead by 2
    chip8.executeOpcode(0x300A);

    std::cout
        << "After 300A (equal), PC = 0x"
        << chip8.getPC()
        << std::endl;

    // Set PC back to 0x300
    chip8.executeOpcode(0x1300);

    // 3005 SE V0, 0x05
    // V0 does not equal 0x05, so PC should not change!!
    chip8.executeOpcode(0x3005);

    std::cout
        << "After 3005 (not equal), PC = 0x"
        << chip8.getPC()
        << std::endl;

    // 4005 SNE V0, 0x05
    // V0 does not equal 0x05, so PC should skip ahead by 2
    chip8.executeOpcode(0x4005);

    std::cout
        << "After 4005 (not equal), PC = 0x"
        << chip8.getPC()
        << std::endl;
    
    // Set V1 = 0x0A and V2 = 0x0A
    chip8.executeOpcode(0x610A);
    chip8.executeOpcode(0x620A);

    // Set PC = 0x300
    chip8.executeOpcode(0x1300);

    // 5120 SE V1, V2
    // V1 equals V2, so PC should skip ahead by 2
    chip8.executeOpcode(0x5120);

    std::cout
        << "After 5120 (equal), PC = 0x"
        << chip8.getPC()
        << std::endl;

    // Change V2 = 0x05
    chip8.executeOpcode(0x6205);

    // Set PC back to 0x300
    chip8.executeOpcode(0x1300);

    // 9120 SNE V1, V2
    // V1 does not equal V2, so PC should skip ahead by 2
    chip8.executeOpcode(0x9120);

    std::cout
        << "After 9120 (not equal), PC = 0x"
        << chip8.getPC()
        << std::endl;

    // ********************************** Test ends **********************************

    // CPU cycle test with the loaded ROM
    Chip8 cycleTest;

    if (cycleTest.loadROM(argv[1]))
    {
        std::cout << std::endl;
        std::cout << "CPU Cycle Test" << std::endl;

        std::cout
            << "PC before cycle = 0x"
            << std::hex
            << std::uppercase
            << cycleTest.getPC()
            << std::endl;

        cycleTest.cycle();

        std::cout
            << "PC after cycle = 0x"
            << cycleTest.getPC()
            << std::endl;
    }
    return 0;
}