//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: codegen.h
//  Created Date: 2021-05-27
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_BACKEND_RISCV_CODEGEN_H__
#define __SYSYC_BACKEND_RISCV_CODEGEN_H__

#include <iostream>

#include "misc/logger.h"
#include "middle/quad/instseq.h"

namespace sysyc
{
namespace backend
{
namespace riscv
{
class RISCVCodeGenerator
{
public:
  RISCVCodeGenerator(std::size_t tabsize, misc::Logger &logger) :
    tabsize(tabsize),
    logger(logger),
    p_instseq(nullptr),
    p_os(nullptr)
  {}

  bool generate(const middle::quad::InstructionList &instseq, std::ostream &os);

private:
  std::string reg2str(const middle::quad::Operand *addr);

  std::string num2str(const middle::quad::Operand *addr);

  std::string regnum2str(const middle::quad::Operand *addr);

  std::string label2str(const middle::quad::Operand *addr);

  std::string addr2str(const middle::quad::Operand *addr);

  std::ostream &add_inst(const std::string &inst);

  std::ostream &add_inst(const std::string &inst, const std::string &arg1);

  std::ostream &add_inst(const std::string &inst, const std::string &arg1,
                         const std::string &arg2);

  std::ostream &add_inst(const std::string &inst, const std::string &arg1,
                         const std::string &arg2, const std::string &arg3);

  std::ostream &add_label(const std::string &label);

  std::size_t tabsize;
  misc::Logger &logger;

  const middle::quad::InstructionList *p_instseq;
  std::ostream *p_os;
};

} // namespace riscv

} // namespace backend

} // namespace sysyc



#endif // __SYSYC_BACKEND_RISCV_CODEGEN_H__
