//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: naive_context.cc
//  Created Date: 2021-05-26
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "backend/quad/regalloc/naive_context.h"

namespace sysyc
{
namespace backend
{
namespace quad
{
namespace __impl
{
using namespace middle::quad;

naive_regalloc_sybtab_entry_s *naive_regalloc_scope_s::get_vardef(
  const addr_t &addr)
{
  auto it = sybtab.find(addr);

  if (it != sybtab.end())
  {
    assert(it->second.env == this);
    return &(it->second);
  }

  if (parent == nullptr)
    return nullptr;

  return parent->get_vardef(addr);
}

naive_regalloc_sybtab_entry_s *naive_regalloc_scope_s::add_vardef(
  const addr_t &addr)
{
  auto it = sybtab.find(addr);

  if (it != sybtab.end())
    return nullptr;

  auto &entry = sybtab[addr];
  entry.env = this;

  return &entry;
}

} // namespace __impl

} // namespace quad

} // namespace backend

} // namespace sysyc
