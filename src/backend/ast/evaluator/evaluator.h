//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: evaluator.h
//  Created Date: 2021-05-06
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_BACKEND_AST_EVALUATOR_EVALUATOR_H__
#define __SYSYC_BACKEND_AST_EVALUATOR_EVALUATOR_H__

#include "misc/logger.h"
#include "middle/ast/analyzer.h"
#include "backend/ast/evaluator/context.h"

namespace sysyc
{
namespace backend
{
namespace ast
{
class ASTEvaluator : public middle::ast::IASTAnalyzer
{
public:
  ASTEvaluator(misc::Logger &logger);

  const std::size_t &get_errcnt() { return errcnt; }

private:
  void report_error(const std::string &errmsg);

  void push_scope();
  void pop_scope();

  misc::Logger &logger;
  std::size_t errcnt;

  __impl::evaluator_scope_s *env;

  bool has_prebuilt_env;
  sysystd::int_t eval_result;

public:
  bool analyze(middle::ast::ProgramASTNode *root) override;
  bool analyze(middle::ast::VarDeclASTNode *root) override;
  bool analyze(middle::ast::VarDefASTNode *root) override;
  bool analyze(middle::ast::InitValListASTNode *root) override;
  bool analyze(middle::ast::FuncDefASTNode *root) override;
  bool analyze(middle::ast::FuncFParamListASTNode *root) override;
  bool analyze(middle::ast::FuncFParamASTNode *root) override;
  bool analyze(middle::ast::BlockASTNode *root) override;
  bool analyze(middle::ast::IfElseStmtASTNode *root) override;
  bool analyze(middle::ast::WhileStmtASTNode *root) override;
  bool analyze(middle::ast::ControlStmtASTNode *root) override;
  bool analyze(middle::ast::BinaryExpASTNode *root) override;
  bool analyze(middle::ast::UnaryExpASTNode *root) override;
  bool analyze(middle::ast::FuncCallASTNode *root) override;
  bool analyze(middle::ast::FuncRParamListASTNode *root) override;
  bool analyze(middle::ast::BareStmtASTNode *root) override;
  bool analyze(middle::ast::LValASTNode *root) override;
  bool analyze(middle::ast::NumberASTNode *root) override;
  bool analyze(middle::ast::PrimTypeASTNode *root) override;
};

} // namespace ast

} // namespace backend

} // namespace sysyc

#endif // __SYSYC_BACKEND_AST_EVALUATOR_EVALUATOR_H__
