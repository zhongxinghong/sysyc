//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: generator.h
//  Created Date: 2021-05-02
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_MIDDLE_AST_GENERATOR_H__
#define __SYSYC_MIDDLE_AST_GENERATOR_H__

#include "middle/ast/node.h"

namespace sysyc
{
namespace middle
{
namespace ast
{
class IASTGenerator
{
public:
  virtual bool generate(const ProgramASTNode *root) = 0;
  virtual bool generate(const VarDeclASTNode *root) = 0;
  virtual bool generate(const VarDefASTNode *root) = 0;
  virtual bool generate(const InitValListASTNode *root) = 0;
  virtual bool generate(const FuncDefASTNode *root) = 0;
  virtual bool generate(const FuncFParamListASTNode *root) = 0;
  virtual bool generate(const FuncFParamASTNode *root) = 0;
  virtual bool generate(const BlockASTNode *root) = 0;
  virtual bool generate(const IfElseStmtASTNode *root) = 0;
  virtual bool generate(const WhileStmtASTNode *root) = 0;
  virtual bool generate(const ControlStmtASTNode *root) = 0;
  virtual bool generate(const BinaryExpASTNode *root) = 0;
  virtual bool generate(const UnaryExpASTNode *root) = 0;
  virtual bool generate(const FuncCallASTNode *root) = 0;
  virtual bool generate(const FuncRParamListASTNode *root) = 0;
  virtual bool generate(const BareStmtASTNode *root) = 0;
  virtual bool generate(const LValASTNode *root) = 0;
  virtual bool generate(const NumberASTNode *root) = 0;
  virtual bool generate(const PrimTypeASTNode *root) = 0;
};

} // namespace ast

} // namespace middle

} // namespace sysyc

#endif // __SYSYC_MIDDLE_AST_GENERATOR_H__
