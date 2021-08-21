//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: register.cc
//  Created Date: 2021-05-25
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "middle/quad/register.h"

namespace std
{
std::string to_string(const sysyc::middle::quad::Register &reg)
{
  using namespace sysyc::middle::quad;

  switch (reg)
  {
  case Register::x0:  return "x0";
  case Register::s0:  return "s0";
  case Register::s1:  return "s1";
  case Register::s2:  return "s2";
  case Register::s3:  return "s3";
  case Register::s4:  return "s4";
  case Register::s5:  return "s5";
  case Register::s6:  return "s6";
  case Register::s7:  return "s7";
  case Register::s8:  return "s8";
  case Register::s9:  return "s9";
  case Register::s10: return "s10";
  case Register::s11: return "s11";
  case Register::t0:  return "t0";
  case Register::t1:  return "t1";
  case Register::t2:  return "t2";
  case Register::t3:  return "t3";
  case Register::t4:  return "t4";
  case Register::t5:  return "t5";
  case Register::t6:  return "t6";
  case Register::a0:  return "a0";
  case Register::a1:  return "a1";
  case Register::a2:  return "a2";
  case Register::a3:  return "a3";
  case Register::a4:  return "a4";
  case Register::a5:  return "a5";
  case Register::a6:  return "a6";
  case Register::a7:  return "a7";
  default:
    assert(false);
  }
  return "";
}

} // namespace std
