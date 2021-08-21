//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: analyzer.h
//  Created Date: 2021-05-04
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_MIDDLE_AST_ANALYZER_H__
#define __SYSYC_MIDDLE_AST_ANALYZER_H__

#include "middle/ast/node.h"

namespace sysyc
{
namespace middle
{
namespace ast
{
class IASTAnalyzer
{
public:
  virtual bool analyze(ProgramASTNode *root) = 0;
  virtual bool analyze(VarDeclASTNode *root) = 0;
  virtual bool analyze(VarDefASTNode *root) = 0;
  virtual bool analyze(InitValListASTNode *root) = 0;
  virtual bool analyze(FuncDefASTNode *root) = 0;
  virtual bool analyze(FuncFParamListASTNode *root) = 0;
  virtual bool analyze(FuncFParamASTNode *root) = 0;
  virtual bool analyze(BlockASTNode *root) = 0;
  virtual bool analyze(IfElseStmtASTNode *root) = 0;
  virtual bool analyze(WhileStmtASTNode *root) = 0;
  virtual bool analyze(ControlStmtASTNode *root) = 0;
  virtual bool analyze(BinaryExpASTNode *root) = 0;
  virtual bool analyze(UnaryExpASTNode *root) = 0;
  virtual bool analyze(FuncCallASTNode *root) = 0;
  virtual bool analyze(FuncRParamListASTNode *root) = 0;
  virtual bool analyze(BareStmtASTNode *root) = 0;
  virtual bool analyze(LValASTNode *root) = 0;
  virtual bool analyze(NumberASTNode *root) = 0;
  virtual bool analyze(PrimTypeASTNode *root) = 0;
};

} // namespace ast

} // namespace middle

} // namespace sysyc

#endif // __SYSYC_MIDDLE_AST_ANALYZER_H__
