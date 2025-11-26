#include <string_view>
#include "mcx_inject.hpp"
#include "mcx/lib/heap.hpp"
#include "mcx/lib/file.hpp"
#include "mcx/lib/fit_ini_file.hpp"


// __thiscall FitIniFile::FitIniFile(void)
MCXI_CTOR_HOOK Hook_FitIniFile_ctor(FitIniFile* this_ptr, void* edx_unused)
{
    File* file = reinterpret_cast<File*>(this_ptr);
    new (file) File();

    file->vtable = (File_vtable*)MCX_RUNTIME_ADDR(va_FitIniFile_vftable);
    FitIniFile* fit_ini_file = reinterpret_cast<FitIniFile*>(file);
    fit_ini_file->num_blocks = 0;
    fit_ini_file->blocks = 0;
    fit_ini_file->field_0x54 = 0;
    fit_ini_file->field_0x58 = 0;
    fit_ini_file->field_0x5c = 0;

    MOV_THIS_TO_EAX(fit_ini_file)
}

#pragma warning(disable:26495, justification:"Initialization happens in Hook_FastFile_ctor" )
FitIniFile::FitIniFile()
{
    Hook_FitIniFile_ctor(this, nullptr);
}
#pragma warning(default:26495)

int FitIniFile::open(File* file, unsigned long file_size, long num_children)
{
    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::open(%p,%d,%d) => this=%p. (%s)",
        file,
        file_size,
        num_children,
        this,
        file->file_name
    );

    File* f = reinterpret_cast<File*>(this);
    auto vtable = (File_vtable*)MCX_RUNTIME_ADDR(va_File_vftable);
    long open_res = vtable->open(f, file, file_size, -1);

    if (open_res != NO_ERR)
    {
        return open_res;
    }

    return this->afterOpen();
}

long FitIniFile::afterOpen()
{
    // HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::afterOpen(%s)", this->file_name);
    unsigned char line_buf[256];
    auto line_buf_ = reinterpret_cast<char*>(line_buf);

    if (this->file_mode == CREATE && this->parent == nullptr)
    {
        sprintf(line_buf_, "%s \n\r", FITINI_HEADER);
        this->write2(line_buf, strlen(line_buf_) - 1);
        this->num_blocks = 0;
        return NO_ERR;
    }

    unsigned char header[12];
    this->readLine(header, 11);
    char* header_ptr = strstr(reinterpret_cast<char*>(header), FITINI_HEADER);
    if (header_ptr == nullptr)
    {
        return INI_ERR_NOT_A_FITINI_FILE;
    }

    uint32_t blocks_num = this->countBlocks();
    this->num_blocks = blocks_num;
    UserHeap* system_heap = *g_systemHeap_ptr;

    FitIniBlock* blocks_ = mcx::heap::adaptive_malloc<FitIniBlock>(system_heap, blocks_num * sizeof(FitIniBlock));

    this->blocks = blocks_;

    if (blocks_ == nullptr)
    {
        return INI_ERR_BLOCKS_MALLOC_FAILED;
    }
    memset(blocks_, 0, blocks_num * sizeof(FitIniBlock));

    unsigned long curr_blk_idx = 0;
    while (this->findNextBlockStart(line_buf_, 254) != INI_ERR_END_OF_BLOCKS)
    {
        // Can this ever happen?
        if (curr_blk_idx >= blocks_num)
        {
            return INI_ERR_BLOCKS_OVERRUN;
        }

        long i = 1; // skip '['
        while (line_buf_[i] != ']' && line_buf_[i] != '\n' && i < 49)
        {
            this->blocks[curr_blk_idx].block_id[i - 1] = line_buf_[i];

            //HOOK_DLOGF(FITINIFILE, INFO, "   File::afterOpen(%s): '%s'[%d]='%c'",
            //    this->file_name,
            //    line_buf_,
            //    i,
            //    line_buf_[i]);

            i += 1;
        }

        //HOOK_DLOGF(FITINIFILE, INFO, "File::afterOpen(%s) [%d %s %d]",
        //    this->file_name,
        //    curr_blk_idx,
        //    this->blocks[curr_blk_idx].block_id,
        //    this->blocks[curr_blk_idx].block_offset);

        if (i >= 49)
        {
            STOP("BlockId oveerrun? i=%d", i);
            return NO_ERR;
        }

        if (line_buf_[i] == '\n')
        {
            HOOK_DLOG(FITINIFILE, FATAL) << "Syntax error.";
            return INI_ERR_MALFORMED_BLOCK;
        }

        blocks_[curr_blk_idx].block_id[i - 1] = '\0';

        blocks_[curr_blk_idx].block_offset = this->pos;

        curr_blk_idx += 1;
    }

    if (curr_blk_idx != this->num_blocks) 
    {
        return INI_ERR_UNEXPECTED_BLOCKS_NUM;
    }

    return NO_ERR;
}

void FitIniFile::close()
{
    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::close(%s)", this->file_name);
    this->atClose();
    File* file = reinterpret_cast<File*>(this);
    return ((File_vtable*)MCX_RUNTIME_ADDR(va_File_vftable))->close(file);
}

void FitIniFile::atClose()
{
    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::atClose(%s)", this->file_name);

    unsigned char line_buf[200];
    auto line_buf_ = reinterpret_cast<char*>(line_buf);

    if (this->file_mode == CREATE)
    {
        this->seek(0, SEEK_END);
        sprintf(line_buf_, FITINI_FOOTER);
        this->write2(line_buf, strlen(line_buf_) - 1);
    }

    UserHeap* system_heap = *g_systemHeap_ptr;
    mcx::heap::adaptive_free(system_heap, this->blocks);
    this->blocks = nullptr;
}

long FitIniFile::findNextBlockStart(char* line, unsigned long line_length)
{
    unsigned char buf[256];
    unsigned char* curr = NULL;

    do
    {
        if (line == nullptr)
        {
            this->readLine(buf, 254);
            curr = buf;
        }
        else
        {
            auto uline = reinterpret_cast<unsigned char*>(line);
            this->readLine(uline, line_length);
            curr = uline;
        }
    } 
    while (!this->eof() && (*curr != '['));

    if (this->eof() == TRUE)
    {
        return INI_ERR_END_OF_BLOCKS;
    }

    return NO_ERR;
}

long FitIniFile::countBlocks()
{
    long blocks_num = 0;
    FileOffset old_offset = this->pos;

    long next_blk_start = this->findNextBlockStart(nullptr, 0);

    while (next_blk_start != INI_ERR_END_OF_BLOCKS)
    { 
        blocks_num += 1;
        next_blk_start = this->findNextBlockStart(nullptr, 0);
    }

    // rewind
    this->seek(old_offset, SEEK_SET);
    
    return blocks_num;
}

FileClass FitIniFile::getFileClass()
{
    return FileClass::INIFILE;
}

long FitIniFile::textToLong(char* line)
{
    char* hex_str = strstr(line, "0x");
    
    // fastpath, plain integer, not hex
    if (hex_str == nullptr)
    {
        return atol(line);
    }

    hex_str += 2; // skip 0x

    size_t len = 0;
    while (hex_str[len] != '\0')
    {
        char c = toupper(hex_str[len]);

        // validate hex digits
        if (!((c >= '0' && c <= '9') || (c >= 'A' && c <= 'F')))
        {
            break;
        }
        len++;
    }

    // convert hex string to long, reverse order
    long result = 0;
    long shift = 0;

    for (int i = len - 1; i >= 0; i--)
    {
        char c = toupper(hex_str[i]);
        int digit;

        if (c >= '0' && c <= '9')
        {
            digit = c - '0';  // '0'-'9' -> 0-9
        }
        else if (c >= 'A' && c <= 'F')
        {
            digit = c - 'A' + 10;  // 'A'-'F' -> 10-15
        }
        else
        {
            return 0;  // should not happen
        }

        result += digit << shift;
        shift += 4;  // each hex digit is 4 bits
    }

    return result;
}

long FitIniFile::booleanToLong(char* line)
{
    char ch = *line;
    size_t i = 0;

    while (ch != '\0' && isspace(ch) != FALSE) 
    {
        i += 1;
        ch = line[i];
    }

    ch = toupper(line[i]);

    if (ch == 'T')
    {
        return 1;
    }

    return this->textToLong(line);
}

long FitIniFile::byteToTextDec(char* text_buf, unsigned char value, unsigned long buf_length)
{
    return NumberToText("%d", text_buf, value, buf_length);
}

long FitIniFile::byteToTextHex(char* text_buf, unsigned char value, unsigned long buf_length)
{
    return NumberToText("0x%x", text_buf, value, buf_length);
}

long FitIniFile::floatToText(char* text_buf, float value, unsigned long buf_length)
{
    return NumberToText("%f4", text_buf, value, buf_length);
}

long FitIniFile::longToTextDec(char* text_buf, long value, unsigned long buf_length)
{
    return NumberToText("%d", text_buf, value, buf_length);
}

long FitIniFile::longToTextHex(char* text_buf, long value, unsigned long buf_length)
{
    return NumberToText("0x%x", text_buf, value, buf_length);
}

long FitIniFile::shortToTextDec(char* text_buf, short value, unsigned long buf_length)
{
    return NumberToText("%d", text_buf, value, buf_length);
}

long FitIniFile::shortToTextHex(char* text_buf, short value, unsigned long buf_length)
{
    return NumberToText("0x%x", text_buf, value, buf_length);
}

long FitIniFile::copyString(char* dst, char* src, unsigned long dst_length)
{
    // Skip source until opening quote(")
    while (*src != '"')
    { 
        src += 1;
    }

    src += 1; // Consume open quote (")

    size_t i = 0;
    while (*src != '"' && i < dst_length)
    {
        dst[i] = *src;
        src += 1;
        i += 1;
    }

    if (i == dst_length)
    {
        return INI_ERR_BUFFER_TOO_SMALL;
    }

    dst[i] = '\0';

    return NO_ERR;
}

long FitIniFile::readIdLong(char* key, long* out_value)
{
    auto orig = GetTrampoline<long(__thiscall*)(void*, char*, long*)>(va_FitIniFile_readIdLong);
    long ret = orig(this, key, out_value);
    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::readIdLong('%s') => %d (%d)", key, ret == 0 ? *out_value : 0, ret);
    return ret;
}


static AutoFuncPatchRegister reg_FitIniFile{
    "FitIniFile",
    {
        { (void*)va_FitIniFile_ctor, (void*)Hook_FitIniFile_ctor },
        { (void*)va_FitIniFile_open, GetMemberPtr(&FitIniFile::open)},
        { (void*)va_FitIniFile_close, GetMemberPtr(&FitIniFile::close)},
        { (void*)va_FitIniFile_afterOpen, GetMemberPtr(&FitIniFile::afterOpen)},
        { (void*)va_FitIniFile_atClose, GetMemberPtr(&FitIniFile::atClose)},
        { (void*)va_FitIniFile_getFileClass, GetMemberPtr(&FitIniFile::getFileClass)},
        { (void*)va_FitIniFile_findNextBlockStart, GetMemberPtr(&FitIniFile::findNextBlockStart)},
        { (void*)va_FitIniFile_countBlocks, GetMemberPtr(&FitIniFile::countBlocks)},
        { (void*)va_FitIniFile_textToLong, GetMemberPtr(&FitIniFile::textToLong)},
        { (void*)va_FitIniFile_booleanToLong, GetMemberPtr(&FitIniFile::booleanToLong)},
        { (void*)va_FitIniFile_byteToTextDec, GetMemberPtr(&FitIniFile::byteToTextDec)},
        { (void*)va_FitIniFile_byteToTextHex, GetMemberPtr(&FitIniFile::byteToTextHex)},
        { (void*)va_FitIniFile_longToTextDec, GetMemberPtr(&FitIniFile::longToTextDec)},
        { (void*)va_FitIniFile_longToTextHex, GetMemberPtr(&FitIniFile::longToTextHex)},
        { (void*)va_FitIniFile_shortToTextDec, GetMemberPtr(&FitIniFile::shortToTextDec)},
        { (void*)va_FitIniFile_shortToTextHex, GetMemberPtr(&FitIniFile::shortToTextHex)},
        { (void*)va_FitIniFile_floatToText, GetMemberPtr(&FitIniFile::floatToText)},
        { (void*)va_FitIniFile_copyString, GetMemberPtr(&FitIniFile::copyString)},
        //{ (void*)va_FitIniFile_readIdLong, GetMemberPtr(&FitIniFile::readIdLong)}
    }
};