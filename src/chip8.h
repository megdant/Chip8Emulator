// This is the declaration

// prevents the header from being included multiple times
#pragma once
// fixed-width integer types like uint8_t
#include <cstdint>

class Chip8
{
public:
    Chip8();
    bool loadROM(const char* filename);
    void printMemoryPreview() const;
private:
    // defining the virtual CHIP8 machine
    uint8_t memory[4096];
    uint8_t V[16];
    uint16_t I;
    uint16_t pc;
};