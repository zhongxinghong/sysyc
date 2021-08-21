//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: pow2_to_logical.cc
//  Created Date: 2021-05-29
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "backend/quad/quadpass/quadpass.h"

namespace sysyc
{
namespace backend
{
namespace quad
{
namespace pass
{
using namespace middle::quad;

namespace __impl
{
inline sysystd::int_t is_pow2(sysystd::int_t x)
{
  return (x & (x - 1)) == 0;
}

inline sysystd::int_t log2_pow2(sysystd::int_t x)
{
  sysystd::int_t e = -1;

  for (; x; x >>= 1)
    ++e;

  return e;
}

} // namespace __impl

bool pow2_to_logical(InstructionList &instseq, misc::Logger &logger)
{
  auto &list = instseq.list;

  for (auto it = list.begin(), e = list.end(); it != e; ++it)
  {
    auto &inst = *it;

    switch (inst->op)
    {
    case Opcode::MUL:
    {
      assert(inst->arg1->is_addr());

      if (inst->arg2->is_num())
      {
        auto val = inst->arg2->val;

        if (val > 0 && __impl::is_pow2(val))
        {
          inst->op = Opcode::SLL;
          inst->arg2->val = __impl::log2_pow2(val);
        }
      }
      break;
    }
    // case Opcode::DIV:
    // {
    //   if (inst->arg2->is_num())
    //   {
    //     assert(inst->arg1->is_addr());

    //     auto val = inst->arg2->val;

    //     if (val > 0 && __impl::is_pow2(val))
    //     {
    //       inst->op = Opcode::SRA;
    //       inst->arg2->val = __impl::log2_pow2(val);
    //     }
    //   }
    //   break;
    // }
    // case Opcode::MOD:
    // {
    //   if (inst->arg2->is_num())
    //   {
    //     assert(inst->arg1->is_addr());

    //     auto val = inst->arg2->val;

    //     if (val > 0 && __impl::is_pow2(val))
    //     {
    //       inst->op = Opcode::AND;
    //       inst->arg2->val = val - 1;
    //     }
    //   }
    //   break;
    // }
    default:
      break;
    }
  }

  return true;
}

} // namespace pass

} // namespace quad

} // namespace backend

} // namespace sysyc


