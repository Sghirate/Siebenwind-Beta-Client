#pragma once

#include "Minimal.h"
#include "DataStream.h"
#include <filesystem>

namespace Core
{

struct MappedFile : public StreamReader
{
    MappedFile();
    virtual ~MappedFile();

    bool Load(const std::filesystem::path& a_path);
    void Unload();

private:
    static unsigned char* Map(const std::filesystem::path& a_path, size_t& a_length);
    static void Unmap(unsigned char* a_buffer, size_t a_length);
};

} // namespace Core
