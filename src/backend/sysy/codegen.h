//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: codegen.h
//  Created Date: 2021-05-02
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_BACKEND_SYSY_CODEGEN_H__
#define __SYSYC_BACKEND_SYSY_CODEGEN_H__

#include <iostream>
#include <sstream>

#include "misc/logger.h"
#include "middle/ast/generator.h"

namespace sysyc
{
namespace backend
{
namespace sysy
{
class SysYCodeGenerator : public middle::ast::IASTGenerator
{
public:
  SysYCodeGenerator(std::size_t tabsize, misc::Logger &logger) :
    tabsize(tabsize),
    logger(logger),
    indent(0),
    is_elseif(false),
    top_binop_prec(0),
    is_binexp_rhs(false)
  {}

  std::ostream &dump(std::ostream &os) { return os << buf.str(); }

private:
  std::ostream &add_indent();

  std::size_t tabsize;
  misc::Logger &logger;

  std::size_t indent;
  bool is_elseif;
  int top_binop_prec;
  bool is_binexp_rhs;

  std::stringstream buf;

public:
  bool generate(const middle::ast::ProgramASTNode *root) override;
  bool generate(const middle::ast::VarDeclASTNode *root) override;
  bool generate(const middle::ast::VarDefASTNode *root) override;
  bool generate(const middle::ast::InitValListASTNode *root) override;
  bool generate(const middle::ast::FuncDefASTNode *root) override;
  bool generate(const middle::ast::FuncFParamListASTNode *root) override;
  bool generate(const middle::ast::FuncFParamASTNode *root) override;
  bool generate(const middle::ast::BlockASTNode *root) override;
  bool generate(const middle::ast::IfElseStmtASTNode *root) override;
  bool generate(const middle::ast::WhileStmtASTNode *root) override;
  bool generate(const middle::ast::ControlStmtASTNode *root) override;
  bool generate(const middle::ast::BinaryExpASTNode *root) override;
  bool generate(const middle::ast::UnaryExpASTNode *root) override;
  bool generate(const middle::ast::FuncCallASTNode *root) override;
  bool generate(const middle::ast::FuncRParamListASTNode *root) override;
  bool generate(const middle::ast::BareStmtASTNode *root) override;
  bool generate(const middle::ast::LValASTNode *root) override;
  bool generate(const middle::ast::NumberASTNode *root) override;
  bool generate(const middle::ast::PrimTypeASTNode *root) override;
};

} // namespace sysy

} // namespace backend

} // namespace sysyc

#endif // __SYSYC_BACKEND_SYSY_CODEGEN_H__
