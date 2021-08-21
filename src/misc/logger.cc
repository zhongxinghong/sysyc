//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: logger.cc
//  Created Date: 2021-04-29
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include "misc/logger.h"

namespace sysyc
{
namespace misc
{

void Logger::debug(const std::string &msg)
{
  if (level > Level::DEBUG)
    return;

  std::cout << msg << std::endl;
}

void Logger::info(const std::string &msg)
{
  if (level > Level::INFO)
    return;

  std::cout << msg << std::endl;
}

void Logger::error(const std::string &msg)
{
  if (level > Level::ERROR)
    return;

  std::cerr << msg << std::endl;
}

} // namespace misc

} // namespace sysyc
