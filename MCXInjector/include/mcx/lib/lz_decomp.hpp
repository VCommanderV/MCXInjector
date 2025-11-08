#include <cstddef>
#include <cstdint>

// Buffer of size LZPacketBufferSize or packet uncompressed size if larger.
static unsigned char* g_LZPacketBuffer;

// 512 KB buffer
static constexpr unsigned long LZPacketBufferSize = 0x7D000;

// Dictionary entry: stores a "chain" (prefix code and suffix character)
#pragma pack(push, 1)
struct HashStruct {
	unsigned short prefix;  // 2-byte: previous code in the chain
	unsigned char suffix;   // 1-byte: character to append
};
#pragma pack(pop)

// Special codes
const unsigned long CLEAR_CODE = 0x100;  // Reset dictionary
const unsigned long END_CODE = 0x101;    // End of data

static unsigned char* LZSrcBufEnd;
static HashStruct* LZOldChain;
static HashStruct* LZChain;
static unsigned long LZCodeMask = 0x1ff;
static unsigned long LZMaxIndex = 0x200;
static unsigned long LZFreeIndex = 0x102;
static char LZOldSuffix = '\0';

static HashStruct LZHashBuffer[4096];

// long __cdecl LZDecomp(unsigned char *,unsigned char *,unsigned long)
long __cdecl LZDecomp(
	unsigned char* compressed_buf,
	unsigned char* uncompressed_buf,
	unsigned long uncompressed_size);

static constexpr uintptr_t va_LZDecomp = 0x0064cc80;
