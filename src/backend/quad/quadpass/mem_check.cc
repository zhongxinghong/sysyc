//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: mem_check.cc
//  Created Date: 2021-05-24
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <string>
#include <unordered_set>

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

bool mem_check(InstructionList &instseq, misc::Logger &logger)
{
  const auto &list = instseq.list;
  std::unordered_set<const Operand *> opnd_set;
  std::size_t lineno = 1;

  for (auto it = list.begin(), e = list.end(); it != e; ++it)
  {
    const auto &inst = *it;

    if (inst->dest && !opnd_set.insert(inst->dest).second)
    {
      logger.error("[mem_check] At line: " + std::to_string(lineno) +
                    ", inst->dest has multiple reference");
      return false;
    }

    if (inst->arg1 && !opnd_set.insert(inst->arg1).second)
    {
      logger.error("[mem_check] At line: " + std::to_string(lineno) +
                    ", inst->arg1 has multiple reference");
      return false;
    }

    if (inst->arg2 && !opnd_set.insert(inst->arg2).second)
    {
      logger.error("[mem_check] At line: " + std::to_string(lineno) +
                    ", inst->arg2 has multiple reference");
      return false;
    }

    lineno++;
  }

  return true;
}

} // namespace pass

} // namespace quad

} // namespace backend

} // namespace sysyc
