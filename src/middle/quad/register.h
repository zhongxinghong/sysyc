//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: register.h
//  Created Date: 2021-05-25
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_MIDDLE_QUAD_REGISTER_H__
#define __SYSYC_MIDDLE_QUAD_REGISTER_H__

#include <string>

#include "middle/quad/address.h"

namespace sysyc
{
namespace middle
{
namespace quad
{
enum class Register : addr_t
{
  x0 = 0,                                               // const 0
  s0, s1, s2, s3, s4, s5, s6, s7, s8, s9, s10, s11,     // callee
  t0, t1, t2, t3, t4, t5, t6,                           // caller
  a0, a1, a2, a3, a4, a5, a6, a7,                       // func param
};

enum : std::size_t
{
  REG_ALL_CNT = 28,
  REG_CALLEE_CNT = 12,
  REG_CALLER_CNT = 7,
  REG_FUNC_PARAM_CNT = 8,
};

namespace reginfo
{
inline bool is_a(const addr_t &addr)
{
  return static_cast<addr_t>(Register::a0) <= addr
      && static_cast<addr_t>(Register::a7) >= addr;
}

inline bool is_s(const addr_t &addr)
{
  return static_cast<addr_t>(Register::s0) <= addr
      && static_cast<addr_t>(Register::s11) >= addr;
}

inline bool is_t(const addr_t &addr)
{
  return static_cast<addr_t>(Register::t0) <= addr
      && static_cast<addr_t>(Register::t6) >= addr;
}

inline bool is_a(const Register &reg)
{
  return is_a(static_cast<addr_t>(reg));
}

inline bool is_s(const Register &reg)
{
  return is_s(static_cast<addr_t>(reg));
}

inline bool is_t(const Register &reg)
{
  return is_t(static_cast<addr_t>(reg));
}

} // namespace reginfo

inline Register operator+(const Register &reg, addr_t offset)
{
  return static_cast<Register>(static_cast<addr_t>(reg) + offset);
}

inline bool operator==(const Register &reg, addr_t addr)
{
  return static_cast<addr_t>(reg) == addr;
}

inline bool operator==(addr_t addr, const Register &reg)
{
  return static_cast<addr_t>(reg) == addr;
}

inline bool operator!=(const Register &reg, addr_t addr)
{
  return static_cast<addr_t>(reg) != addr;
}

inline bool operator!=(addr_t addr, const Register &reg)
{
  return static_cast<addr_t>(reg) != addr;
}

} // namespace quad

} // namespace middle

} // namespace sysyc

namespace std
{
std::string to_string(const sysyc::middle::quad::Register &reg);

} // namespace std

#endif // __SYSYC_MIDDLE_QUAD_REGISTER_H__
