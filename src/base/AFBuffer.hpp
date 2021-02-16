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

namespace ark {

class AFBuffer final
{
public:
    explicit AFBuffer(size_t buffer_size = 1024 * 512)
    {
        data_ = ARK_NEW_ARRAY(char, buffer_size);
        if (data_ != nullptr)
        {
            data_size_ = buffer_size;
        }
    }

    ~AFBuffer()
    {
        ARK_DELETE_ARRAY(char, data_);
    }

    bool write(const char* data, size_t len)
    {
        bool write_ret = true;

        if (get_write_valid_count() >= len)
        {
            memcpy(get_write_ptr(), data, len);
            add_write_pos(len);
        }
        else
        {
            size_t left_len = data_size_ - get_length();

            if (left_len >= len)
            {
                adjust_to_head();
                write(data, len);
            }
            else
            {
                size_t needLen = len - left_len;

                if (needLen > 0)
                {
                    grow(needLen);
                    write(data, len);
                }
                else
                {
                    write_ret = false;
                }
            }
        }

        return write_ret;
    }

    size_t get_length()
    {
        return write_pos_ - read_pos_;
    }

    char* get_data()
    {
        if (read_pos_ < data_size_)
        {
            return data_ + read_pos_;
        }
        else
        {
            return nullptr;
        }
    }

    void remove_data(size_t value)
    {
        size_t temp = read_pos_ + value;

        if (temp <= data_size_)
        {
            read_pos_ = temp;
        }
    }

private:
    void adjust_to_head()
    {
        size_t len = 0;

        if (read_pos_ <= 0)
        {
            return;
        }

        len = get_length();

        if (len > 0)
        {
            memmove(data_, data_ + read_pos_, len);
        }

        read_pos_ = 0;
        write_pos_ = len;
    }

    void init()
    {
        read_pos_ = 0;
        write_pos_ = 0;
    }

    size_t get_write_pos()
    {
        return write_pos_;
    }

    size_t get_read_pos()
    {
        return read_pos_;
    }

    void add_write_pos(size_t value)
    {
        size_t temp = write_pos_ + value;

        if (temp <= data_size_)
        {
            write_pos_ = temp;
        }
    }

    size_t get_write_valid_count()
    {
        return data_size_ - write_pos_;
    }

    size_t get_size()
    {
        return data_size_;
    }

    char* get_write_ptr()
    {
        if (write_pos_ < data_size_)
        {
            return data_ + write_pos_;
        }
        else
        {
            return nullptr;
        }
    }

    void grow(size_t len)
    {
        size_t n = data_size_ + len;
        char* d = new char[n];

        memcpy(d, data_, get_write_pos());
        data_size_ = n;
        delete[] data_;
        data_ = d;
    }

    char* data_{nullptr};
    size_t data_size_{0};

    size_t write_pos_{0};
    size_t read_pos_{0};
};

} // namespace ark
