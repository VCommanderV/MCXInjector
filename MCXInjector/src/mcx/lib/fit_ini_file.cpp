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
    fit_ini_file->curr_block_id = 0;
    fit_ini_file->curr_block_offset = 0;
    fit_ini_file->curr_block_size = 0;

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
    UserHeap* system_heap = g_systemHeap;

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
    //HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::close(%s)", this->file_name);
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

    UserHeap* system_heap = g_systemHeap;
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
    long result = 0;
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

unsigned long FitIniFile::textToULong(char* line)
{
    unsigned long result = 0;
    char* hex_str = strstr(line, "0x");

    // fastpath, plain integer, not hex
    if (hex_str == nullptr)
    {
        return (unsigned long)atol(line);
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

char FitIniFile::textToChar(char* line)
{
    char result = 0;
    char* hex_str = strstr(line, "0x");

    // fastpath, plain integer, not hex
    if (hex_str == nullptr)
    {
        char res = (char)atol(line);
        return res;
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

unsigned char FitIniFile::textToUChar(char* line)
{
    unsigned char result = 0;
    char* hex_str = strstr(line, "0x");

    // fastpath, plain integer, not hex
    if (hex_str == nullptr)
    {
        char res = (unsigned char)atol(line);
        return res;
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

short FitIniFile::textToShort(char* line)
{
    short result = 0;
    char* hex_str = strstr(line, "0x");

    // fastpath, plain integer, not hex
    if (hex_str == nullptr)
    {
        short res = (short)atol(line);
        return res;
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

short FitIniFile::textToUShort(char* line)
{
    unsigned short result = 0;
    char* hex_str = strstr(line, "0x");

    // fastpath, plain integer, not hex
    if (hex_str == nullptr)
    {
        short res = (unsigned short)atol(line);
        return res;
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

float FitIniFile::textToFloat(char* line)
{
    return (float)atof(line);
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

long FitIniFile::getNextWord(char** line, char* buf, unsigned long buf_length)
{
    /*
        char* str = " 0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0";
        char** line = &str;
        -------------------------------------------------------------
        **line   // char ptr:  ' '
        *line    // str ptr:   " 0, 1, 2, 3, 4, 5, 6, 7, 0, 0, 0, 0"
    */
    // HOOK_DLOGF(FITINIFILE, INFO ,"FitIniFile::getNextWord(%s)", *line);

    if (**line == '\0' || **line == '/')
    {
        return INI_ERR_END_OF_LINE;
    }

    // Skip leading whitespace, tabs, and commas
    while (**line != '\0' && (**line == ' ' || **line == '\t' || **line == ','))
    {
        // advance the string pointer
        *line = *line + 1;
    }

    // empty or comment after trim leading
    if (**line == '\0' || **line == '/')
    {
        return INI_ERR_END_OF_LINE;
    }

    char* word_start = *line;
    size_t word_len = 0;

    while (**line != '\0' && (**line != ' ' && **line != '\t' && **line != ','))
    {
        *line = *line + 1;
        word_len += 1;
    }

    if (buf_length < word_len)
    {
        return INI_ERR_BUFFER_TOO_SMALL;
    }

    strncpy(buf, word_start, word_len);
    buf[word_len] = '\0';

    // HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::getNextWord(%s) => '%s'", *line, buf);

    return NO_ERR;
}

char FitIniFile::mathToChar(char*)
{
    // ???
    return 0;
}

float FitIniFile::mathToFloat(char*)
{
    return 0.0;
}

long FitIniFile::mathToLong(char*)
{
    return 0;
}

short FitIniFile::mathToShort(char*)
{
    return 0;
}

unsigned char FitIniFile::mathToUChar(char*)
{
    return 0;
}

long FitIniFile::mathToULong(char*)
{
    return 0;
}

unsigned short FitIniFile::mathToUShort(char*)
{
    return 0;
}

long FitIniFile::readIdBoolean(char* key, int* result)
{
    unsigned char line_buf[256];

    // points to start of b {id} = {value}
    //                             ^
    char* value_start = nullptr; 
    
    long search_res = this->find_key_value("b", key, line_buf, &value_start);

    if (search_res != NO_ERR)
    {
        *result = 0; 
        return search_res;
    }

    *result = this->booleanToLong(value_start);
    
    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::readIdBool('%s','%s') => %d (%x)", this->file_name, key, *result, search_res);
 
    return NO_ERR;
}

long FitIniFile::readIdLong(char* key, long* result)
{
    unsigned char line_buf[256];

    // points to start of l {id} = {value}
    //                             ^
    char* value_start = nullptr;

    long search_res = this->find_key_value("l", key, line_buf, &value_start);

    if (search_res != NO_ERR)
    {
        *result = 0;
        return search_res;
    }

    *result = this->textToLong(value_start);

    if (*result == 0)
    {
        *result = this->mathToLong(value_start);
    }

    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::readIdLong('%s','%s') => %d (%x)", this->file_name, key, *result, search_res);

    return NO_ERR;
}

long FitIniFile::readIdChar(char* key, char* result)
{
    unsigned char line_buf[256];
 
    char* value_start = nullptr;

    long search_res = this->find_key_value("c", key, line_buf, &value_start);

    if (search_res != NO_ERR)
    {
        *result = 0;
        return search_res;
    }

    char ch = this->textToChar(value_start);

    *result = ch;
    if (*result == '\0')
    {
        *result = this->mathToChar(value_start);
    }

    // HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::readIdChar('%s','%s') => ['%s'] %d (%x)", this->file_name, key, (char*)line_buf, *result, search_res);

    return NO_ERR;
}

long FitIniFile::readIdUChar(char* key, unsigned char* result)
{
    unsigned char line_buf[256];

    char* value_start = nullptr;

    long search_res = this->find_key_value("uc", key, line_buf, &value_start);

    if (search_res != NO_ERR)
    {
        *result = 0;
        return search_res;
    }

    char ch = this->textToUChar(value_start);

    *result = ch;
    if (*result == '\0')
    {
        *result = this->mathToUChar(value_start);
    }

    // HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::readIdChar('%s','%s') => ['%s'] %d (%x)", this->file_name, key, (char*)line_buf, *result, search_res);

    return NO_ERR;
}

long FitIniFile::readIdULong(char* key, unsigned long* result)
{
    unsigned char line_buf[256];

    // points to start of l {id} = {value}
    //                             ^
    char* value_start = nullptr;

    long search_res = this->find_key_value("ul", key, line_buf, &value_start);

    if (search_res != NO_ERR)
    {
        *result = 0;
        return search_res;
    }

    *result = this->textToULong(value_start);

    if (*result == 0)
    {
        *result = this->mathToULong(value_start);
    }

    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::readIdULong('%s','%s') => %d (%x)", this->file_name, key, *result, search_res);

    return NO_ERR;
}

long FitIniFile::readIdFloat(char* key, float* result)
{
    unsigned char line_buf[256];

    // points to start of l {id} = {value}
    //                             ^
    char* value_start = nullptr;

    long search_res = this->find_key_value("f", key, line_buf, &value_start);

    if (search_res != NO_ERR)
    {
        *result = 0;
        return search_res;
    }

    *result = this->textToFloat(value_start);

    if (*result == 0)
    {
        *result = this->mathToFloat(value_start);
    }

    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::readIdFloat('%s','%s') => %f (%x)", this->file_name, key, *result, search_res);

    return NO_ERR;
}

long FitIniFile::readIdString(char* key, char* buf, unsigned long buf_length)
{
    unsigned char line_buf[256];

    // points to start of l {id} = {value}
    //                             ^
    char* value_start = nullptr;

    long search_res = this->find_key_value("st", key, line_buf, &value_start);

    if (search_res != NO_ERR)
    {
        return search_res;
    }

    long copy_res = this->copyString(buf, value_start, buf_length);

    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::readIdString('%s','%s') => %s (%x)", this->file_name, key, buf, copy_res);
    
    return copy_res;
}

long FitIniFile::readIdShort(char* key, short* result)
{
    unsigned char line_buf[256];

    // points to start of l {id} = {value}
    //                             ^
    char* value_start = nullptr;

    long search_res = this->find_key_value("s", key, line_buf, &value_start);

    if (search_res != NO_ERR)
    {
        *result = 0;
        return search_res;
    }

    *result = this->textToShort(value_start);

    if (*result == 0)
    {
        *result = this->mathToShort(value_start);
    }

    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::readIdShort('%s','%s') => %d (%x)", this->file_name, key, *result, search_res);

    return NO_ERR;
}

long FitIniFile::readIdUShort(char* key, unsigned short* result)
{
    unsigned char line_buf[256];

    // points to start of l {id} = {value}
    //                             ^
    char* value_start = nullptr;

    long search_res = this->find_key_value("us", key, line_buf, &value_start);

    if (search_res != NO_ERR)
    {
        *result = 0;
        return search_res;
    }

    *result = this->textToUShort(value_start);

    if (*result == 0)
    {
        *result = this->mathToUShort(value_start);
    }

    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::readIdUShort('%s','%s') => %d (%x)", this->file_name, key, *result, search_res);

    return NO_ERR;
}

long FitIniFile::readIdLongArray(char* key, long* buf, unsigned long buf_length)
{             
    unsigned char line_buf[256];
    char item_buf[12];
    char* items_ptr = nullptr;
    uint32_t block_end = 0;
    unsigned long items_count = 0;


    long search_res = this->find_array_key_value("l", key, line_buf, &items_ptr, buf_length, &block_end, &items_count);

    if (search_res != NO_ERR)
    {
        return search_res;
    }

    unsigned long parsed_items = 0;

    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::readIdLongArray('%s','%s') => %s (%x)", this->file_name, key, (char*)line_buf, search_res);
 
    while (this->pos < block_end && parsed_items < items_count)
    {
        long next_word_res = this->getNextWord(&items_ptr, item_buf, 9);

        if (next_word_res == INI_ERR_END_OF_LINE)
        {
            this->readLine(line_buf, 254);
            items_ptr = (char*)line_buf;
            continue;
        }

        if (next_word_res != NO_ERR)
        {
            return next_word_res;
        }

        long item = this->textToLong(item_buf);
        if (item == 0)
        {
            item = this->mathToLong(item_buf);
        }

        buf[parsed_items] = item;
        
        HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::readIdLongArray('%s','%s') => buf[%d]=%d", this->file_name, key, parsed_items, item);
       
        parsed_items += 1;
    }

    if (this->pos >= block_end && parsed_items < items_count) {
        return INI_ERR_ARRAY_NOT_ENOUGH_ITEMS;
    }

    return NO_ERR;
}

long FitIniFile::writeBlock(char* key)
{
    unsigned char block_id_buf[256];

    sprintf((char*)block_id_buf, "\r\n[%s]\r\n", key);

    size_t block_id_len = strlen((char*)block_id_buf);

    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::writeBlock('%s', '%s')", this->file_name, (char*)block_id_buf);

    return this->write2(block_id_buf, block_id_len);
}

long FitIniFile::writeIdLong(char* key, long value)
{
    return WriteInteger("l", key, value);
}

long FitIniFile::writeIdShort(char* key, short value)
{
    return WriteInteger("s", key, value);
}

long FitIniFile::writeIdUShort(char* key, unsigned short value)
{
    return WriteInteger("us", key, value);
}

long FitIniFile::writeIdULong(char* key, unsigned long value)
{
    return WriteInteger("ul", key, value);
}

long FitIniFile::writeIdFloat(char* key, float value)
{
    unsigned char key_buf[256];
    sprintf((char*)key_buf, "f %s=%f\r\n", key, value);
    size_t key_len = strlen((char*)key_buf);
    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::writeIdFloat('%s', '%s')",
        this->file_name, (char*)key_buf);
    return this->write2(key_buf, key_len);
}

long FitIniFile::writeIdBoolean(char* key, BOOL value)
{
    unsigned char key_buf[256];

    sprintf((char*)key_buf, "b %s=%s\r\n", key, value == FALSE ? "FALSE" : "TRUE");
    size_t key_len = strlen((char*)key_buf);

    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::writeBoolean('%s', '%s')", this->file_name, (char*)key_buf);

    return this->write2(key_buf, key_len);
}

long FitIniFile::writeIdLongArray(char* key, long* values, unsigned long length)
{
    return this->WriteIntegerArray("l", key, values, length);
}

long FitIniFile::writeIdUShortArray(char* key, unsigned short* values, unsigned long length)
{
    return this->WriteIntegerArray("us", key, values, length);
}

long FitIniFile::writeIdUCharArray(char* key, unsigned char* values, unsigned long length)
{
    return this->WriteIntegerArray("uc", key, values, length);
}

long FitIniFile::writeIdString(char* key, char* value)
{
    unsigned char key_buf[256];

    sprintf((char*)key_buf, "st %s=\"%s\"\r\n", key, value);
    size_t key_len = strlen((char*)key_buf);

    HOOK_DLOGF(FITINIFILE, INFO, "FitIniFile::writeBoolean('%s', '%s')", this->file_name, (char*)key_buf);

    return this->write2(key_buf, key_len);
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
        { (void*)va_FitIniFile_textToChar, GetMemberPtr(&FitIniFile::textToChar)},
        { (void*)va_FitIniFile_booleanToLong, GetMemberPtr(&FitIniFile::booleanToLong)},
        { (void*)va_FitIniFile_byteToTextDec, GetMemberPtr(&FitIniFile::byteToTextDec)},
        { (void*)va_FitIniFile_byteToTextHex, GetMemberPtr(&FitIniFile::byteToTextHex)},
        { (void*)va_FitIniFile_longToTextDec, GetMemberPtr(&FitIniFile::longToTextDec)},
        { (void*)va_FitIniFile_longToTextHex, GetMemberPtr(&FitIniFile::longToTextHex)},
        { (void*)va_FitIniFile_shortToTextDec, GetMemberPtr(&FitIniFile::shortToTextDec)},
        { (void*)va_FitIniFile_shortToTextHex, GetMemberPtr(&FitIniFile::shortToTextHex)},
        { (void*)va_FitIniFile_floatToText, GetMemberPtr(&FitIniFile::floatToText)},
        { (void*)va_FitIniFile_copyString, GetMemberPtr(&FitIniFile::copyString)},
        { (void*)va_FitIniFile_getNextWord, GetMemberPtr(&FitIniFile::getNextWord)},
        { (void*)va_FitIniFile_readIdBoolean, GetMemberPtr(&FitIniFile::readIdBoolean)},
        { (void*)va_FitIniFile_readIdLong, GetMemberPtr(&FitIniFile::readIdLong)},
        { (void*)va_FitIniFile_readIdULong, GetMemberPtr(&FitIniFile::readIdULong)},
        { (void*)va_FitIniFile_readIdChar, GetMemberPtr(&FitIniFile::readIdChar)},
        { (void*)va_FitIniFile_readIdUChar, GetMemberPtr(&FitIniFile::readIdUChar)},
        { (void*)va_FitIniFile_readIdShort, GetMemberPtr(&FitIniFile::readIdShort)},
        { (void*)va_FitIniFile_readIdUShort, GetMemberPtr(&FitIniFile::readIdUShort)},
        { (void*)va_FitIniFile_readIdFloat, GetMemberPtr(&FitIniFile::readIdFloat)},
        { (void*)va_FitIniFile_readIdString, GetMemberPtr(&FitIniFile::readIdString)},
        { (void*)va_FitIniFile_readIdLongArray, GetMemberPtr(&FitIniFile::readIdLongArray)},
        { (void*)va_FitIniFile_writeBlock, GetMemberPtr(&FitIniFile::writeBlock)},
        { (void*)va_FitIniFile_writeIdLong, GetMemberPtr(&FitIniFile::writeIdLong)},
        { (void*)va_FitIniFile_writeIdShort, GetMemberPtr(&FitIniFile::writeIdShort) },
        { (void*)va_FitIniFile_writeIdUShort, GetMemberPtr(&FitIniFile::writeIdUShort) },
        { (void*)va_FitIniFile_writeIdULong, GetMemberPtr(&FitIniFile::writeIdULong) },
        { (void*)va_FitIniFile_writeIdFloat, GetMemberPtr(&FitIniFile::writeIdFloat) },
        { (void*)va_FitIniFile_writeIdBoolean, GetMemberPtr(&FitIniFile::writeIdBoolean) },
        { (void*)va_FitIniFile_writeIdString, GetMemberPtr(&FitIniFile::writeIdString) },
        { (void*)va_FitIniFile_writeIdLongArray, GetMemberPtr(&FitIniFile::writeIdLongArray) },
        { (void*)va_FitIniFile_writeIdUShortArray, GetMemberPtr(&FitIniFile::writeIdUShortArray) },
        { (void*)va_FitIniFile_writeIdUCharArray, GetMemberPtr(&FitIniFile::writeIdUCharArray) },

    }
};