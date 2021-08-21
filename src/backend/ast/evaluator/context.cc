//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: context.cc
//  Created Date: 2021-05-22
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "backend/ast/evaluator/context.h"

namespace sysyc
{
namespace backend
{
namespace ast
{
namespace __impl
{
using value_t = evaluator_scope_s::value_t;

const value_t *evaluator_scope_s::find(const std::string &id) const
{
  auto it = values.find(id);

  if (it != values.end())
    return &(it->second);

  if (parent == nullptr)
    return nullptr;

  return parent->find(id);
}

value_t *evaluator_scope_s::alloc(const std::string &id)
{
  if (values.find(id) != values.end())
    return nullptr;

  return &values[id];
}

} // namespace __impl

} // namespace ast

} // namespace backend

} // namespace sysyc
