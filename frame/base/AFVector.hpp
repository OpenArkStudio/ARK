/*
* This source file is part of ARK
* For the latest info, see https://github.com/ArkNX
*
* Copyright (c) 2013-2019 ArkNX authors.
*
* Licensed under the Apache License, Version 2.0 (the "License");
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

#include "AFPlatform.hpp"

namespace ark
{

    template<typename VALUE, bool is_smart_ptr>
    class AFVectorValueType
    {
    public:
        using value_type = VALUE *;
    };

    template<typename VALUE>
    class AFVectorValueType<VALUE, false>
    {
    public:
        using value_type = VALUE *;
    };

    template<typename VALUE>
    class AFVectorValueType<VALUE, true>
    {
    public:
        using value_type = std::shared_ptr<VALUE>;
    };

    template<typename VALUE, bool is_smart_ptr>
    class AFVectorBase
    {
    public:
        using vector_type = typename std::vector<VALUE>;
        using value_type = typename AFVectorValueType<VALUE, is_smart_ptr>::value_type;
        using reference = value_type &;
        using const_reference = const value_type &;
        using iterator = typename vector_type::iterator;
        using const_iterator = typename vector_type::const_iterator;
        using reverse_iterator = typename vector_type::reverse_iterator;
        using const_reverse_iterator = typename vector_type::const_reverse_iterator;

        AFVectorBase() = default;
        ~AFVectorBase()
        {
            clear();
        }

        std::size_t size() const
        {
            return datas_.size();
        }

        bool empty() const
        {
            return datas_.empty();
        }

        void clear()
        {
            if (!is_smart_ptr)
            {
                for (auto data : datas_)
                {
                    ARK_DELETE(data);
                }
            }

            datas_.clear();
        }

        value_type at(std::size_t index)
        {
            if (index >= size())
            {
                return nullptr;
            }

            return datas_[index];
        }

        value_type operator[](std::size_t index)
        {
            ARK_ASSERT_RET_VAL(index < size())
        }

        void resize(std::size_t new_size)
        {
            if (new_size <= size())
            {
                return;
            }

            datas_.resize(new_size, nullptr);
        }

        std::size_t first_empty()
        {
            std::size_t index = 0;
            for_each(begin(), end(), [](value_type value)
            {
                if (value == nullptr)
                {
                    return index;
                }
                else
                {
                    index++;
                }
            });

            return index;
        }

        void erase(std::size_t index)
        {
            ARK_ASSERT_RET_NONE(index < size());
            auto data = datas_[index];
            ARK_DELETE(data);
            datas_[index] = nullptr;
        }

        void push_back(value_type value)
        {
            std::size_t index = first_empty();
            push_back(index, value);
        }

        void push_back(std::size_t index, value_type value)
        {
            if (index < size())
            {
                datas_[index] = value;
            }
            else
            {
                datas_.emplace_back(value);
            }
        }

        iterator begin() noexcept
        {
            return datas_.begin();
        }

        iterator end() noexcept
        {
            return datas_.end();
        }

        const_iterator cbegin() const noexcept
        {
            return datas_.cbegin();
        }

        const_iterator cend() const noexcept
        {
            return datas_.cbegin();
        }

        reverse_iterator rbegin() noexcept
        {
            return datas_.rbegin();
        }

        reverse_iterator rend() noexcept
        {
            return datas_.rend();
        }

        const_reverse_iterator crbegin() const noexcept
        {
            return datas_.crbegin();
        }

        const_reverse_iterator crend() const noexcept
        {
            return datas_.crbegin();
        }

    private:
        vector_type datas_;
    };

    template<typename VALUE>
    using AFVector = std::vector<VALUE>;

    template<typename VALUE>
    using AFPtrVector = AFVectorBase<VALUE, false>;

    template<typename VALUE>
    using AFSmartPtrVector = AFVectorBase<VALUE, true>;

}