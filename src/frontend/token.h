//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: token.h
//  Created Date: 2021-04-29
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_FRONTEND_TOKEN_H__
#define __SYSYC_FRONTEND_TOKEN_H__

#include <string>

#include "frontend/sysystd.h"

#define __sysyc_kw_e2_1(a1, a2) a1,
#define __sysyc_op_e2_1(a1, a2) a1,

namespace sysyc
{
namespace frontend
{
enum class Token
{
  ID,
  KEYWORD,
  INTEGER,
  OPERATOR,
  SEPARATOR,
  END,
  ERROR,
};

enum class Keyword { SYSYC_SYSY_KEYWORD_TABLE(__sysyc_kw_e2_1) };

enum class Operator { SYSYC_SYSY_OPERATOR_TABLE(__sysyc_op_e2_1) };

namespace __impl
{
bool kw_str2enum(const std::string &s, Keyword *kw);

bool kw_enum2str(const Keyword &kw, std::string *s);

bool op_str2enum(const std::string &s, Operator *op);

bool op_enum2str(const Operator &op, std::string *s);

} // namespace __impl

} // namespace frontend

} // namespace sysyc

#undef __sysyc_kw_e2_1
#undef __sysyc_op_e2_1

#endif // __SYSYC_FRONTEND_TOKEN_H__
