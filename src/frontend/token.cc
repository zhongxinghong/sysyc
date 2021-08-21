//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: token.cc
//  Created Date: 2021-04-29
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <unordered_map>

#include "frontend/token.h"

#define __sysyc_kw_e2_b21b(a1, a2) {(a2), Keyword::a1},
#define __sysyc_kw_e2_b12b(a1, a2) {Keyword::a1, (a2)},
#define __sysyc_op_e2_b21b(a1, a2) {(a2), Operator::a1},
#define __sysyc_op_e2_b12b(a1, a2) {Operator::a1, (a2)},

namespace sysyc
{
namespace frontend
{
namespace __impl
{

bool kw_str2enum(const std::string &s, Keyword *kw)
{
  static const std::unordered_map<std::string, Keyword> table = \
    { SYSYC_SYSY_KEYWORD_TABLE(__sysyc_kw_e2_b21b) };

  auto it = table.find(s);

  if (it == table.end())
    return false;

  *kw = it->second;
  return true;
}

bool kw_enum2str(const Keyword &kw, std::string *s)
{
  static const std::unordered_map<Keyword, std::string> table = \
    { SYSYC_SYSY_KEYWORD_TABLE(__sysyc_kw_e2_b12b) };

  auto it = table.find(kw);

  if (it == table.end())
    return false;

  *s = it->second;
  return true;
}

bool op_str2enum(const std::string &s, Operator *op)
{
  static const std::unordered_map<std::string, Operator> table = \
    { SYSYC_SYSY_OPERATOR_TABLE(__sysyc_op_e2_b21b) };

  auto it = table.find(s);

  if (it == table.end())
    return false;

  *op = it->second;
  return true;
}

bool op_enum2str(const Operator &op, std::string *s)
{
  static const std::unordered_map<Operator, std::string> table = \
    { SYSYC_SYSY_OPERATOR_TABLE(__sysyc_op_e2_b12b) };

  auto it = table.find(op);

  if (it == table.end())
    return false;

  *s = it->second;
  return true;
}

} // namespace __impl

} // namespace frontend

} // namespace sysyc

#undef __sysyc_kw_e2_b21b
#undef __sysyc_kw_e2_b12b
#undef __sysyc_op_e2_b21b
#undef __sysyc_op_e2_b12b
