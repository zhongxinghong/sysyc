//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: quadpass.h
//  Created Date: 2021-05-29
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_BACKEND_QUAD_QUADPASS_QUADPASS_H__
#define __SYSYC_BACKEND_QUAD_QUADPASS_QUADPASS_H__

#include "misc/logger.h"
#include "middle/quad/instseq.h"

namespace sysyc
{
namespace backend
{
namespace quad
{
namespace pass
{
bool mem_check(middle::quad::InstructionList &instseq, misc::Logger &logger);

bool reorder_decl(middle::quad::InstructionList &instseq, misc::Logger &logger);

bool reduce_label(middle::quad::InstructionList &instseq, misc::Logger &logger);

bool adjust_expr(middle::quad::InstructionList &instseq, misc::Logger &logger);

bool pow2_to_logical(middle::quad::InstructionList &instseq, misc::Logger &logger);

} // namespace pass

} // namespace quad

} // namespace backend

} // namespace sysyc

#endif // __SYSYC_BACKEND_QUAD_QUADPASS_QUADPASS_H__
