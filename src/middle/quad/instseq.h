//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: instseq.h
//  Created Date: 2021-05-22
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_MIDDLE_QUAD_INSTSEQ_H__
#define __SYSYC_MIDDLE_QUAD_INSTSEQ_H__

#include <list>
#include <unordered_map>

#include "middle/quad/inst.h"

namespace sysyc
{
namespace middle
{
namespace quad
{
struct InstructionList
{
  using iterator = std::list<Instruction *>::iterator;
  using const_iterator = std::list<Instruction *>::const_iterator;

  std::list<Instruction *> list;
  std::unordered_map<std::size_t, std::string> funcaddr_map;
};

} // namespace quad

} // namespace middle

} // namespace sysyc

#endif // __SYSYC_MIDDLE_QUAD_INSTSEQ_H__
