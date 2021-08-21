//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: naive_regalloc.h
//  Created Date: 2021-05-26
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_BACKEND_QUAD_REGALLOC_NAIVE_REGALLOC_H__
#define __SYSYC_BACKEND_QUAD_REGALLOC_NAIVE_REGALLOC_H__

#include <vector>

#include "misc/logger.h"
#include "middle/quad/instseq.h"
#include "backend/quad/regalloc/naive_context.h"

namespace sysyc
{
namespace backend
{
namespace quad
{
class NaiveRegisterAllocator
{
public:
  NaiveRegisterAllocator(misc::Logger &logger);

  bool run(middle::quad::InstructionList &instseq);

private:
  using scope_t = __impl::naive_regalloc_scope_s;
  using regtab_entry_t = __impl::naive_regalloc_regtab_entry_s;
  using sybtab_entry_t = __impl::naive_regalloc_sybtab_entry_s;

  void report_error(const std::string &errmsg);

  void push_scope();
  void pop_scope();

  void reg_prealloc(std::size_t reg_cnt);

  middle::quad::addr_t reg_alloc(sybtab_entry_t *entry, bool load);
  middle::quad::addr_t reg_alloc(sysystd::int_t number);

  middle::quad::addr_t opnd2reg_dest(middle::quad::Operand **p_opnd);
  middle::quad::addr_t opnd2reg_argx(middle::quad::Operand **p_opnd);

  void reg_free(middle::quad::addr_t reg);

  void flush_regtab(std::size_t st, std::size_t ed);
  void flush_regtab_number(std::size_t st, std::size_t ed);
  void flush_regtab();
  void flush_regtab_nonlocal();

  void reset_regtab();

  void slfree_analyse();

  scope_t *env;
  std::vector<regtab_entry_t> regtab;
  std::size_t regtab_free;
  InstructionList *p_instseq;
  InstructionList::iterator *p_it;
  misc::Logger &logger;
};

} // namespace quad

} // namespace backend

} // namespace sysyc

#endif // __SYSYC_BACKEND_QUAD_REGALLOC_NAIVE_REGALLOC_H__
