#pragma once
#include <cstdint>
#include <cstddef>
#include "mcx_inject.hpp"
#include "mcx/lib/fit_ini_file.hpp"

#ifndef ENABLE_LOG_COLOR
#define ENABLE_LOG_COLOR 0
#endif

#ifndef NO_ERR
#define NO_ERR	0
#endif

// TODO
static constexpr auto ERR_FAILED_PALETTE_ALLOC = 0xABDA0001; // 0xABDA0001
static constexpr auto ERR_FAILED_PALETTE2_ALLOC = 0xABDA0002; // 0xABDA0002
static constexpr auto ERR_FAILED_BW_PALETTE_ALLOC = 0xABDA0003; // 0xABDA0003
static constexpr auto ERR_HAZE_DEPTH_TABLE_BAD_FILE = 0xABDA0004; // 0xABDA0004
static constexpr auto ERR_HAZE_DEPTH_TABLE_ALLOC = 0xABDA0005; // 0xABDA0005
static constexpr auto ERR_FADE_PALETTE_ALLOC = 0xABDA0006; // 0xABDA0006
static constexpr auto ERR_FAILED_EX_PALETTE_ALLOC = 0xABDA0007; // 0xABDA0007

class PaletteBlock
{
public:
	short length;      // 0x0
	short field_0x02;  // 0x02
	uint8_t* rgb_data; // 0x04
public:
	//  public: void __thiscall PaletteBlock::initRgbData(unsigned char *)
	void initRgbData(unsigned char* data);
};

class Palette;

class ColorRangeData
{
public:
    uint8_t base_color_idx;
    uint8_t max_lit_color;
    uint8_t max_hazed_color;
    uint8_t base_red;
    uint8_t base_green;
    uint8_t base_blue;
    uint8_t light_source_flag;
    uint8_t depth_cue_flag;
    uint8_t calc_light_source;
    uint8_t calc_depth_cue;
};

class ColorRange 
{
public:
    Palette* palette; // 0x0
    uint32_t light_source_flag; //0x4;
    uint32_t depth_cue_flag; //0x8;
    uint32_t calc_light_source; //0xc;
    uint32_t calc_depth_cue; //0x10;
    uint32_t base_color_idx; //0x14;
    uint8_t base_red; //0x18;
    uint8_t base_green; //0x19;
    uint8_t	base_blue; //0x1a;
    uint8_t padding; //0x1b;
    uint32_t max_lit_color; //0x1c;
    uint32_t max_hazed_color; //0x20;
public:
   ColorRange(ColorRangeData* data, Palette* palette);
};

static constexpr size_t PAL_FILENAME_LEN = 8;

class Palette
{
public:
	short length;      // 0x0
	short field_0x02;  // 0x02
	uint8_t* rgb_data_; // 0x04
    int32_t  palette_data_size; // 0x08
    uint8_t* bw_palette_data; // 0x0c
    uint8_t* data_buf; // 0x10
    uint8_t* ex_palette_data; // 0x14
    int32_t  field_0x18; // 0x18
    int32_t  field_0x1c; // 0x1c
    int32_t  field_0x20; // 0x20
    uint8_t  fade_r; // 0x24
    uint8_t  fade_g; // 0x25
    uint8_t  fade_b; // 0x26
    uint8_t  padding_0x27; // 0x24
    uint8_t* depth_haze_table_data; // 0x28
    int32_t  num_depth_haze_tables; // 0x2c
    int32_t*  curr_depth_haze_table; // 0x30
    int32_t  depth_haze_table_size; // 0x34
    int32_t  depth_haze_table_entry_shift; // 0x38
    int32_t  bitmap_haze_scale; // 0x3c
    char palette_filename[PAL_FILENAME_LEN]; // 0x40
    char ex_palette_filename[PAL_FILENAME_LEN]; // 0x48
    char bw_palette_filename[PAL_FILENAME_LEN]; // 0x50
    char depth_table_filename[PAL_FILENAME_LEN]; // 0x58
    char fade_table_filename[PAL_FILENAME_LEN]; // 0x60
    char all_fade_table_filename[PAL_FILENAME_LEN]; // 0x68
    uint8_t* palette_data; // 0x70
    int32_t  init_flag; // 0x74
    int32_t  bitmap_haze_levels_copy; // 0x78
    int32_t  num_bitmap_haze_levels; // 0x7c
    int32_t  num_color_ranges; // 0x80
    int32_t  ex_palette_size; // 0x84
    ColorRange* color_ranges; // 0x88 // num_color_ranges * 36
    int32_t  all_fade_palettes_size; // 0x8c
    int32_t  fade_palettes_size; // 0x90
    uint8_t* fade_palettes_data; // 0x94
    int32_t*  all_fade_palettes_data; // 0x98
    int32_t  haze_shift; // 0x9c // Log2 of haze_table_size. i.e shift=8 results in 256 entries
    int32_t  haze_table_size; // 0xa0 // size = 2 ^ haze_shift
    int32_t  haze_table_half_size; // 0xa4 // Midpoint of haze table
    int32_t  curr_min_haze; // 0xa8
    int32_t  curr_max_haze; // 0xac
    int32_t  altitude_shift; // 0xb0 // Log2 multiplier for altitude (Multiplier for depth tables at different altitudes)
    int32_t  total_depth_tables; // 0xb4 // num_tables << altitude_shift
    int32_t  field_0xb8; // 0xb8
public:
    // public: void __thiscall Palette::init(void)
    void init();

    // public: long __thiscall Palette::init(char *)
    long init2(char* palette_name);

    // public: long __thiscall Palette::init(class FitIniFile &)
    long init3(FitIniFile* ini_file);

    // protected: long __thiscall Palette::loadBWPalette(class FitIniFile &)
    long loadBWPalette(FitIniFile* ini_file);

    // protected: long __thiscall Palette::loadExtractPalette(class FitIniFile &)
    long loadExtractPalette(FitIniFile* ini_file);

    // protected long __thiscall Palette::loadColorRanges(FitIniFile &)
    long loadColorRanges(FitIniFile* ini_file);

    // protected: long __thiscall Palette::loadPalette(class FitIniFile &)
    long loadPalette(FitIniFile* ini_file);

    // protected: long __thiscall Palette::loadPaletteInfo(class FitIniFile &)
    long loadPaletteInfo(FitIniFile* ini_file);

    // public: long __thiscall Palette::loadTables(void)
    long loadTables();

    // protected: long __thiscall Palette::loadFadePalettes(class File &)
    long loadFadePalettes(File* file);

    // protected: long __thiscall Palette::loadAllFadePalettes(class File &)
    long loadAllFadePalettes(File* file);

    // protected: long __thiscall Palette::loadDepthHazeTables(class File &)
    long loadDepthHazeTables(File* file);

    // protected: void __thiscall Palette::recalculateDepthVsHazeInfo(long)
    void recalculateDepthVsHazeInfo(long param_1);
    
    // void __thiscall Palette::activate(long,long)
    void activate(long param_1, long param_2);
public:
    static const char* s_HazeShift;
    static const char* s_NumBitmapHazeLevels;
    static const char* s_NumDepthHazeEntriesShift;
    static const char* s_NumDepthAtHazeLevelTables;
    static const char* s_AltitudeShift;
    static const char* s_Ranges;
    static const char* s_NumColorRanges;
    static const char* s_PaletteFileName;
    static const char* s_BWPaletteFileName;
    static const char* s_ExPaletteFileName;
    static const char* s_BaseColorIndex;
    static const char* s_MaxLitColor;
    static const char* s_MaxHazedColor;
    static const char* s_BaseRed;
    static const char* s_BaseGreen;
    static const char* s_BaseBlue;
    static const char* s_LightSourceFlag;
    static const char* s_DepthCueFlag;
    static const char* s_CalcLightSource;
    static const char* s_CalcDepthCue;
    static const char* s_Tables;
    static const char* s_FadeTableFile;
    static const char* s_DepthTableFile;
    static const char* s_AllFadeTableFile;
    static const char* s_tbl;
    static const char* s_pal;
    static const char* s_fit;
};

#pragma region VAs
VADDR(va_PaletteBlock_initRgbData, 0x006b2180);

VADDR(va_Palette_init, 0x006b2b90);
VADDR(va_Palette_init2, 0x006b2bc0);
VADDR(va_Palette_init3, 0x006b2c70);

VADDR(va_gamePalette, 0x007f09a8);
VADDR(va_lastMinDepth, 0x00794134);
PTR_REF(Palette, g_gamePalette, va_gamePalette);
OBJ_REF(long, g_lastMinDepth, va_lastMinDepth);
#pragma endregion

#pragma region Layout asserts
ASSERT_SIZE(Palette, 188);

ASSERT_FIELD_OFFSET(Palette, palette_filename, 0x40);
ASSERT_FIELD_OFFSET(Palette, ex_palette_filename, 0x48);
ASSERT_FIELD_OFFSET(Palette, bw_palette_filename, 0x50);
ASSERT_FIELD_OFFSET(Palette, depth_table_filename, 0x58);
ASSERT_FIELD_OFFSET(Palette, fade_table_filename, 0x60);
ASSERT_FIELD_OFFSET(Palette, all_fade_table_filename, 0x68);
#pragma endregion