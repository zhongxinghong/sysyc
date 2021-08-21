//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: node_type.cc
//  Created Date: 2021-05-02
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include "middle/ast/node.h"

namespace sysyc
{
namespace middle
{
namespace ast
{
ASTNodeType ProgramASTNode::node_type() const
{
  return ASTNodeType::Program;
}

ASTNodeType VarDeclASTNode::node_type() const
{
  return ASTNodeType::VarDecl;
}

ASTNodeType VarDefASTNode::node_type() const
{
  return ASTNodeType::VarDef;
}

ASTNodeType InitValListASTNode::node_type() const
{
  return ASTNodeType::InitValList;
}

ASTNodeType FuncDefASTNode::node_type() const
{
  return ASTNodeType::FuncDef;
}

ASTNodeType FuncFParamListASTNode::node_type() const
{
  return ASTNodeType::FuncFParamList;
}

ASTNodeType FuncFParamASTNode::node_type() const
{
  return ASTNodeType::FuncFParam;
}

ASTNodeType BlockASTNode::node_type() const
{
  return ASTNodeType::Block;
}

ASTNodeType IfElseStmtASTNode::node_type() const
{
  return ASTNodeType::IfElseStmt;
}

ASTNodeType WhileStmtASTNode::node_type() const
{
  return ASTNodeType::WhileStmt;
}

ASTNodeType ControlStmtASTNode::node_type() const
{
  return ASTNodeType::ControlStmt;
}

ASTNodeType BinaryExpASTNode::node_type() const
{
  return ASTNodeType::BinaryExp;
}

ASTNodeType UnaryExpASTNode::node_type() const
{
  return ASTNodeType::UnaryExp;
}

ASTNodeType FuncCallASTNode::node_type() const
{
  return ASTNodeType::FuncCall;
}

ASTNodeType FuncRParamListASTNode::node_type() const
{
  return ASTNodeType::FuncRParamList;
}

ASTNodeType BareStmtASTNode::node_type() const
{
  return ASTNodeType::BareStmt;
}

ASTNodeType LValASTNode::node_type() const
{
  return ASTNodeType::LVal;
}

ASTNodeType NumberASTNode::node_type() const
{
  return ASTNodeType::Number;
}

ASTNodeType PrimTypeASTNode::node_type() const
{
  return ASTNodeType::PrimType;
}

} // namespace ast

} // namespace middle

} // namespace sysyc
