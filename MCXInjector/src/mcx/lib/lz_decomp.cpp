#include <cstdint>
#include <cstring>
#include <vector>
#include "mcx/lib/lz_decomp.hpp"

static inline std::vector<uint8_t> stack;

long __cdecl LZDecomp(
    unsigned char* out_buf,       
    unsigned char* in_buf, 
    unsigned long  compressed_size)
{
    unsigned char* out_ptr = out_buf;
    unsigned char* in_ptr = in_buf;

    LZSrcBufEnd = in_buf + (compressed_size - 3);
    LZOldChain = nullptr;
    LZChain = nullptr;
    LZCodeMask = 0x1ff;
    LZMaxIndex = 0x200;
    LZFreeIndex = 0x102;
    LZOldSuffix = '\0';

    uint8_t bit_pos = 0; // bit position (0-7)
    uint8_t bits_needed = 9; // bits to read

    while (in_ptr <= LZSrcBufEnd) {
        // Read next code
        uint32_t input_dword = *(uint32_t*)in_ptr;
        uint32_t code = input_dword >> bit_pos;
        code &= LZCodeMask;

        uint8_t temp_cl = bit_pos + bits_needed;
        uint32_t bytes_to_advance = temp_cl >> 3;
        bit_pos = temp_cl & 0x7;
        in_ptr += bytes_to_advance;

        if (code == END_CODE) break;

        if (code == CLEAR_CODE) {
            bits_needed = 9;
            LZCodeMask = 0x1ff;
            LZMaxIndex = 0x200;
            LZFreeIndex = 0x102;

            if (in_ptr > LZSrcBufEnd) break;

            // Read next 9-bit code
            input_dword = *(uint32_t*)in_ptr;
            code = input_dword >> bit_pos;
            temp_cl = bit_pos + 9;
            bytes_to_advance = temp_cl >> 3;
            bit_pos = temp_cl & 0x7;
            code &= 0x1ff;
            in_ptr += bytes_to_advance;

            LZOldChain = (HashStruct*)(uintptr_t)code;
            LZOldSuffix = (uint8_t)code;
            *out_ptr++ = LZOldSuffix;
            continue;
        }

        // Normal code handling
        uint32_t curr_code = code;
        LZChain = (HashStruct*)(uintptr_t)code;
        HashStruct* entry = &LZHashBuffer[curr_code];

        // Stack for reversing string
        // tries to simulate the reversing on the stack (ESP) that was going on in the binary.
        //std::vector<uint8_t> stack;
        stack.clear();

        // LZ KwKwK case
        if (curr_code >= LZFreeIndex) {
            // Push old suffix onto stack
            stack.push_back(LZOldSuffix);

            // Add entry at this index
            LZHashBuffer[curr_code].suffix = LZOldSuffix;
            LZHashBuffer[curr_code].prefix = (uint16_t)(uintptr_t)LZOldChain;

            // Walk from OldChain
            curr_code = (uint32_t)(uintptr_t)LZOldChain;
            entry = &LZHashBuffer[curr_code];
        }

        // Walk chain and push bytes onto stack
        while (curr_code >= 256) {  // while (AH != 0) in assembly?
            stack.push_back(entry->suffix);
            curr_code = entry->prefix;
            entry = &LZHashBuffer[curr_code];
        }

        // Push final literal
        LZOldSuffix = (uint8_t)curr_code;
        stack.push_back(LZOldSuffix);

        // Pop stack and output (actually reverses the string)
        for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
            *out_ptr++ = *it;
        }

        // Add new dictionary entry
        LZHashBuffer[LZFreeIndex].suffix = LZOldSuffix;
        LZHashBuffer[LZFreeIndex].prefix = (uint16_t)(uintptr_t)LZOldChain;
        LZFreeIndex++;

        LZOldChain = LZChain;

        // Check if we need to grow the bit width
        if (LZFreeIndex >= LZMaxIndex) {
            if (bits_needed != 0xc) {
                bits_needed++;
                LZMaxIndex <<= 1;
                LZCodeMask = (LZCodeMask << 1) | 1;
            }
        }
    }
    
    // return uncompressed (output) bytes
    return (long)(out_ptr - out_buf);
}