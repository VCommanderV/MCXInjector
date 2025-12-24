#include "mcx/lib/heap.hpp"
#include "mcx/color/color.hpp"

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

static AutoFuncPatchRegister reg_PaletteBlock{
	"PaletteBlock",
	{
		{ (void*)va_PaletteBlock_initRgbData, GetMemberPtr(&PaletteBlock::initRgbData)},
	}
};
