//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: utils.hpp
//  Created Date: 2021-05-17
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <fstream>

namespace sysyc
{
namespace test
{

bool check_argc(int argc, int expect)
{
  if (argc != expect)
  {
    std::cerr << "Incorrect number of arguments, expect: " << expect
              << ", current: " << argc << std::endl;
    return false;
  }
  return true;
}

} // namespace test

} // namespace sysyc
