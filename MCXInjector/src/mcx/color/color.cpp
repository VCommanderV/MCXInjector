#include "mcx_inject.hpp"
#include "mcx/lib/heap.hpp"
#include "mcx/lib/a_globs.hpp"
#include "mcx/lib/c_ident.hpp"
#include "mcx/lib/file.hpp"
#include "mcx/lib/fit_ini_file.hpp"
#include "mcx/color/color.hpp"

# pragma region PaletteBlock
void PaletteBlock::initRgbData(unsigned char* data)
{
	this->length = *(short*)(data + 2);
	this->field_0x02 = *(short*)(data);

	size_t bytes_size = this->length * 3;

	if (this->rgb_data == nullptr && g_systemHeap != nullptr && g_systemHeap->heap_size != 0)
	{
		this->rgb_data = reinterpret_cast<uint8_t*>(g_systemHeap->malloc(bytes_size));
	}

	if (this->rgb_data != nullptr)
	{
		memcpy(this->rgb_data, data + 4, bytes_size);
	}

	return;
}
#pragma endregion

#pragma region Palette
const char* Palette::s_HazeShift = "HazeShift";
const char* Palette::s_NumBitmapHazeLevels = "NumBitmapHazeLevels";
const char* Palette::s_NumDepthHazeEntriesShift = "NumDepthHazeEntriesShift";
const char* Palette::s_NumDepthAtHazeLevelTables = "NumDepthAtHazeLevelTables";
const char* Palette::s_AltitudeShift = "AltitudeShift";
const char* Palette::s_Ranges = "Ranges";
const char* Palette::s_NumColorRanges = "NumColorRanges";
const char* Palette::s_PaletteFileName = "PaletteFileName";
const char* Palette::s_BWPaletteFileName = "BWPaletteFileName";
const char* Palette::s_ExPaletteFileName = "ExPaletteFileName";
const char* Palette::s_BaseColorIndex = "BaseColorIndex";
const char* Palette::s_MaxLitColor = "MaxLitColor";
const char* Palette::s_MaxHazedColor = "MaxHazedColor";
const char* Palette::s_BaseRed = "BaseRed";
const char* Palette::s_BaseGreen = "BaseGreen";
const char* Palette::s_BaseBlue = "BaseBlue";
const char* Palette::s_LightSourceFlag = "LightSourceFlag";
const char* Palette::s_DepthCueFlag = "DepthCueFlag";
const char* Palette::s_CalcLightSource = "CalcLightSource";
const char* Palette::s_CalcDepthCue = "CalcDepthCue";
const char* Palette::s_Tables = "Tables";
const char* Palette::s_FadeTableFile = "FadeTableFile";
const char* Palette::s_DepthTableFile = "DepthTableFile";
const char* Palette::s_AllFadeTableFile = "AllFadeTableFile";
const char* Palette::s_tbl = ".tbl";
const char* Palette::s_pal = ".pal";
const char* Palette::s_fit = ".fit";

#pragma region ColorRange
ColorRange::ColorRange(ColorRangeData* data, Palette* palette)
{
	this->base_color_idx = (uint32_t)data->base_color_idx;
	this->max_lit_color = (uint32_t)data->max_lit_color;
	this->max_hazed_color = (uint32_t)data->max_hazed_color;
	this->base_red = data->base_red;
	this->base_green = data->base_green;
	this->base_blue = data->base_blue;
	this->light_source_flag = (uint32_t)data->light_source_flag;
	this->depth_cue_flag = (uint32_t)data->depth_cue_flag;
	this->calc_light_source = (uint32_t)data->calc_light_source;
	this->calc_depth_cue = (uint32_t)data->calc_depth_cue;
	this->palette = palette;
}
#pragma endregion

void Palette::init()
{
	this->palette_data = nullptr;
	this->data_buf = nullptr;
	this->bw_palette_data = nullptr;
	this->ex_palette_data = nullptr;
	this->fade_palettes_data = nullptr;
	this->depth_haze_table_data = nullptr;
	this->color_ranges = nullptr;
	this->rgb_data_ = nullptr;
}

long Palette::loadPalette(FitIniFile* ini_file)
{
	long res = ini_file->readIdString((char*)s_PaletteFileName, &this->palette_filename[0], PAL_FILENAME_LEN);
	
	if (res != NO_ERR)
	{
		return res;
	}

	FullPathFileName* path = new FullPathFileName();
	path->init(&g_palettePath, &this->palette_filename[0], (char*)".pal");
	File* file = new File();
	res = file->open(path->buf, READ, FILE_DEFAULT_MAX_CHILDREN);
	if (res != NO_ERR)
	{
		delete path;
		delete file;
		return res;
	}

	FileSize size = file->fileSize();
	uint8_t* buf = reinterpret_cast<uint8_t*>(g_systemHeap->malloc(size));
	this->palette_data = buf;
	if (buf == nullptr)
	{
		delete path;
		delete file;
		return ERR_FAILED_PALETTE_ALLOC;
	}

	file->read2(buf, size);
	PaletteBlock* pal_block = reinterpret_cast<PaletteBlock*>(this);
	pal_block->initRgbData(this->palette_data);

	this->field_0x1c = 0;
	this->field_0x20 = -1;
	this->fade_r = 0;
	this->fade_g = 0;
	this->fade_b = 0;
	this->palette_data_size = size;

	uint8_t* buf2 = reinterpret_cast<uint8_t*>(g_systemHeap->malloc(size));
	this->data_buf = buf2;
	if (buf2 == nullptr)
	{
		delete path;
		delete file;
		return ERR_FAILED_PALETTE2_ALLOC;
	}

	file->close();
	delete file;
	delete path;
	return NO_ERR;
}

long Palette::loadBWPalette(FitIniFile* ini_file)
{
	long res = ini_file->readIdString((char*)s_BWPaletteFileName, &this->bw_palette_filename[0], PAL_FILENAME_LEN);

	if (res != NO_ERR)
	{
		return res;
	}

	FullPathFileName* path = new FullPathFileName();
	path->init(&g_palettePath, &this->bw_palette_filename[0], (char*)".pal");
	File* file = new File();
	res = file->open(path->buf, READ, FILE_DEFAULT_MAX_CHILDREN);
	if (res != NO_ERR)
	{
		delete path;
		delete file;
		return res;
	}

	FileSize size = file->fileSize();
	uint8_t* buf = reinterpret_cast<uint8_t*>(g_systemHeap->malloc(size));
	this->bw_palette_data = buf;
	if (buf == nullptr)
	{
		delete path;
		delete file;
		return ERR_FAILED_BW_PALETTE_ALLOC;
	}

	file->read2(buf, size);
	file->close();
	delete file;
	delete path;
	return NO_ERR;
}

long Palette::loadExtractPalette(FitIniFile* ini_file)
{
	long res = ini_file->readIdString((char*)s_ExPaletteFileName, &this->ex_palette_filename[0], PAL_FILENAME_LEN);

	if (res != NO_ERR)
	{
		return res;
	}

	FullPathFileName* path = new FullPathFileName();
	path->init(&g_palettePath, &this->ex_palette_filename[0], (char*)".pal");
	File* file = new File();
	res = file->open(path->buf, READ, FILE_DEFAULT_MAX_CHILDREN);
	if (res != NO_ERR)
	{
		delete path;
		delete file;
		return res;
	}

	FileSize size = file->fileSize();
	uint8_t* buf = reinterpret_cast<uint8_t*>(g_systemHeap->malloc(size));
	this->ex_palette_data = buf;
	if (buf == nullptr)
	{
		delete path;
		delete file;
		return ERR_FAILED_EX_PALETTE_ALLOC;
	}
	this->ex_palette_size = size >> 6; // / 772
	file->read2(buf, size);
	file->close();
	delete file;
	delete path;
	return NO_ERR;
}

long Palette::loadColorRanges(FitIniFile* ini_file)
{
	long res = ini_file->readIdLong((char*)s_NumColorRanges, (long*)&this->num_color_ranges);
	
	if (res == NO_ERR)
	{
		ColorRange* buf = (ColorRange*)operator new(this->num_color_ranges * sizeof(ColorRange));

		if (buf == nullptr) {
			this->color_ranges = nullptr;
		}
		else {
			this->color_ranges = buf;
		}

		char block_name[12];
		ColorRangeData col_range_data = {};

		if (this->num_color_ranges > 0)
		{
			for (size_t i = 0; i < this->num_color_ranges; i++)
			{
				sprintf(block_name, "Range%d", i);
				res = ini_file->seekToBlock(block_name);

				if (res != NO_ERR)
				{
					return res;
				}
				res = ini_file->readIdUChar((char*)s_BaseColorIndex, &col_range_data.base_color_idx);
				if (res != 0) {
					return res;
				}

				res = ini_file->readIdUChar((char*)s_MaxLitColor, &col_range_data.max_lit_color);
				if (res != 0) {
					return res;
				}

				res = ini_file->readIdUChar((char*)s_MaxHazedColor, &col_range_data.max_hazed_color);
				if (res != 0) {
					return res;
				}

				res = ini_file->readIdUChar((char*)s_BaseRed, &col_range_data.base_red);
				if (res != 0) {
					return res;
				}

				res = ini_file->readIdUChar((char*)s_BaseGreen, &col_range_data.base_green);
				if (res != 0) {
					return res;
				}

				res = ini_file->readIdUChar((char*)s_BaseBlue, &col_range_data.base_blue);
				if (res != 0) {
					return res;
				}

				res = ini_file->readIdUChar((char*)s_LightSourceFlag, &col_range_data.light_source_flag);
				if (res != 0) {
					return res;
				}

				res = ini_file->readIdUChar((char*)s_DepthCueFlag, &col_range_data.depth_cue_flag);
				if (res != 0) {
					return res;
				}

				res = ini_file->readIdUChar((char*)s_CalcLightSource, &col_range_data.calc_light_source);
				if (res != 0) {
					return res;
				}

				res = ini_file->readIdUChar((char*)s_CalcDepthCue, &col_range_data.calc_depth_cue);
				if (res != 0) {
					return res;
				}

				ColorRange col_range(&col_range_data, this);
				ColorRange* dst = &this->color_ranges[i];
				memcpy(dst, &col_range, sizeof(ColorRange));
			}
		}

		res = NO_ERR;
	}

	return res;
}

long Palette::loadPaletteInfo(FitIniFile* ini_file)
{
	long res = NO_ERR;

	res = ini_file->readIdLong((char*)s_HazeShift, (long*)&this->haze_shift);
	if (res != NO_ERR) return res;

	res = ini_file->readIdLong((char*)s_NumBitmapHazeLevels, (long*)&this->num_bitmap_haze_levels);
	if (res != NO_ERR) return res;
	
	res = ini_file->readIdLong((char*)s_NumDepthHazeEntriesShift, (long*)&this->depth_haze_table_entry_shift);
	if (res != NO_ERR) return res;
	
	res = ini_file->readIdLong((char*)s_NumDepthAtHazeLevelTables, (long*)&this->num_depth_haze_tables);
	if (res != NO_ERR) return res;
	
	res = ini_file->readIdLong((char*)s_AltitudeShift,(long*)&this->altitude_shift);
	if (res != NO_ERR) return res;
	
	res = ini_file->readIdLong((char*)s_AltitudeShift, (long*)&this->altitude_shift);
	if (res != NO_ERR) return res;

	// calculate the haze table dimensions from the shift values
	int haze_table_size = 1 << (this->haze_shift & 0x1f);
	// NOTE: & 0x1f ensures shifts are in 0-31 range (valid x86 shift range), preventin UB.
	this->init_flag = 0;
	this->haze_table_size = haze_table_size;
	this->haze_table_half_size = haze_table_size / 2;
	this->bitmap_haze_levels_copy = this->num_bitmap_haze_levels;
	this->bitmap_haze_scale = this->num_bitmap_haze_levels * 256;
	this->depth_haze_table_size = 1 << (this->depth_haze_table_entry_shift & 0x1f);
	this->total_depth_tables = this->num_depth_haze_tables << (this->altitude_shift & 0x1f);

	res = this->loadPalette(ini_file);
	if (res != NO_ERR) return res;

	this->loadBWPalette(ini_file);
	this->loadExtractPalette(ini_file);
	res = ini_file->seekToBlock((char*)s_Ranges); 
	if (res != NO_ERR) return res;

	res = this->loadColorRanges(ini_file);
	return res;
}

long Palette::loadTables()
{
	FullPathFileName* depth_path = new FullPathFileName();
	FullPathFileName* fade_path = new FullPathFileName();
	FullPathFileName* all_fade_path = new FullPathFileName();

	depth_path->init(&g_palettePath, this->depth_table_filename, (char*)s_tbl);
	fade_path->init(&g_palettePath, this->fade_table_filename, (char*)s_tbl);
	all_fade_path->init(&g_palettePath, this->all_fade_table_filename, (char*)s_tbl);

	File* depth_file = new File();
	File* fade_file = new File();
	File* all_fade_file = new File();

	long res = depth_file->open(depth_path->buf, READ, FILE_DEFAULT_MAX_CHILDREN);
	
	if (res == NO_ERR)
	{
		this->loadDepthHazeTables(depth_file);
		depth_file->close();

		long res = fade_file->open(fade_path->buf, READ, FILE_DEFAULT_MAX_CHILDREN);
		if (res == NO_ERR)
		{
			this->loadFadePalettes(fade_file);
			fade_file->close();

			long res = all_fade_file->open(all_fade_path->buf, READ, FILE_DEFAULT_MAX_CHILDREN);
			if (res == NO_ERR) 
			{
				this->loadAllFadePalettes(all_fade_file);
				all_fade_file->close();

				delete depth_file;
				delete fade_file;
				delete all_fade_file;

				delete depth_path;
				delete fade_path;
				delete all_fade_path;

				return NO_ERR;
			}
		}
	}
	
	delete depth_file;
	delete fade_file;
	delete all_fade_file;

	delete depth_path;
	delete fade_path;
	delete all_fade_path;

	return res;
}

long Palette::loadFadePalettes(File* file)
{
	FileSize size = file->fileSize();
	this->fade_palettes_size = size;
	uint8_t* buf = reinterpret_cast<uint8_t*>(g_systemHeap->malloc(size));
	this->fade_palettes_data = buf;
	if (buf == nullptr)
	{
		return ERR_FADE_PALETTE_ALLOC;
	}
	file->read2(buf, size);
	this->fade_palettes_size = size >> 8;
	return NO_ERR;
}

long Palette::loadAllFadePalettes(File* file)
{
	FileSize size = file->fileSize();
	this->all_fade_palettes_size = size;
	int32_t* buf = reinterpret_cast<int32_t*>(g_systemHeap->malloc(size));
	this->all_fade_palettes_data = buf;
	if (buf == nullptr)
	{
		return ERR_FADE_PALETTE_ALLOC;
	}
	file->read2(reinterpret_cast<uint8_t*>(buf), size);
	this->all_fade_palettes_size = size >> 8;
	return NO_ERR;
}

long Palette::loadDepthHazeTables(File* file)
{
	FileSize size = file->fileSize();
	size_t expected_size = (this->num_depth_haze_tables << this->depth_haze_table_entry_shift) * 4;
	
	if (size != expected_size)
	{
		return ERR_HAZE_DEPTH_TABLE_BAD_FILE;
	}

	size = file->fileSize();
	uint8_t* buf = reinterpret_cast<uint8_t*>(g_systemHeap->malloc(size));
	this->depth_haze_table_data = buf;
	if (buf == nullptr)
	{
		return ERR_HAZE_DEPTH_TABLE_ALLOC;
	}
	size = file->fileSize();
	file->read2(buf, size);
	return NO_ERR;
}

// Given a depth, select the correct haze table and cache its bounds.
void Palette::recalculateDepthVsHazeInfo(long depth_value)
{
	int depth_idx = 0;

	if (depth_value < this->total_depth_tables)
	{
		// raw depth to table index;
		depth_idx = depth_value >> this->altitude_shift;
	}
	else 
	{
		// clamp to num_depth_at_haze_level_tables
		depth_idx = this->num_depth_haze_tables - 1;
	}

	// this->depth_haze_table_data + (depth << shift) (int32_t* arithemetic)
	int32_t* haze_table = (int32_t*)(
		this->depth_haze_table_data + (depth_idx << this->depth_haze_table_entry_shift));

	this->curr_depth_haze_table = haze_table;
	this->curr_min_haze = haze_table[0];
	g_lastMinDepth = -1;
	this->curr_max_haze = haze_table[this->depth_haze_table_size - 1];
}

void Palette::activate(long param_1, long param_2)
{
	uint8_t* data;

	if (param_1 == 1)
	{
		if (param_2 < this->ex_palette_size)
		{
			if (param_2 < 0)
			{
				data = nullptr;
			}
			else
			{
				data = this->ex_palette_data + param_2 * 772 + 4;
			}
		}
		else
		{
			data = nullptr;
		}
		g_application->activatePalette(data, 0, 256);
		return;
	}
	
	if (param_1 != 2)
	{
		g_application->activatePalette(this->rgb_data_, 0, 256);
		return;
	}

	g_application->activatePalette(this->bw_palette_data, 0, 256);
}

long Palette::init3(FitIniFile* ini_file)
{
	long res = NO_ERR;

	res = this->loadPaletteInfo(ini_file);
	if (res != NO_ERR) return res;

	res = ini_file->seekToBlock((char*)s_Tables);
	if (res != NO_ERR) return res;

	res = ini_file->readIdString((char*)s_FadeTableFile, this->fade_table_filename, PAL_FILENAME_LEN);
	if (res != NO_ERR) return res;

	res = ini_file->readIdString((char*)s_DepthTableFile, this->depth_table_filename, PAL_FILENAME_LEN);
	if (res != NO_ERR) return res;

	res = ini_file->readIdString((char*)s_AllFadeTableFile, this->all_fade_table_filename, PAL_FILENAME_LEN);
	if (res != NO_ERR) return res;

	res = this->loadTables();
	if (res != NO_ERR) return res;
	
	this->recalculateDepthVsHazeInfo(0);
	return NO_ERR;
}

long Palette::init2(char* palette_name)
{
	FullPathFileName* path = new FullPathFileName();
	path->buf = nullptr;
	path->init(&g_palettePath, palette_name, (char*)s_fit);

	FitIniFile* ini_file = new FitIniFile();
	long res = ini_file->vtable->open2(ini_file, path->buf, READ, FILE_DEFAULT_MAX_CHILDREN);
	
	if (res != NO_ERR)
	{
		delete ini_file;
		delete path;
		return res;
	}

	res = ini_file->seekToBlock((char*)"Palette");

	if (res != NO_ERR)
	{
		// Failed to seek "Palette" block in fit ini file
		delete ini_file;
		delete path;
		return res;
	}

	res = this->init3(ini_file);
	if (res != NO_ERR)
	{
		ini_file->close();
		delete ini_file;
		delete path;
		return res;
	}

	delete ini_file;
	delete path;
	return res;
}
#pragma endregion

static AutoFuncPatchRegister reg_PaletteBlock{
	"PaletteBlock",
	{
		{ (void*)va_PaletteBlock_initRgbData, GetMemberPtr(&PaletteBlock::initRgbData)},
	}
};

static AutoFuncPatchRegister reg_Palette{
	"Palette",
	{
		{ (void*)va_Palette_init, GetMemberPtr(&Palette::init)},
		{ (void*)va_Palette_init2, GetMemberPtr(&Palette::init2)},
		{ (void*)va_Palette_init3, GetMemberPtr(&Palette::init3)},
	}
};