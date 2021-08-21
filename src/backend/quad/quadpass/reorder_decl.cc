//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: reorder_decl.cc
//  Created Date: 2021-05-23
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>
#include <stack>

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

static bool reorder_global_stmt(InstructionList &instseq, misc::Logger &logger)
{
  auto &list = instseq.list;
  const auto &fmap = instseq.funcaddr_map;

  auto it_main = list.end();

  for (auto it = list.begin(), e = list.end(); it != e; ++it)
  {
    const auto &inst = *it;

    if (inst->op != Opcode::FDEF)
      continue;

    assert(inst->dest->is_addr());

    auto fit = fmap.find(inst->dest->val);
    assert(fit != fmap.end());

    if (fit->second == "main")
    {
      it_main = it;
      break;
    }
  }

  if (it_main == list.end())
  {
    logger.error("[quad.reorder_global_stmt] function \"main\" is missing");
    return false;
  }

  it_main++;

  bool in_global = true;

  for (auto it = list.begin(), e = list.end(); it != e; ++it)
  {
    const auto &inst = *it;

    if (inst->op == Opcode::FDEF)
    {
      assert(in_global);
      in_global = false;
      continue;
    }

    if (inst->op == Opcode::FEND)
    {
      assert(!in_global);
      in_global = true;
      continue;
    }

    if (in_global && inst->op != Opcode::VDEF)
    {
      list.splice(it_main, list, it++);
      it--;
    }
  }

  return true;
}

static bool reorder_vardecl(InstructionList &instseq, misc::Logger &logger)
{
  auto &list = instseq.list;
  std::stack<InstructionList::iterator> st_fdef;

  st_fdef.push(list.begin());

  for (auto it = list.begin(), e = list.end(); it != e; ++it)
  {
    const auto &inst = *it;

    if (inst->op == Opcode::FDEF)
    {
      st_fdef.push(std::next(it, 1));
      continue;
    }

    if (inst->op == Opcode::FEND)
    {
      assert(!st_fdef.empty());
      st_fdef.pop();
      continue;
    }

    if (inst->op == Opcode::VDEF)
    {
      list.splice(st_fdef.top(), list, it++);
      it--;
    }
  }

  assert(!st_fdef.empty());
  st_fdef.pop();
  assert(st_fdef.empty());

  return true;
}

// static bool reorder_param(InstructionList &instseq, misc::Logger &logger)
// {
//   auto &list = instseq.list;
//   std::vector<InstructionList::iterator> it_param_list;

//   for (auto it = list.begin(), e = list.end(); it != e; ++it)
//   {
//     switch ((*it)->op)
//     {
//     case Opcode::PARAM:
//       it_param_list.push_back(it);
//       break;

//     case Opcode::CALL:
//       for (auto &pit : it_param_list)
//         list.splice(it, list, pit);
//       it_param_list.clear();
//       break;

//     default:
//       break;
//     }
//   }

//   return true;
// }

bool reorder_decl(InstructionList &instseq, misc::Logger &logger)
{
  if (!reorder_global_stmt(instseq, logger))
    return false;

  if (!reorder_vardecl(instseq, logger))
    return false;

  // if (!reorder_param(instseq, logger))
  //   return false;

  return true;
}

} // namespace pass

} // namespace quad

} // namespace backend

} // namespace sysyc
