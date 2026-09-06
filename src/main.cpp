// Notes:
// arcv = arg values. argc = arg count. 
// 0 = success. 1 = error.


#include <iostream>
#include "chip8.h"

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

    return 0;
}