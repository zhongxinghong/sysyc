//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: adjust_expr.cc
//  Created Date: 2021-05-26
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
sysystd::int_t eval(Opcode op, sysystd::int_t val)
{
  switch (op)
  {
  case Opcode::NEG:   return -val;
  case Opcode::LNOT:  return sysystd::bool2int(!sysystd::int2bool(val));

  case Opcode::JEQZ:  return sysystd::bool2int(val == 0);
  case Opcode::JNEZ:  return sysystd::bool2int(val != 0);

  default:
    assert(false);
  }
  return 0;
}

sysystd::int_t eval(Opcode op, sysystd::int_t lhs, sysystd::int_t rhs)
{
  switch (op)
  {
  case Opcode::ADD:   return lhs + rhs;
  case Opcode::SUB:   return lhs - rhs;
  case Opcode::MUL:   return lhs * rhs;
  case Opcode::DIV:   return lhs / rhs;
  case Opcode::MOD:   return lhs % rhs;

  case Opcode::NE:  case Opcode::JNE:   return sysystd::bool2int(lhs != rhs);
  case Opcode::EQ:  case Opcode::JEQ:   return sysystd::bool2int(lhs == rhs);
  case Opcode::GT:  case Opcode::JGT:   return sysystd::bool2int(lhs > rhs);
  case Opcode::LT:  case Opcode::JLT:   return sysystd::bool2int(lhs < rhs);
  case Opcode::GE:  case Opcode::JGE:   return sysystd::bool2int(lhs >= rhs);
  case Opcode::LE:  case Opcode::JLE:   return sysystd::bool2int(lhs <= rhs);

  case Opcode::LAND:
    return sysystd::bool2int(sysystd::int2bool(lhs) && sysystd::int2bool(rhs));
  case Opcode::LOR:
    return sysystd::bool2int(sysystd::int2bool(lhs) || sysystd::int2bool(rhs));

  default:
    assert(false);
  }
  return 0;
}

inline Opcode inv_relop(const Opcode &op)
{
  switch (op)
  {
  case Opcode::GT:    return Opcode::LT;
  case Opcode::LT:    return Opcode::GT;
  case Opcode::GE:    return Opcode::LE;
  case Opcode::LE:    return Opcode::GE;
  case Opcode::JGT:   return Opcode::JLT;
  case Opcode::JLT:   return Opcode::JGT;
  case Opcode::JGE:   return Opcode::JLE;
  case Opcode::JLE:   return Opcode::JGE;
  default:
    assert(false);
  }
  return static_cast<Opcode>(0);
}

inline void swap_arg12(Instruction *inst)
{
  auto opnd = inst->arg1;
  inst->arg1 = inst->arg2;
  inst->arg2 = opnd;
}

inline bool is_number(const Operand *opnd, sysystd::int_t num)
{
  return opnd->is_num() && opnd->val == num;
}

} // namespace __impl

static bool binexpr_swap_argx(InstructionList &instseq, misc::Logger &logger)
{
  auto &list = instseq.list;

  for (auto it = list.begin(), e = list.end(); it != e; ++it)
  {
    auto &inst = *it;

    switch (inst->op)
    {
    case Opcode::NE:
    case Opcode::EQ:
    case Opcode::JNE:
    case Opcode::JEQ:
    case Opcode::LAND:
    case Opcode::LOR:
    case Opcode::ADD:
    case Opcode::MUL:
    {
      if (inst->arg1->is_num() && !inst->arg2->is_num())
        __impl::swap_arg12(inst);
      break;
    }
    case Opcode::GT:
    case Opcode::LT:
    case Opcode::GE:
    case Opcode::LE:
    case Opcode::JGT:
    case Opcode::JLT:
    case Opcode::JGE:
    case Opcode::JLE:
    {
      if (inst->arg1->is_num() && !inst->arg2->is_num())
      {
        inst->op = __impl::inv_relop(inst->op);
        __impl::swap_arg12(inst);
      }
      break;
    }
    case Opcode::SUB:
    case Opcode::DIV:
    case Opcode::MOD:
      break;
    default:
      break;
    }
  }

  return true;
}

static bool simplify_expr(InstructionList &instseq, misc::Logger &logger)
{
  auto &list = instseq.list;

  for (auto it = list.begin(), e = list.end(); it != e; )
  {
    auto &inst = *it;

    switch (inst->op)
    {
    case Opcode::LNOT:
    case Opcode::NEG:
    {
      if (inst->arg1->is_num())
      {
        auto ans = __impl::eval(inst->op, inst->arg1->val);

        inst->op = Opcode::SWORD;
        inst->arg1->val = ans;
      }
      break;
    }
    case Opcode::NE:
    case Opcode::EQ:
    case Opcode::GT:
    case Opcode::LT:
    case Opcode::GE:
    case Opcode::LE:
    case Opcode::LAND:
    case Opcode::LOR:
    case Opcode::ADD:
    case Opcode::SUB:
    case Opcode::MUL:
    case Opcode::DIV:
    case Opcode::MOD:
    {
      if (inst->arg1->is_num() && inst->arg2->is_num())
      {
        auto ans = __impl::eval(inst->op, inst->arg1->val, inst->arg2->val);

        inst->op = Opcode::SWORD;

        delete inst->arg2;

        inst->arg1->val = ans;
        inst->arg2 = nullptr;
      }
      else if (inst->op == Opcode::LAND)
      {
        if (inst->arg2->is_num() && !(sysystd::int2bool(inst->arg2->val)))
        {
          inst->op = Opcode::SWORD;

          delete inst->arg1;
          delete inst->arg2;

          inst->arg1 = new Operand(Operand::Type::NUM, sysystd::bool2int(false));
          inst->arg2 = nullptr;
        }
      }
      else if (inst->op == Opcode::LOR)
      {
        if (inst->arg2->is_num() && sysystd::int2bool(inst->arg2->val))
        {
          inst->op = Opcode::SWORD;

          delete inst->arg1;
          delete inst->arg2;

          inst->arg1 = new Operand(Operand::Type::NUM, sysystd::bool2int(true));
          inst->arg2 = nullptr;
        }
      }
      else if (inst->op == Opcode::ADD)
      {
        if (__impl::is_number(inst->arg2, 0))
        {
          inst->op = Opcode::SWORD;

          delete inst->arg2;
          inst->arg2 = nullptr;
        }
      }
      else if (inst->op == Opcode::SUB)
      {
        if (__impl::is_number(inst->arg2, 0))
        {
          inst->op = Opcode::SWORD;

          delete inst->arg2;
          inst->arg2 = nullptr;
        }
      }
      else if (inst->op == Opcode::MUL)
      {
        if (__impl::is_number(inst->arg2, 0))
        {
          inst->op = Opcode::SWORD;

          delete inst->arg1;
          delete inst->arg2;

          inst->arg1 = new Operand(Operand::Type::NUM, 0);
          inst->arg2 = nullptr;
        }
        else if (__impl::is_number(inst->arg2, 1))
        {
          inst->op = Opcode::SWORD;

          delete inst->arg2;
          inst->arg2 = nullptr;
        }
        else if (__impl::is_number(inst->arg2, -1))
        {
          inst->op = Opcode::NEG;

          delete inst->arg2;
          inst->arg2 = nullptr;
        }
        // else if (__impl::is_number(inst->arg2, 2))
        // {
        //   inst->op = Opcode::ADD;

        //   delete inst->arg2;

        //   inst->arg2 = new Operand(inst->arg1);
        // }
      }
      else if (inst->op == Opcode::DIV)
      {
        if (__impl::is_number(inst->arg1, 0))
        {
          inst->op = Opcode::SWORD;

          delete inst->arg2;
          inst->arg2 = nullptr;
        }
        else if (__impl::is_number(inst->arg2, 1))
        {
          inst->op = Opcode::SWORD;

          delete inst->arg2;
          inst->arg2 = nullptr;
        }
        else if (__impl::is_number(inst->arg2, -1))
        {
          inst->op = Opcode::NEG;

          delete inst->arg2;
          inst->arg2 = nullptr;
        }
      }
      break;
    }
    case Opcode::JNE:
    case Opcode::JEQ:
    case Opcode::JGT:
    case Opcode::JLT:
    case Opcode::JGE:
    case Opcode::JLE:
    {
      if (inst->arg1->is_num() && inst->arg2->is_num())
      {
        auto ans = __impl::eval(inst->op, inst->arg1->val, inst->arg2->val);

        if (sysystd::int2bool(ans))
        {
          inst->op = Opcode::JMP;

          delete inst->arg1;
          delete inst->arg2;

          inst->arg1 = nullptr;
          inst->arg2 = nullptr;
        }
        else
        {
          delete inst;
          it = list.erase(it);
          continue;
        }
      }
      break;
    }
    case Opcode::JEQZ:
    case Opcode::JNEZ:
    {
      if (inst->arg1->is_num())
      {
        auto ans = __impl::eval(inst->op, inst->arg1->val);

        if (sysystd::int2bool(ans))
        {
          inst->op = Opcode::JMP;

          delete inst->arg1;
          inst->arg1 = nullptr;
        }
        else
        {
          delete inst;
          it = list.erase(it);
          continue;
        }
      }
      break;
    }
    default:
      break;
    }

    it++;
  }

  return true;
}

bool adjust_expr(InstructionList &instseq, misc::Logger &logger)
{
  if (!binexpr_swap_argx(instseq, logger))
    return false;

  if (!simplify_expr(instseq, logger))
    return false;

  return true;
}

} // namespace pass

} // namespace quad

} // namespace backend

} // namespace sysyc
