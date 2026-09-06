// This is the implementation

#include "chip8.h"

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
    pc = 0x200;
}