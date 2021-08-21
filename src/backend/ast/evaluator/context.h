//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: context.h
//  Created Date: 2021-05-22
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_BACKEND_AST_EVALUATOR_CONTEXT_H__
#define __SYSYC_BACKEND_AST_EVALUATOR_CONTEXT_H__

#include <vector>
#include <unordered_map>

#include "frontend/sysystd.h"

namespace sysyc
{
namespace backend
{
namespace ast
{
namespace __impl
{
struct evaluator_value_s
{
  evaluator_value_s() : const_(false) {}

  bool is_const() const { return const_; }
  bool is_array() const { return !dim_list.empty(); }

  bool const_;

  sysystd::int_t value;

  std::vector<std::size_t> dim_list;
  std::unordered_map<std::size_t, sysystd::int_t> value_map;
};

struct evaluator_scope_s
{
  using value_t = evaluator_value_s;

  evaluator_scope_s(evaluator_scope_s *parent) : parent(parent) {}

  const value_t *find(const std::string &id) const;
  value_t *alloc(const std::string &id);

  evaluator_scope_s *parent;
  std::unordered_map<std::string, value_t> values;
};

} // namespace __impl

} // namespace ast

} // namespace backend

} // namespace sysyc

#endif // __SYSYC_BACKEND_AST_EVALUATOR_CONTEXT_H__
