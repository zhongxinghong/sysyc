//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: node_type.h
//  Created Date: 2021-05-02
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_MIDDLE_AST_NODE_TYPE_H__
#define __SYSYC_MIDDLE_AST_NODE_TYPE_H__

namespace sysyc
{
namespace middle
{
namespace ast
{
enum class ASTNodeType
{
  Program,
  VarDecl,
  VarDef,
  InitValList,
  FuncDef,
  FuncFParamList,
  FuncFParam,
  Block,
  IfElseStmt,
  WhileStmt,
  ControlStmt,
  BinaryExp,
  UnaryExp,
  FuncCall,
  FuncRParamList,
  BareStmt,
  LVal,
  Number,
  PrimType,
};

} // namespace ast

} // namespace middle

} // namespace sysyc

#endif // __SYSYC_MIDDLE_AST_NODE_TYPE_H__
