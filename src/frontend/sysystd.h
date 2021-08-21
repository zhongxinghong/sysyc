//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: sysystd.h
//  Created Date: 2021-04-29
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cstdint>
#include <cstddef>

#ifndef __SYSYC_FRONTEND_SYSYSTD_H__
#define __SYSYC_FRONTEND_SYSYSTD_H__

#define SYSYC_SYSY_KEYWORD_TABLE(e)     \
  e(CONST,      "const")                \
  e(INT,        "int")                  \
  e(VOID,       "void")                 \
  e(IF,         "if")                   \
  e(ELSE,       "else")                 \
  e(WHILE,      "while")                \
  e(BREAK,      "break")                \
  e(CONTINUE,   "continue")             \
  e(RETURN,     "return")

#define SYSYC_SYSY_OPERATOR_TABLE(e)    \
  e(LNOT,       "!")                    \
  e(MUL,        "*")                    \
  e(DIV,        "/")                    \
  e(MOD,        "%")                    \
  e(ADD,        "+")                    \
  e(SUB,        "-")                    \
  e(LT,         "<")                    \
  e(GT,         ">")                    \
  e(LE,         "<=")                   \
  e(GE,         ">=")                   \
  e(EQ,         "==")                   \
  e(NE,         "!=")                   \
  e(LAND,       "&&")                   \
  e(LOR,        "||")                   \
  e(ASSIGN,     "=")

#define SYSYC_SYSY_SEPARATOR "{}[](),;"

namespace sysyc
{
namespace sysystd
{
using int_t = std::int32_t;

inline bool int2bool(int_t val) { return val != 0; }

inline int_t bool2int(bool val) { return val ? 1 : 0; }

inline bool is_int10(int_t val) { return val >= -512 && val <= 511; }

inline bool is_int12(int_t val) { return val >= -2048 && val <= 2047; }

} // namespace sysystd

} // namespace sysyc

#endif // __SYSYC_FRONTEND_SYSYSTD_H__
