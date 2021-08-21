//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: codegen.h
//  Created Date: 2021-05-25
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_BACKEND_TIGGER_CODEGEN_H__
#define __SYSYC_BACKEND_TIGGER_CODEGEN_H__

#include <iostream>

#include "misc/logger.h"
#include "middle/quad/instseq.h"

namespace sysyc
{
namespace backend
{
namespace tigger
{
class TiggerCodeGenerator
{
public:
  TiggerCodeGenerator(std::size_t tabsize, misc::Logger &logger) :
    tabsize(tabsize),
    logger(logger),
    indent(0),
    p_instseq(nullptr),
    p_os(nullptr)
  {}

  bool generate(const middle::quad::InstructionList &instseq, std::ostream &os);

private:
  std::string addr2str(const middle::quad::Operand *addr);

  std::string num2str(const middle::quad::Operand *addr);

  std::string label2str(const middle::quad::Operand *addr);

  std::string op2str(const middle::quad::Opcode &op);

  std::ostream &add_indent();

  std::size_t tabsize;
  misc::Logger &logger;

  std::size_t indent;
  const middle::quad::InstructionList *p_instseq;
  std::ostream *p_os;
};

} // namespace tigger

} // namespace backend

} // namespace sysyc



#endif // __SYSYC_BACKEND_TIGGER_CODEGEN_H__
