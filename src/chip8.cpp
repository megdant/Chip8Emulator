// This is the implementation

#include "chip8.h"
// file stream support
#include <fstream>
// adding for debugging purposes (memory preview)
#include <iostream>
#include <iomanip>

// the constructor, runs each time there is a new Chip8 object created
Chip8::Chip8()
{
    for (int i = 0; i < 4096; i++)
    {
        memory[i] = 0;
    }

    for (int i = 0; i < 16; i++)
    {
        V[i] = 0;
    }

    I = 0;
    // the chip8 program will begin here at 0x200
    pc = 0x200;

    // Memory      = cleared
    // V0-VF       = 0
    // I register  = 0
    // PC          = 0x200
}

// loading the ROM into memory
bool Chip8::loadROM(const char* filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file)
    {
        return false;
    }

    file.seekg(0, std::ios::end);
    std::streamsize size = file.tellg();
    file.seekg(0, std::ios::beg);

    if (size > 4096 - 0x200)
    {
        return false;
    }

    // take bytes from the ROM
    // copy the bytes into chip8 memory starting at 0x200
    file.read(
        reinterpret_cast<char*>(&memory[0x200]),
        size
    );

    if (!file)
    {
        return false;
    }

    return true;
}

// debug check to see a preview of the memory
void Chip8::printMemoryPreview() const
{
    for (int i = 0x200; i < 0x210; i++)
    {
        std::cout
            << "0x"
            << std::hex
            << std::uppercase
            << i
            << ": 0x"
            << std::setw(2)
            << std::setfill('0')
            // memory[i] is a unit_8, using static_cast to print it as an int
            << static_cast<int>(memory[i])
            << std::endl;
    }
}

// will fetch opcode from memory at the current pc
uint16_t Chip8::fetchOpcode() const
{
    uint16_t opcode = (memory[pc] << 8) | memory[pc + 1];

    return opcode;
}