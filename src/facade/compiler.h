//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: compiler.h
//  Created Date: 2021-05-24
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_FACADE_COMPILER_H__
#define __SYSYC_FACADE_COMPILER_H__

#include <fstream>

#include "misc/logger.h"
#include "facade/config.h"
#include "middle/ast/node.h"
#include "middle/quad/instseq.h"

namespace sysyc
{
namespace facade
{
class Compiler
{
public:
  Compiler(const char *input_file, const char *output_file) :
    logger(misc::Logger::Level::DEBUG),
    input_file(input_file),
    output_file(output_file),
    ast(nullptr)
  {}

  Compiler() : Compiler("", "") {}

  ~Compiler();

  bool run_parser();
  bool run_evaluator();
  bool run_ast2quad();
  bool run_ast2quad_pass();
  bool run_regalloc();
  bool run_riscv_pass();

  bool generate_sysy();
  bool generate_eeyore();
  bool generate_tigger();
  bool generate_riscv();

  bool run(const Config &config);

private:
  void report_error(const std::string &errmsg);

  misc::Logger logger;
  std::string input_file;
  std::string output_file;
  std::fstream fin;
  std::fstream fout;
  middle::ast::ASTNode *ast;
  middle::quad::InstructionList instseq;
};

} // namespace facade

} // namespace sysyc

#endif // __SYSYC_FACADE_COMPILER_H__
