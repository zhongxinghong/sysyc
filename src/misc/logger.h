//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: logger.h
//  Created Date: 2021-04-29
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_MISC_LOGGER_H__
#define __SYSYC_MISC_LOGGER_H__

#include <iostream>
#include <string>

namespace sysyc
{
namespace misc
{
class Logger
{
public:
  enum class Level : int
  {
    DEBUG = 0,
    INFO = 1,
    ERROR = 2,
  };

public:
  Logger(Level level) : level(level) {}
  Logger() : level(Level::DEBUG) {}

  void debug(const std::string &msg);
  void info(const std::string &msg);
  void error(const std::string &msg);

private:
  Level level;
};

} // namespace misc

} // namespace sysyc

#endif // __SYSYC_MISC_LOGGER_H__
