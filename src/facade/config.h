//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: config.h
//  Created Date: 2021-05-24
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_FACADE_CONFIG_H__
#define __SYSYC_FACADE_CONFIG_H__

#include <string>

namespace sysyc
{
namespace facade
{
struct Config
{
  enum class Target { SYSY, EEYORE, TIGGER, RISCV };

  std::string input_file;
  std::string output_file;
  Target target;
};

} // namespace facade

} // namespace sysyc

#endif // __SYSYC_FACADE_CONFIG_H__
