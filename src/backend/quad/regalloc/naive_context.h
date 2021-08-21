//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: naive_context.h
//  Created Date: 2021-05-26
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_BACKEND_QUAD_REGALLOC_NAIVE_CONTEXT_H__
#define __SYSYC_BACKEND_QUAD_REGALLOC_NAIVE_CONTEXT_H__

#include <unordered_map>

#include "middle/quad/address.h"
#include "middle/quad/instseq.h"

namespace sysyc
{
namespace backend
{
namespace quad
{
using namespace middle::quad;

namespace __impl
{
struct naive_regalloc_scope_s;

struct naive_regalloc_sybtab_entry_s
{
  naive_regalloc_sybtab_entry_s() :
    env(nullptr),
    is_array(false),
    reg_addr(0),
    stack_addr(0)
  {}

  naive_regalloc_scope_s *env;
  bool is_array;
  middle::quad::addr_t reg_addr;
  middle::quad::addr_t stack_addr;
};

struct naive_regalloc_scope_s
{
  naive_regalloc_scope_s(naive_regalloc_scope_s *parent)
    : parent(parent), inst_fdef(nullptr), stack_size(0), is_slfree(false) {}

  naive_regalloc_sybtab_entry_s *get_vardef(const middle::quad::addr_t &addr);
  naive_regalloc_sybtab_entry_s *add_vardef(const middle::quad::addr_t &addr);

  naive_regalloc_scope_s *parent;
  middle::quad::Instruction *inst_fdef;
  std::size_t stack_size;
  std::unordered_map<middle::quad::addr_t, naive_regalloc_sybtab_entry_s> sybtab;

  bool is_slfree;
};

struct naive_regalloc_regtab_entry_s
{
  bool is_dirty;
  bool is_number;
  naive_regalloc_sybtab_entry_s *sybtab_entry;

  bool is_used() const { return is_number || sybtab_entry != nullptr; }
};

} // namespace __impl

} // namespace quad

} // namespace backend

} // namespace sysyc

#endif // __SYSYC_BACKEND_QUAD_REGALLOC_NAIVE_CONTEXT_H__
