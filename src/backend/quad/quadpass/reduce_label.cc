//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: reduce_label.cc
//  Created Date: 2021-05-24
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>
#include <vector>
#include <unordered_map>

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

bool reduce_label(InstructionList &instseq, misc::Logger &logger)
{
  auto &list = instseq.list;
  std::unordered_map<label_t, std::vector<Operand *> > mp;

  for (auto it = list.begin(), e = list.end(); it != e; ++it)
  {
    auto &inst = *it;
    switch (inst->op)
    {
    case Opcode::JNE:
    case Opcode::JEQ:
    case Opcode::JGT:
    case Opcode::JLT:
    case Opcode::JGE:
    case Opcode::JLE:
    case Opcode::JEQZ:
    case Opcode::JNEZ:
    case Opcode::JMP:
      mp[inst->dest->val].push_back(inst->dest);
      break;
    default:
      break;
    }
  }

  auto it_label = list.end();

  for (auto it = list.begin(), e = list.end(); it != e; ++it)
  {
    if ((*it)->op != Opcode::LABEL)
      continue;

    if (it_label == list.end() || it != std::next(it_label, 1))
    {
      it_label = it;
      continue;
    }

    label_t label_from = (*it_label)->arg1->val;
    label_t label_to = (*it)->arg1->val;

    auto &vec_label_from = mp[label_from];
    auto &vec_label_to = mp[label_to];

    for (auto &opnd : vec_label_from)
    {
      opnd->val = label_to;
      vec_label_to.push_back(opnd);
    }

    vec_label_from.clear();

    delete (*it_label);
    it_label = list.erase(it_label);
  }

  for (auto it = list.begin(), e = list.end(); it != e; )
  {
    if ((*it)->op == Opcode::LABEL && mp[(*it)->arg1->val].empty())
    {
      delete (*it);
      it = list.erase(it);
      continue;
    }
    it++;
  }

  return true;
}

} // namespace pass

} // namespace quad

} // namespace backend

} // namespace sysyc
