//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: unit-evaluator.cc
//  Created Date: 2021-05-15
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include "utils.hpp"
#include "facade/compiler.h"

int main(int argc, char const *argv[])
{
  if (!sysyc::test::check_argc(argc, 3))
    return EXIT_FAILURE;

  sysyc::facade::Compiler compiler(argv[1], argv[2]);

  if (!compiler.run_parser())
    return EXIT_FAILURE;

  if (!compiler.run_evaluator())
    return EXIT_FAILURE;

  if (!compiler.generate_sysy())
    return EXIT_FAILURE;

  return EXIT_SUCCESS;
}
