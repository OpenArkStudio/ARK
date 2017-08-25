/*****************************************************************************
// * This source file is part of ArkGameFrame                                *
// * For the latest info, see https://github.com/ArkGame                     *
// *                                                                         *
// * Copyright(c) 2013 - 2017 ArkGame authors.                               *
// *                                                                         *
// * Licensed under the Apache License, Version 2.0 (the "License");         *
// * you may not use this file except in compliance with the License.        *
// * You may obtain a copy of the License at                                 *
// *                                                                         *
// *     http://www.apache.org/licenses/LICENSE-2.0                          *
// *                                                                         *
// * Unless required by applicable law or agreed to in writing, software     *
// * distributed under the License is distributed on an "AS IS" BASIS,       *
// * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.*
// * See the License for the specific language governing permissions and     *
// * limitations under the License.                                          *
// *                                                                         *
// *                                                                         *
// * @FileName  AFIPropertyConfigModule.h                                                *
// * @Author    Ark Game Tech                                                *
// * @Date      2015-12-15                                                   *
// * @Module    AFIPropertyConfigModule                                                  *
// * @email     ArkGameTech@163.com                                          *
// * @brief     AFIPropertyConfigModule                                                  *
*****************************************************************************/
#ifndef AFI_PROPERTY_COAFIG_MODULE_H
#define AFI_PROPERTY_COAFIG_MODULE_H

#include <iostream>
#include "AFIModule.h"

class AFIPropertyConfigModule
    : public AFIModule
{
public:

    virtual int CalculateBaseValue(const int nJob,  const int nLevel, const std::string& strProperty) = 0;
    virtual bool LegalLevel(const int nJob, const int nLevel) = 0;
};


#endif

