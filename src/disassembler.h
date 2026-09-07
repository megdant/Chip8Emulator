#pragma once

#include <cstdint>
#include <string>

// Disassembles a CHIP8 opcode into a human readable string
std::string disassembleOpcode(uint16_t opcode);