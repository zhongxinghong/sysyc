//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: regalloc.h
//  Created Date: 2021-05-25
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_BACKEND_QUAD_REGALLOC_REGALLOC_H__
#define __SYSYC_BACKEND_QUAD_REGALLOC_REGALLOC_H__

#include "backend/quad/regalloc/naive_regalloc.h"

namespace sysyc
{
namespace backend
{
namespace quad
{
using RegisterAllocator = NaiveRegisterAllocator;

} // namespace quad

} // namespace backend

} // namespace sysyc

#endif // __SYSYC_BACKEND_QUAD_REGALLOC_REGALLOC_H__
