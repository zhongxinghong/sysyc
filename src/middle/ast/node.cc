//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: node.cc
//  Created Date: 2021-05-01
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "middle/ast/node.h"

namespace sysyc
{
namespace middle
{
namespace ast
{

void ASTNode::replace(ASTNode **p_ast, ASTNode *repl_ast)
{
  ASTNode *old_ast = *p_ast;
  assert(old_ast != repl_ast);

  *p_ast = repl_ast;
  delete old_ast;
}

void ASTNode::replace(std::vector<ASTNode *> *p_ast_list, std::size_t i,
                      ASTNode *repl_ast)
{
  ASTNode *old_ast = (*p_ast_list)[i];
  assert(old_ast != repl_ast);

  (*p_ast_list)[i] = repl_ast;
  delete old_ast;
}

} // namespace ast

} // namespace middle

} // namespace sysyc

namespace std
{
std::string to_string(const sysyc::middle::ast::BinaryExpASTNode::Operator &op)
{
  using BinaryOp = sysyc::middle::ast::BinaryExpASTNode::Operator;

  switch (op)
  {
  case BinaryOp::MUL:     return "*";
  case BinaryOp::DIV:     return "/";
  case BinaryOp::MOD:     return "%";
  case BinaryOp::ADD:     return "+";
  case BinaryOp::SUB:     return "-";
  case BinaryOp::LT:      return "<";
  case BinaryOp::GT:      return ">";
  case BinaryOp::LE:      return "<=";
  case BinaryOp::GE:      return ">=";
  case BinaryOp::EQ:      return "==";
  case BinaryOp::NE:      return "!=";
  case BinaryOp::LAND:    return "&&";
  case BinaryOp::LOR:     return "||";
  case BinaryOp::ASSIGN:  return "=";
  }

  assert(false);
  return "";
}

std::string to_string(const sysyc::middle::ast::UnaryExpASTNode::Operator &op)
{
  using UnaryOp = sysyc::middle::ast::UnaryExpASTNode::Operator;

  switch (op)
  {
  case UnaryOp::POS:      return "+";
  case UnaryOp::NEG:      return "-";
  case UnaryOp::LNOT:     return "!";
  }

  assert(false);
  return "";
}

} // namespace std
