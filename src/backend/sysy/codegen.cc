//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: codegen.cc
//  Created Date: 2021-05-02
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "backend/sysy/codegen.h"

namespace sysyc
{
namespace backend
{
namespace sysy
{
using namespace middle::ast;

std::ostream &SysYCodeGenerator::add_indent()
{
  for (std::size_t i = 0; i < indent; ++i)
    buf << ' ';
  return buf;
}

/**
 * Program       ::= {CompUnit};
 */
bool SysYCodeGenerator::generate(const ProgramASTNode *root)
{
  const auto &cunit_list = root->get_comp_unit_list();

  for (std::size_t i = 0, n = cunit_list.size(); i < n; ++i)
  {
    if (i > 0 && (cunit_list[i]->node_type() == ASTNodeType::FuncDef ||
                  cunit_list[i - 1]->node_type() == ASTNodeType::FuncDef))
      buf << std::endl;

    if (!cunit_list[i]->generate_on(*this))
      return false;

    if (i == cunit_list.size() - 1)
      buf << std::endl;
  }

  return true;
}

/**
 * ConstDecl     ::= "const" BType ConstDef {"," ConstDef} ";";
 * VarDecl       ::= BType VarDef {"," VarDef} ";";
 */
bool SysYCodeGenerator::generate(const VarDeclASTNode *root)
{
  add_indent();

  if (root->is_const())
    buf << "const ";

  if (!root->get_type()->generate_on(*this))
    return false;

  buf << ' ';

  const auto &def_list = root->get_def_list();

  for (std::size_t i = 0, n = def_list.size(); i < n; ++i)
  {
    if (!def_list[i]->generate_on(*this))
      return false;

    if (i != def_list.size() - 1)
      buf << ", ";
  }

  buf << ';' << std::endl;

  return true;
}

/**
 * ConstDef      ::= IDENT {"[" ConstExp "]"} "=" ConstInitVal;
 * VarDef        ::= IDENT {"[" ConstExp "]"}
 *                 | IDENT {"[" ConstExp "]"} "=" InitVal;
 */
bool SysYCodeGenerator::generate(const VarDefASTNode *root)
{
  buf << root->get_id();

  for (auto &expr : root->get_dim_list())
  {
    buf << '[';

    top_binop_prec = 0;
    is_binexp_rhs = false;

    if (!expr->generate_on(*this))
      return false;

    buf << ']';
  }

  if (root->get_init_val() != nullptr)
  {
    buf << " = ";

    if (!root->get_init_val()->generate_on(*this))
      return false;
  }

  return true;
}

/**
 * ConstInitVal  ::= ConstExp | "{" [ConstInitVal {"," ConstInitVal}] "}";
 * InitVal       ::= Exp | "{" [InitVal {"," InitVal}] "}";
 */
bool SysYCodeGenerator::generate(const InitValListASTNode *root)
{
  buf << '{';

  const auto &init_val_list = root->get_init_val_list();

  for (std::size_t i = 0, n = init_val_list.size(); i < n; ++i)
  {
    if (!init_val_list[i]->generate_on(*this))
      return false;

    if (i != init_val_list.size() - 1)
      buf << ", ";
  }

  buf << '}';

  return true;
}

/**
 * FuncDef       ::= FuncType IDENT "(" [FuncFParams] ")" Block;
 */
bool SysYCodeGenerator::generate(const FuncDefASTNode *root)
{
  if (!root->get_type()->generate_on(*this))
    return false;

  buf << ' ' << root->get_id() << '(';

  if (root->get_params() && !root->get_params()->generate_on(*this))
    return false;

  buf << ')' << std::endl;

  if (!root->get_block()->generate_on(*this))
    return false;

  return true;
}

/**
 * FuncFParams   ::= FuncFParam {"," FuncFParam};
 */
bool SysYCodeGenerator::generate(const FuncFParamListASTNode *root)
{
  const auto &param_list = root->get_param_list();

  for (std::size_t i = 0, n = param_list.size(); i < n; ++i)
  {
    if (!param_list[i]->generate_on(*this))
      return false;

    if (i != param_list.size() - 1)
      buf << ", ";
  }

  return true;
}

/**
 * FuncFParam    ::= BType IDENT ["[" "]" {"[" ConstExp "]"}];
 */
bool SysYCodeGenerator::generate(const FuncFParamASTNode *root)
{
  if (!root->get_type()->generate_on(*this))
    return false;

  buf << ' ' << root->get_id();

  for (auto &expr : root->get_dim_list())
  {
    if (expr == nullptr)
    {
      buf << "[]";
      continue;
    }

    buf << '[';

    top_binop_prec = 0;
    is_binexp_rhs = false;

    if (!expr->generate_on(*this))
      return false;

    buf << ']';
  }

  return true;
}

/**
 * Block         ::= "{" {BlockItem} "}";
 * BlockItem     ::= Decl | Stmt;
 */
bool SysYCodeGenerator::generate(const BlockASTNode *root)
{
  add_indent() << '{' << std::endl;
  indent += tabsize;

  for (auto &item : root->get_block_item_list())
  {
    if (!item->generate_on(*this))
      return false;
  }

  indent -= tabsize;
  add_indent() << '}' << std::endl;

  return true;
}

/**
 * IfElseStmt    ::= "if" "(" Cond ")" Stmt ["else" Stmt];
 */
bool SysYCodeGenerator::generate(const IfElseStmtASTNode *root)
{
  const ASTNode *stmt_if = root->get_stmt_if();
  const ASTNode *stmt_else = root->get_stmt_else();
  bool is_stmt_if_Block, is_stmt_else_Block, is_stmt_else_IfElseStmt;

  if (is_elseif)
    is_elseif = false;
  else
    add_indent();

  buf << "if (";

  if (!root->get_cond()->generate_on(*this))
    return false;

  buf << ')' << std::endl;

  is_stmt_if_Block = (stmt_if->node_type() == ASTNodeType::Block);

  if (!is_stmt_if_Block)
    indent += tabsize;

  if (!stmt_if->generate_on(*this))
    return false;

  if (!is_stmt_if_Block)
    indent -= tabsize;

  if (stmt_else == nullptr)
    return true;

  is_stmt_else_IfElseStmt = (stmt_else->node_type() == ASTNodeType::IfElseStmt);
  is_stmt_else_Block = (stmt_else->node_type() == ASTNodeType::Block);

  add_indent() << "else";

  if (is_stmt_else_IfElseStmt)
    buf << ' ';
  else
    buf << std::endl;

  if (!is_stmt_else_Block && !is_stmt_else_IfElseStmt)
    indent += tabsize;

  if (is_stmt_else_IfElseStmt)
    is_elseif = true;

  if (!stmt_else->generate_on(*this))
    return false;

  if (!is_stmt_else_Block && !is_stmt_else_IfElseStmt)
    indent -= tabsize;

  return true;
}

/**
 * WhileStmt     ::= "while" "(" Cond ")" Stmt;
 */
bool SysYCodeGenerator::generate(const WhileStmtASTNode *root)
{
  bool is_stmt_Block = (root->get_stmt()->node_type() == ASTNodeType::Block);

  add_indent() << "while (";

  if (!root->get_cond()->generate_on(*this))
    return false;

  buf << ')' << std::endl;

  if (!is_stmt_Block)
    indent += tabsize;

  if (!root->get_stmt()->generate_on(*this))
    return false;

  if (!is_stmt_Block)
    indent -= tabsize;

  return true;
}

/**
 * ControlStmt   ::= "break" ";"
 *                | "continue" ";"
 *                | "return" [Exp] ";";
 */
bool SysYCodeGenerator::generate(const ControlStmtASTNode *root)
{
  switch (root->get_type())
  {
  case ControlStmtASTNode::Type::BREAK:
  {
    add_indent() << "break;" << std::endl;
    break;
  }
  case ControlStmtASTNode::Type::CONTINUE:
  {
    add_indent() << "continue;" << std::endl;
    break;
  }
  case ControlStmtASTNode::Type::RETURN:
  {
    add_indent() << "return";
    if (root->get_expr())
    {
      buf << ' ';
      if (!root->get_expr()->generate_on(*this))
        return false;
    }
    buf << ';' << std::endl;
    break;
  }
  default:
    assert(false);
  }

  return true;
}

/**
 * AssignStmt    ::= LVal "=" Exp ";";
 * MulExp        ::= UnaryExp {("*" | "/" | "%") UnaryExp};
 * AddExp        ::= MulExp {("+" | "-") MulExp};
 * RelExp        ::= AddExp {("<" | ">" | "<=" | ">=") AddExp};
 * EqExp         ::= RelExp {("==" | "!=") RelExp};
 * LAndExp       ::= EqExp {"&&" EqExp};
 * LOrExp        ::= LAndExp {"||" LAndExp};
 */
bool SysYCodeGenerator::generate(const BinaryExpASTNode *root)
{
  bool is_stmt = (root->get_op() == BinaryExpASTNode::Operator::ASSIGN);
  int cur_prec = root->get_op_prec();
  bool need_parentheses = (
    top_binop_prec != 0 && ((!is_binexp_rhs && cur_prec < top_binop_prec) ||
                            (is_binexp_rhs && cur_prec <= top_binop_prec)));
  // TODO: consider the associative law for operators of the same precedence
  std::string s_op;

  if (is_stmt)
    add_indent();

  switch (root->get_op())
  {
  case BinaryExpASTNode::Operator::ASSIGN:   s_op = "=";     break;
  case BinaryExpASTNode::Operator::ADD:      s_op = "+";     break;
  case BinaryExpASTNode::Operator::SUB:      s_op = "-";     break;
  case BinaryExpASTNode::Operator::MUL:      s_op = "*";     break;
  case BinaryExpASTNode::Operator::DIV:      s_op = "/";     break;
  case BinaryExpASTNode::Operator::MOD:      s_op = "%";     break;
  case BinaryExpASTNode::Operator::LT:       s_op = "<";     break;
  case BinaryExpASTNode::Operator::GT:       s_op = ">";     break;
  case BinaryExpASTNode::Operator::LE:       s_op = "<=";    break;
  case BinaryExpASTNode::Operator::GE:       s_op = ">=";    break;
  case BinaryExpASTNode::Operator::EQ:       s_op = "==";    break;
  case BinaryExpASTNode::Operator::NE:       s_op = "!=";    break;
  case BinaryExpASTNode::Operator::LAND:     s_op = "&&";    break;
  case BinaryExpASTNode::Operator::LOR:      s_op = "||";    break;
  default:
    assert(false);
  }

  if (need_parentheses)
    buf << '(';

  top_binop_prec = cur_prec;
  is_binexp_rhs = false;

  if (!root->get_lhs()->generate_on(*this))
    return false;

  buf << ' ' << s_op << ' ';

  top_binop_prec = cur_prec;
  is_binexp_rhs = true;

  if (!root->get_rhs()->generate_on(*this))
    return false;

  if (need_parentheses)
    buf << ')';

  if (is_stmt)
    buf << ';' << std::endl;

  top_binop_prec = 0;
  is_binexp_rhs = false;

  return true;
}

/**
 * UnaryExp      ::= PrimaryExp | FuncCall | UnaryOp UnaryExp
 */
bool SysYCodeGenerator::generate(const UnaryExpASTNode *root)
{
  bool is_binexp = (root->get_expr()->node_type() == ASTNodeType::BinaryExp);
  std::string s_op;

  switch (root->get_op())
  {
  case UnaryExpASTNode::Operator::POS:   s_op = "";    break;
  case UnaryExpASTNode::Operator::NEG:   s_op = "-";   break;
  case UnaryExpASTNode::Operator::LNOT:  s_op = "!";   break;
  default:
    assert(false);
  }

  buf << s_op;

  if (!s_op.empty() && is_binexp)
  {
    top_binop_prec = 0;
    is_binexp_rhs = false;
    buf << '(';
  }

  if (!root->get_expr()->generate_on(*this))
    return false;

  if (!s_op.empty() && is_binexp)
    buf << ')';

  return true;
}

/**
 * FuncCall      ::= IDENT "(" [FuncRParams] ")";
 */
bool SysYCodeGenerator::generate(const FuncCallASTNode *root)
{
  buf << root->get_id() << '(';

  if (root->get_params() && !root->get_params()->generate_on(*this))
    return false;

  buf << ')';

  return true;
}

/**
 * FuncRParams   ::= Exp {"," Exp};
 */
bool SysYCodeGenerator::generate(const FuncRParamListASTNode *root)
{
  const auto &param_list = root->get_param_list();

  for (std::size_t i = 0, n = param_list.size(); i < n; ++i)
  {
    if (!param_list[i]->generate_on(*this))
      return false;

    if (i != param_list.size() - 1)
      buf << ", ";
  }

  return true;
}

/**
 * BareStmt      ::= [Exp] ";";
 */
bool SysYCodeGenerator::generate(const BareStmtASTNode *root)
{
  add_indent();

  if (root->get_expr() && !root->get_expr()->generate_on(*this))
    return false;

  buf << ';' << std::endl;

  return true;
}

/**
 * LVal          ::= IDENT {"[" Exp "]"};
 */
bool SysYCodeGenerator::generate(const LValASTNode *root)
{
  buf << root->get_id();

  for (auto &expr : root->get_ix_list())
  {
    buf << '[';

    top_binop_prec = 0;
    is_binexp_rhs = false;

    if (!expr->generate_on(*this))
      return false;

    buf << ']';
  }

  return true;
}

/**
 * Number        ::= INT_CONST;
 */
bool SysYCodeGenerator::generate(const NumberASTNode *root)
{
  buf << root->get_val();
  return true;
}

/**
 * BType         ::= "int";
 * FuncType      ::= "void" | "int";
 */
bool SysYCodeGenerator::generate(const PrimTypeASTNode *root)
{
  std::string s_type;

  switch (root->get_type())
  {
  case PrimTypeASTNode::Type::VOID:    s_type = "void";      break;
  case PrimTypeASTNode::Type::INT:     s_type = "int";       break;
  default:
    assert(false);
  }

  buf << s_type;

  return true;
}

} // namespace sysy

} // namespace backend

} // namespace sysyc
