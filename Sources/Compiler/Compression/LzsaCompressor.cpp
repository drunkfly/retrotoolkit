#include "LzsaCompressor.h"
#include "Compiler/CompilerError.h"
#include <string.h>

extern "C" {
#include <lib.h>
#include <shrink_streaming.h>
#include <shrink_context.h>
}

namespace
{
    struct LZSAStream : public lzsa_stream_t
    {
        std::vector<uint8_t>& data;
        size_t readPos;

        explicit LZSAStream(std::vector<uint8_t>& v)
            : data(v)
            , readPos(0)
        {
            read = lzsaRead;
            write = lzsaWrite;
            eof = lzsaEof;
            close = lzsaClose;
        }

        static size_t lzsaRead(lzsa_stream_t *stream, void *ptr, size_t size)
        {
            auto s = reinterpret_cast<LZSAStream*>(stream);

            size_t bytesAvailable = s->data.size() - s->readPos;
            if (size > bytesAvailable)
                size = bytesAvailable;

            memcpy(ptr, s->data.data() + s->readPos, size);
            s->readPos += size;

            return size;
        }

        static size_t lzsaWrite(lzsa_stream_t *stream, void *ptr, size_t size)
        {
            auto s = reinterpret_cast<LZSAStream*>(stream);
            s->data.insert(s->data.end(), reinterpret_cast<uint8_t*>(ptr), reinterpret_cast<uint8_t*>(ptr) + size);
            return size;
        }

        static int lzsaEof(lzsa_stream_t *stream)
        {
            auto s = reinterpret_cast<LZSAStream*>(stream);
            return s->readPos >= s->data.size();
        }

        static void lzsaClose(lzsa_stream_t* stream)
        {
        }
    };
}

void LzsaCompressor::compress(SourceLocation* location, std::vector<uint8_t> src, std::vector<uint8_t>& dst)
{
    if (src.empty())
        return;

    LZSAStream streamIn(src);
    LZSAStream streamOut(dst);

    long long originalSize = 0, compressedSize = 0;
    int commandCount = 0, safeDist = 0;
    lzsa_stats stats;

    int flags = LZSA_FLAG_FAVOR_RATIO | LZSA_FLAG_RAW_BLOCK;
    int version = 2;
    lzsa_status_t status = lzsa_compress_stream(&streamIn, &streamOut, nullptr, 0,
        flags, 0, version, nullptr, &originalSize, &compressedSize, &commandCount, &safeDist, &stats);

    switch (status) {
        case LZSA_OK: break;
        case LZSA_ERROR_SRC: throw CompilerError(location, "lzsa: error reading input.");
        case LZSA_ERROR_DST: throw CompilerError(location, "lzsa: error writing output.");
        case LZSA_ERROR_DICTIONARY: throw CompilerError(location, "lzsa: error reading dictionary.");
        case LZSA_ERROR_MEMORY: throw CompilerError(location, "lzsa: out of memory.");
        case LZSA_ERROR_COMPRESSION: throw CompilerError(location, "lzsa: internal compression error.");
        case LZSA_ERROR_RAW_TOOLARGE:
            throw CompilerError(location, "lzsa: raw blocks can only be used with files <= 64 Kb.");
        case LZSA_ERROR_RAW_UNCOMPRESSED:
            throw CompilerError(location, "lzsa: incompressible data needs to be <= 64 Kb in raw blocks.");
        default: throw CompilerError(location, "lzsa: unknown compression error."); break;
    }
}
