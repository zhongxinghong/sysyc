//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: parser.h
//  Created Date: 2021-04-29
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_FRONTEND_PARSER_H__
#define __SYSYC_FRONTEND_PARSER_H__

#include "misc/logger.h"
#include "frontend/lexer.h"
#include "middle/ast/node.h"

namespace sysyc
{
namespace frontend
{
class Parser
{
public:
  Parser(Lexer &lexer, misc::Logger &logger) : logger(logger), lexer(lexer) {}

  middle::ast::ASTNode *generate_ast();

private:
  Token next_token() { return lexer.next_token(); }
  Token peek_nth_token(std::size_t nth) { return lexer.peek_nth_token(nth); }

  bool match_nth_keyword(std::size_t nth, const Keyword &kw);
  bool match_nth_separator(std::size_t nth, const char &sep);
  bool match_nth_operator(std::size_t nth, const Operator &op);

  void report_error(const std::string &errmsg);

  misc::Logger &logger;
  Lexer lexer;

private:
  middle::ast::ASTNode *parse_Program();
  middle::ast::ASTNode *parse_CompUnit();
  middle::ast::ASTNode *parse_Decl();
  middle::ast::ASTNode *parse_BType();
  middle::ast::ASTNode *parse_ConstDecl();
  middle::ast::ASTNode *parse_ConstDef();
  middle::ast::ASTNode *parse_ConstInitVal();
  middle::ast::ASTNode *parse_VarDecl();
  middle::ast::ASTNode *parse_VarDef();
  middle::ast::ASTNode *parse_InitVal();
  middle::ast::ASTNode *parse_FuncDef();
  middle::ast::ASTNode *parse_FuncType();
  middle::ast::ASTNode *parse_FuncFParams();
  middle::ast::ASTNode *parse_FuncFParam();
  middle::ast::ASTNode *parse_Block();
  middle::ast::ASTNode *parse_BlockItem();
  middle::ast::ASTNode *parse_Stmt();
  middle::ast::ASTNode *parse_AssignStmt_BareStmt();
  middle::ast::ASTNode *parse_IfElseStmt();
  middle::ast::ASTNode *parse_WhileStmt();
  middle::ast::ASTNode *parse_ControlStmt();
  middle::ast::ASTNode *parse_Exp();
  middle::ast::ASTNode *parse_Cond();
  middle::ast::ASTNode *parse_ConstExp();
  middle::ast::ASTNode *parse_LVal();
  middle::ast::ASTNode *parse_PrimaryExp();
  middle::ast::ASTNode *parse_Number();
  middle::ast::ASTNode *parse_UnaryExp();
  middle::ast::ASTNode *parse_FuncCall();
  middle::ast::ASTNode *parse_FuncRParams();
  middle::ast::ASTNode *parse_MulExp();
  middle::ast::ASTNode *parse_AddExp();
  middle::ast::ASTNode *parse_RelExp();
  middle::ast::ASTNode *parse_EqExp();
  middle::ast::ASTNode *parse_LAndExp();
  middle::ast::ASTNode *parse_LOrExp();
};

} // namespace frontend

} // namespace sysyc

#endif // __SYSYC_FRONTEND_PARSER_H__
