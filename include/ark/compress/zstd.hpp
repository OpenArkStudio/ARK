/*
 * This source file is part of ARK
 * For the latest info, see https://github.com/ArkNX
 *
 * Copyright (c) 2013-2020 ArkNX authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"),
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#pragma once

#include <sys/stat.h>
#include <sys/types.h>

#include <zstd.h>

#include "ark/macros.hpp"

namespace ark {

#define PRE_ALLOCATED_ZSTD_BUFF_SIZE (1024 * 1024)

/**
 * zstd usage and some functions
 * 1. simple compress for normal using, because we don't need to generate dict for every data type.
 * 2. different big data use different dict, you need generate those dicts using the commands below first.
 * (Plz see the zstd readme to find how to generate dict)
 * TODO: 3. add streaming compress for some situations.
 */

class zstd_compress final
{
private:
    /// compress dict list
    std::map<uint16_t, ZSTD_CDict* const> compress_dicts_;

    /// decompress dict list
    std::map<uint16_t, ZSTD_DDict* const> decompress_dicts_;

    ZSTD_CCtx* compress_ctx_{nullptr};
    ZSTD_DCtx* decompress_ctx_{nullptr};

public:
    zstd_compress() = default;

    ~zstd_compress() {
        ZSTD_freeCCtx(compress_ctx_);
        ZSTD_freeDCtx(decompress_ctx_);
        for (auto& [type, c_dict] : compress_dicts_)
        {
            ZSTD_freeCDict(c_dict);
        }

        for (auto& [type, d_dict] : decompress_dicts_)
        {
            ZSTD_freeDDict(d_dict);
        }
    }

    /// simple compress and decompress
    bool normal_compress(
        IN const char* in_data, IN std::size_t in_size, IN uint8_t compress_level, OUT std::string& compressed_buff) {
        static char pre_buffer[PRE_ALLOCATED_ZSTD_BUFF_SIZE] = {0};
        memset(pre_buffer, 0, sizeof(pre_buffer));

        size_t const pre_buff_size = ZSTD_compressBound(in_size); // calc needed size
        if (pre_buff_size >= PRE_ALLOCATED_ZSTD_BUFF_SIZE)
        {
            return false;
        }

        std::size_t const compressed_size = ZSTD_compress(pre_buffer, pre_buff_size, in_data, in_size, compress_level);
        ARK_ASSERT_RET_VAL(!ZSTD_isError(compressed_size), false);

        compressed_buff.append(pre_buffer, compressed_size);

        return true;
    }

    bool normal_decompress(IN const char* in_data, IN std::size_t in_size, OUT std::string& decompressed_buff) {
        static char pre_buffer[PRE_ALLOCATED_ZSTD_BUFF_SIZE] = {0};
        memset(pre_buffer, 0, sizeof(pre_buffer));

        unsigned long long const r_size = ZSTD_getFrameContentSize(in_data, in_size);
        ARK_ASSERT_RET_VAL(r_size != ZSTD_CONTENTSIZE_ERROR, false);
        ARK_ASSERT_RET_VAL(r_size != ZSTD_CONTENTSIZE_UNKNOWN, false);

        size_t const decompressed_size = ZSTD_decompress(pre_buffer, r_size, in_data, in_size);
        ARK_ASSERT_RET_VAL(!ZSTD_isError(decompressed_size), false);
        // When zstd knows the content size, it will error if it doesn't match.
        ARK_ASSERT_RET_VAL(decompressed_size == r_size, false);

        decompressed_buff.append(pre_buffer, decompressed_size);
        return true;
    }

    /// dict compress and decompress
    /**
     * Create zstd compress dict and decompress dict from dict file
     */
    bool init_z_std_dict(uint16_t dict_type, std::string const& filename, int c_level) {
        std::size_t dict_size = 0;
        void* const dict_buffer = malloc_and_load_file_or_die(filename.c_str(), &dict_size);

        // compress dict
        {
            ZSTD_CDict* const cdict = ZSTD_createCDict(dict_buffer, dict_size, c_level);
            ARK_ASSERT_RET_VAL(cdict != NULL, false);
            compress_dicts_.insert(std::make_pair(dict_type, cdict));
        }

        // decompress dict
        {
            ZSTD_DDict* const ddict = ZSTD_createDDict(dict_buffer, dict_size);
            ARK_ASSERT_RET_VAL(ddict != NULL, false);
            decompress_dicts_.insert(std::make_pair(dict_type, ddict));
        }

        free(dict_buffer);
        return true;
    }

    bool dict_compress(
        IN uint16_t dict_type, IN const char* in_data, IN std::size_t in_size, OUT std::string& compressed_buff) {
        static char pre_buffer[PRE_ALLOCATED_ZSTD_BUFF_SIZE] = {0};
        memset(pre_buffer, 0, sizeof(pre_buffer));

        size_t const pre_buff_size = ZSTD_compressBound(in_size);
        if (pre_buff_size >= PRE_ALLOCATED_ZSTD_BUFF_SIZE)
        {
            return false;
        }

        if (compress_ctx_ == nullptr)
        {
            compress_ctx_ = ZSTD_createCCtx();
        }

        ARK_ASSERT_RET_VAL(compress_ctx_ != nullptr, false);

        auto iter = compress_dicts_.find(dict_type);
        ARK_ASSERT_RET_VAL(iter != compress_dicts_.end(), false);

        std::size_t const compressed_size =
            ZSTD_compress_usingCDict(compress_ctx_, pre_buffer, pre_buff_size, in_data, in_size, iter->second);
        ARK_ASSERT_RET_VAL(!ZSTD_isError(compressed_size), false);

        compressed_buff.append(pre_buffer, compressed_size);
        return true;
    }

    bool dict_decompress(
        IN uint16_t dict_type, IN const char* in_data, IN std::size_t in_size, OUT std::string& decompressed_buff) {
        static char pre_buffer[PRE_ALLOCATED_ZSTD_BUFF_SIZE] = {0};
        memset(pre_buffer, 0, sizeof(pre_buffer));

        unsigned long long const r_size = ZSTD_getFrameContentSize(in_data, in_size);
        ARK_ASSERT_RET_VAL(r_size != ZSTD_CONTENTSIZE_ERROR, false);
        ARK_ASSERT_RET_VAL(r_size != ZSTD_CONTENTSIZE_UNKNOWN, false);

        auto iter = decompress_dicts_.find(dict_type);
        ARK_ASSERT_RET_VAL(iter != decompress_dicts_.end(), false);
        ARK_ASSERT_RET_VAL(iter->second != nullptr, false);

        unsigned const expected_dict_id = ZSTD_getDictID_fromDDict(iter->second);
        unsigned const actual_dict_id = ZSTD_getDictID_fromFrame(in_data, in_size);
        ARK_ASSERT_RET_VAL(actual_dict_id == expected_dict_id, false);

        if (decompress_ctx_ == nullptr)
        {
            decompress_ctx_ = ZSTD_createDCtx();
        }

        ARK_ASSERT_RET_VAL(decompress_ctx_ != nullptr, false);

        std::size_t decompressed_size =
            ZSTD_decompress_usingDDict(decompress_ctx_, pre_buffer, r_size, in_data, in_size, iter->second);
        ARK_ASSERT_RET_VAL(!ZSTD_isError(decompressed_size), false);
        // When zstd knows the content size, it will error if it doesn't match.
        ARK_ASSERT_RET_VAL(decompressed_size == r_size, false);

        decompressed_buff.append(pre_buffer, decompressed_size);
        return true;
    }

    /// streaming compress and decompress

protected:
    /*
     * Define the returned error code from utility functions.
     */
    enum common_error_code
    {
        error_fsize = 1,
        error_fopen = 2,
        error_fclose = 3,
        error_fread = 4,
        error_fwrite = 5,
        error_load_file = 6,
        error_save_file = 7,
        error_malloc = 8,
        error_large_file = 9,
    };

    /*! malloc_orDie() :
     * Allocate memory.
     *
     * @return If successful this function returns a pointer to allo-
     * cated memory.  If there is an error, this function will send that
     * error to stderr and exit.
     */
    static void* malloc_or_die(std::size_t size) {
        void* const buff = malloc(size);
        if (buff)
        {
            return buff;
        }

        /* error */
        perror("malloc");
        exit(common_error_code::error_malloc);
    }

    /*! fsize_orDie() :
     * Get the size of a given file path.
     *
     * @return The size of a given file path.
     */
    static size_t fsize_or_die(const char* filename) {
        struct stat st;
        if (stat(filename, &st) != 0)
        {
            /* error */
            perror(filename);
            exit(common_error_code::error_fsize);
        }

        off_t const file_size = st.st_size;
        size_t const size = (size_t)file_size;
        /* 1. fileSize should be non-negative,
         * 2. if off_t -> size_t type conversion results in discrepancy,
         *    the file size is too large for type size_t.
         */
        if ((file_size < 0) || (file_size != (off_t)size))
        {
            fprintf(stderr, "%s : filesize too large \n", filename);
            exit(common_error_code::error_large_file);
        }

        return size;
    }

    /*! fopen_orDie() :
     * Open a file using given file path and open option.
     *
     * @return If successful this function will return a FILE pointer to an
     * opened file otherwise it sends an error to stderr and exits.
     */
    static FILE* fopen_or_die(const char* filename, const char* instruction) {
        FILE* const in_file = fopen(filename, instruction);
        if (in_file)
        {
            return in_file;
        }
        /* error */
        perror(filename);
        exit(common_error_code::error_fopen);
    }

    /*! loadFile_orDie() :
     * load file into buffer (memory).
     *
     * Note: This function will send an error to stderr and exit if it
     * cannot read data from the given file path.
     *
     * @return If successful this function will load file into buffer and
     * return file size, otherwise it will printout an error to stderr and exit.
     */
    static size_t load_file_or_die(const char* file_name, void* buffer, size_t buffer_size) {
        size_t const file_size = fsize_or_die(file_name);
        ARK_ASSERT_RET_VAL(file_size <= buffer_size, 0);

        FILE* const in_file = fopen_or_die(file_name, "rb");
        size_t const read_size = fread(buffer, 1, file_size, in_file);
        if (read_size != (size_t)file_size)
        {
            fprintf(stderr, "fread: %s : %s \n", file_name, strerror(errno));
            exit(common_error_code::error_fread);
        }
        fclose(in_file); /* can't fail, read only */
        return file_size;
    }

    /*! mallocAndLoadFile_orDie() :
     * allocate memory buffer and then load file into it.
     *
     * Note: This function will send an error to stderr and exit if memory allocation
     * fails or it cannot read data from the given file path.
     *
     * @return If successful this function will return buffer and bufferSize(=fileSize),
     * otherwise it will printout an error to stderr and exit.
     */
    static void* malloc_and_load_file_or_die(const char* file_name, size_t* buffer_size) {
        size_t const file_size = fsize_or_die(file_name);
        *buffer_size = file_size;
        void* const buffer = malloc_or_die(*buffer_size);
        load_file_or_die(file_name, buffer, *buffer_size);
        return buffer;
    }
};

} // namespace ark