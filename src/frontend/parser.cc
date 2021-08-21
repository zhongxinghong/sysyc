//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: parser.cc
//  Created Date: 2021-04-29
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>
#include <sstream>

#include "frontend/parser.h"

namespace sysyc
{
namespace frontend
{
using namespace middle::ast;

inline bool Parser::match_nth_keyword(std::size_t nth, const Keyword &kw)
{
  return lexer.peek_nth_token(nth) == Token::KEYWORD
      && lexer.peek_nth_kw_val(nth) == kw;
}

inline bool Parser::match_nth_separator(std::size_t nth, const char &sep)
{
  return lexer.peek_nth_token(nth) == Token::SEPARATOR
      && lexer.peek_nth_sep_val(nth) == sep;
}

inline bool Parser::match_nth_operator(std::size_t nth, const Operator &op)
{
  return lexer.peek_nth_token(nth) == Token::OPERATOR
      && lexer.peek_nth_op_val(nth) == op;
}

void Parser::report_error(const std::string &errmsg)
{
  std::stringstream sst;
  sst << "[Parser] At Line " << lexer.get_lineno() << ", Col " << lexer.get_colno()
      << ": " << errmsg;
  logger.error(sst.str());
}

ASTNode *Parser::generate_ast()
{
  next_token(); // init
  return parse_Program();
}

/**
 * Program       ::= {CompUnit};
 */
ASTNode *Parser::parse_Program()
{
  ASTNode *ast_Program = nullptr;
  std::vector<ASTNode *> ast_CompUnit_list;
  ASTNode *ast_CompUnit = nullptr;

  while (peek_nth_token(1) != Token::END)
  {
    if (peek_nth_token(1) == Token::ERROR)
    {
      report_error("In parse_Program, encounter Token::ERROR");
      goto CLEANUP;
    }

    if ((ast_CompUnit = parse_CompUnit()) == nullptr)
    {
      report_error("In parse_Program, expect CompUnit");
      goto CLEANUP;
    }

    ast_CompUnit_list.push_back(ast_CompUnit);
  }

  ast_Program = new ProgramASTNode(ast_CompUnit_list);

CLEANUP:
  if (!ast_Program)
  {
    for (auto &ast : ast_CompUnit_list)
      delete ast;
  }
  return ast_Program;
}

/**
 * CompUnit      ::= Decl | FuncDef;
 * Decl          ::= ConstDecl | VarDecl;
 * ConstDecl     ::= "const" BType ConstDef {"," ConstDef} ";";
 * VarDecl       ::= BType VarDef {"," VarDef} ";";
 * BType         ::= "int";
 * FuncDef       ::= FuncType IDENT "(" [FuncFParams] ")" Block;
 * FuncType      ::= "void" | "int";
 */
ASTNode *Parser::parse_CompUnit()
{
  ASTNode *ast_CompUnit = nullptr;
  ASTNode *ast_Decl = nullptr;
  ASTNode *ast_FuncDef = nullptr;

  if (match_nth_keyword(1, Keyword::CONST))
  {
    goto parse_CompUnit_Decl;
  }
  else if (match_nth_keyword(1, Keyword::VOID))
  {
    goto parse_CompUnit_FuncDef;
  }
  else if (match_nth_separator(3, '('))
  {
    goto parse_CompUnit_FuncDef;
  }
  else
  {
    goto parse_CompUnit_Decl;
  }

parse_CompUnit_Decl:
  if ((ast_Decl = parse_Decl()) == nullptr)
  {
    report_error("In parse_CompUnit, expect Decl");
    goto CLEANUP;
  }
  ast_CompUnit = ast_Decl;
  goto SUCCESS;

parse_CompUnit_FuncDef:
  if ((ast_FuncDef = parse_FuncDef()) == nullptr)
  {
    report_error("In parse_CompUnit, expect FuncDef");
    goto CLEANUP;
  }
  ast_CompUnit = ast_FuncDef;
  goto SUCCESS;

CLEANUP:
  if (!ast_CompUnit)
  {
    delete ast_FuncDef;
    delete ast_Decl;
  }
SUCCESS:
  return ast_CompUnit;
}

/**
 * Decl          ::= ConstDecl | VarDecl;
 * ConstDecl     ::= "const" BType ConstDef {"," ConstDef} ";";
 * VarDecl       ::= BType VarDef {"," VarDef} ";";
 */
ASTNode *Parser::parse_Decl()
{
  ASTNode *ast_Decl = nullptr;
  ASTNode *ast_ConstDecl = nullptr;
  ASTNode *ast_VarDecl = nullptr;

  if (match_nth_keyword(1, Keyword::CONST))
  {
    if ((ast_ConstDecl = parse_ConstDecl()) == nullptr)
    {
      report_error("In parse_Decl, expect ConstDecl");
      goto CLEANUP;
    }
    ast_Decl = ast_ConstDecl;
  }
  else
  {
    if ((ast_VarDecl = parse_VarDecl()) == nullptr)
    {
      report_error("In parse_Decl, expect VarDecl");
      goto CLEANUP;
    }
    ast_Decl = ast_VarDecl;
  }

CLEANUP:
  if (!ast_Decl)
  {
    delete ast_VarDecl;
    delete ast_ConstDecl;
  }
  return ast_Decl;
}

/**
 * BType         ::= "int";
 */
ASTNode *Parser::parse_BType()
{
  ASTNode *ast_BType = nullptr;

  if (peek_nth_token(1) == Token::KEYWORD)
  {
    switch (lexer.kw_val())
    {
    case Keyword::INT:
      next_token(); // eat "int"
      ast_BType = new PrimTypeASTNode(PrimTypeASTNode::Type::INT);
      break;
    default:
      break;
    }
  }

  if (!ast_BType)
    report_error("In parse_BType, expect \"int\"");

  return ast_BType;
}

/**
 * ConstDecl     ::= "const" BType ConstDef {"," ConstDef} ";";
 */
ASTNode *Parser::parse_ConstDecl()
{
  ASTNode *ast_ConstDecl = nullptr;
  ASTNode *ast_BType = nullptr;
  std::vector<ASTNode *> ast_ConstDef_list;
  ASTNode *ast_ConstDef = nullptr;

  if (!match_nth_keyword(1, Keyword::CONST))
  {
    report_error("In parse_ConstDecl, expect \"const\"");
    goto CLEANUP;
  }

  next_token(); // eat "const"

  if ((ast_BType = parse_BType()) == nullptr)
  {
    report_error("In parse_ConstDecl, expect BType");
    goto CLEANUP;
  }

  if ((ast_ConstDef = parse_ConstDef()) == nullptr)
  {
    report_error("In parse_ConstDecl, expect first ConstDef");
    goto CLEANUP;
  }

  ast_ConstDef_list.push_back(ast_ConstDef);

  for (;;)
  {
    if (match_nth_separator(1, ';'))
    {
      next_token(); // eat ';'
      break;
    }

    if (!match_nth_separator(1, ','))
    {
      report_error("In parse_ConstDecl, expect ',' before subsequent ConstDef");
      goto CLEANUP;
    }

    next_token(); // eat ','

    if ((ast_ConstDef = parse_ConstDef()) == nullptr)
    {
      report_error("In parse_ConstDecl, expect subsequent ConstDef");
      goto CLEANUP;
    }

    ast_ConstDef_list.push_back(ast_ConstDef);
  }

  ast_ConstDecl = new VarDeclASTNode(ast_BType, ast_ConstDef_list, true);

CLEANUP:
  if (!ast_ConstDecl)
  {
    for (auto &ast : ast_ConstDef_list)
      delete ast;
    delete ast_BType;
  }
  return ast_ConstDecl;
}

/**
 * ConstDef      ::= IDENT {"[" ConstExp "]"} "=" ConstInitVal;
 */
ASTNode *Parser::parse_ConstDef()
{
  ASTNode *ast_ConstDef = nullptr;
  std::vector<ASTNode *> ast_ConstExp_list;
  ASTNode *ast_ConstInitVal = nullptr;
  ASTNode *ast_ConstExp = nullptr;
  std::string id;

  if (peek_nth_token(1) != Token::ID)
  {
    report_error("In parse_ConstDef, expect IDENT");
    goto CLEANUP;
  }

  id = lexer.id_val();
  next_token(); // eat IDENT

  for (;;)
  {
    if (!match_nth_separator(1, '['))
      break;

    next_token(); // eat '['

    if ((ast_ConstExp = parse_ConstExp()) == nullptr)
    {
      report_error("In parse_ConstDef, expect ConstExp");
      goto CLEANUP;
    }

    ast_ConstExp_list.push_back(ast_ConstExp);

    if (!match_nth_separator(1, ']'))
    {
      report_error("In parse_ConstDef, expect ']' after ConstExp");
      goto CLEANUP;
    }

    next_token(); // eat ']'
  }

  if (!match_nth_operator(1, Operator::ASSIGN))
  {
    report_error("In parse_ConstDef, expect '='");
    goto CLEANUP;
  }

  next_token(); // eat '='

  if ((ast_ConstInitVal = parse_ConstInitVal()) == nullptr)
  {
    report_error("In parse_ConstDef, expect ConstInitVal");
    goto CLEANUP;
  }

  ast_ConstDef = new VarDefASTNode(id, ast_ConstExp_list, ast_ConstInitVal, true);

CLEANUP:
  if (!ast_ConstDef)
  {
    delete ast_ConstInitVal;
    for (auto &ast : ast_ConstExp_list)
      delete ast;
  }
  return ast_ConstDef;
}

/**
 * ConstInitVal  ::= ConstExp | "{" [ConstInitVal {"," ConstInitVal}] "}";
 */
ASTNode *Parser::parse_ConstInitVal()
{
  ASTNode *ast_ConstInitVal_result = nullptr;
  ASTNode *ast_ConstExp = nullptr;
  std::vector<ASTNode *> ast_ConstInitVal_list;
  ASTNode *ast_ConstInitVal = nullptr;

  if (!match_nth_separator(1, '{'))
  {
    if ((ast_ConstExp = parse_ConstExp()) == nullptr)
    {
      report_error("In parse_ConstInitVal, expect ConstExp");
      goto CLEANUP;
    }
    ast_ConstInitVal_result = ast_ConstExp;
  }
  else
  {
    next_token(); // eat '{'

    for (;;)
    {
      if (match_nth_separator(1, '}'))
      {
        next_token(); // eat '}'
        break;
      }

      if (!ast_ConstInitVal_list.empty())
      {
        if (!match_nth_separator(1, ','))
        {
          report_error("In parse_ConstInitVal, expect ',' before subsequent ConstInitVal");
          goto CLEANUP;
        }
        next_token(); // eat ','
      }

      if ((ast_ConstInitVal = parse_ConstInitVal()) == nullptr)
      {
        if (ast_ConstInitVal_list.empty())
          report_error("In parse_ConstInitVal, expect first ConstInitVal");
        else
          report_error("In parse_ConstInitVal, expect subsequent ConstInitVal");

        goto CLEANUP;
      }

      ast_ConstInitVal_list.push_back(ast_ConstInitVal);
    }

    ast_ConstInitVal_result = new InitValListASTNode(ast_ConstInitVal_list, true);
  }

CLEANUP:
  if (!ast_ConstInitVal_result)
  {
    for (auto &ast : ast_ConstInitVal_list)
      delete ast;
    delete ast_ConstExp;
  }
  return ast_ConstInitVal_result;
}

/**
 * VarDecl       ::= BType VarDef {"," VarDef} ";";
 */
ASTNode *Parser::parse_VarDecl()
{
  ASTNode *ast_VarDecl = nullptr;
  ASTNode *ast_BType = nullptr;
  std::vector<ASTNode *> ast_VarDef_list;
  ASTNode *ast_VarDef = nullptr;

  if ((ast_BType = parse_BType()) == nullptr)
  {
    report_error("In parse_VarDecl, expect BType");
    goto CLEANUP;
  }

  if ((ast_VarDef = parse_VarDef()) == nullptr)
  {
    report_error("In parse_VarDecl, expect first VarDef");
    goto CLEANUP;
  }

  ast_VarDef_list.push_back(ast_VarDef);

  for (;;)
  {
    if (match_nth_separator(1, ';'))
    {
      next_token(); // eat ';'
      break;
    }

    if (!match_nth_separator(1, ','))
    {
      report_error("In parse_VarDecl, expect ',' before subsequent VarDef");
      goto CLEANUP;
    }

    next_token(); // eat ','

    if ((ast_VarDef = parse_VarDef()) == nullptr)
    {
      report_error("In parse_VarDecl, expect subsequent VarDef");
      goto CLEANUP;
    }

    ast_VarDef_list.push_back(ast_VarDef);
  }

  ast_VarDecl = new VarDeclASTNode(ast_BType, ast_VarDef_list, false);

CLEANUP:
  if (!ast_VarDecl)
  {
    for (auto &ast : ast_VarDef_list)
      delete ast;
    delete ast_BType;
  }
  return ast_VarDecl;
}

/**
 * VarDef        ::= IDENT {"[" ConstExp "]"}
 *                 | IDENT {"[" ConstExp "]"} "=" InitVal;
 */
ASTNode *Parser::parse_VarDef()
{
  ASTNode *ast_VarDef = nullptr;
  std::vector<ASTNode *> ast_ConstExp_list;
  ASTNode *ast_InitVal = nullptr;
  ASTNode *ast_ConstExp = nullptr;
  std::string id;

  if (peek_nth_token(1) != Token::ID)
  {
    report_error("In parse_VarDef, expect IDENT");
    goto CLEANUP;
  }

  id = lexer.id_val();
  next_token(); // eat IDENT

  for (;;)
  {
    if (!match_nth_separator(1, '['))
      break;

    next_token(); // eat '['

    if ((ast_ConstExp = parse_ConstExp()) == nullptr)
    {
      report_error("In parse_VarDef, expect ConstExp after '['");
      goto CLEANUP;
    }

    ast_ConstExp_list.push_back(ast_ConstExp);

    if (!match_nth_separator(1, ']'))
    {
      report_error("In parse_VarDef, expect ']' after ConstExp");
      goto CLEANUP;
    }

    next_token(); // eat ']'
  }

  if (match_nth_operator(1, Operator::ASSIGN))
  {
    next_token(); // eat '='

    if ((ast_InitVal = parse_InitVal()) == nullptr)
    {
      report_error("In parse_VarDef, expect InitVal after '='");
      goto CLEANUP;
    }
  }

  ast_VarDef = new VarDefASTNode(id, ast_ConstExp_list, ast_InitVal, false);

CLEANUP:
  if (!ast_VarDef)
  {
    delete ast_InitVal;
    for (auto &ast : ast_ConstExp_list)
      delete ast;
  }
  return ast_VarDef;
}

/**
 * InitVal       ::= Exp | "{" [InitVal {"," InitVal}] "}";
 */
ASTNode *Parser::parse_InitVal()
{
  ASTNode *ast_InitVal_result = nullptr;
  ASTNode *ast_Exp = nullptr;
  std::vector<ASTNode *> ast_InitVal_list;
  ASTNode *ast_InitVal = nullptr;

  if (!match_nth_separator(1, '{'))
  {
    if ((ast_Exp = parse_Exp()) == nullptr)
    {
      report_error("In parse_InitVal, expect Exp");
      goto CLEANUP;
    }
    ast_InitVal_result = ast_Exp;
  }
  else
  {
    next_token(); // eat '{'

    for (;;)
    {
      if (match_nth_separator(1, '}'))
      {
        next_token(); // eat '}'
        break;
      }

      if (!ast_InitVal_list.empty())
      {
        if (!match_nth_separator(1, ','))
        {
          report_error("In parse_InitVal, expect ',' before subsequent InitVal");
          goto CLEANUP;
        }
        next_token(); // eat ','
      }

      if ((ast_InitVal = parse_InitVal()) == nullptr)
      {
        if (ast_InitVal_list.empty())
          report_error("In parse_InitVal, expect first InitVal");
        else
          report_error("In parse_InitVal, expect subsequent InitVal");

        goto CLEANUP;
      }

      ast_InitVal_list.push_back(ast_InitVal);
    }

    ast_InitVal_result = new InitValListASTNode(ast_InitVal_list, false);
  }

CLEANUP:
  if (!ast_InitVal_result)
  {
    for (auto &ast : ast_InitVal_list)
      delete ast;
    delete ast_Exp;
  }
  return ast_InitVal_result;
}

/**
 * FuncDef       ::= FuncType IDENT "(" [FuncFParams] ")" Block;
 */
ASTNode *Parser::parse_FuncDef()
{
  ASTNode *ast_FuncDef = nullptr;
  ASTNode *ast_FuncType = nullptr;
  ASTNode *ast_FuncFParams = nullptr;
  ASTNode *ast_Block = nullptr;
  std::string id;

  if ((ast_FuncType = parse_FuncType()) == nullptr)
  {
    report_error("In parse_FuncDef, expect FuncType");
    goto CLEANUP;
  }

  if (peek_nth_token(1) != Token::ID)
  {
    report_error("In parse_FuncDef, expect IDENT");
    goto CLEANUP;
  }

  id = lexer.id_val();
  next_token(); // eat IDENT

  if (!match_nth_separator(1, '('))
  {
    report_error("In parse_FuncDef, expect '('");
    goto CLEANUP;
  }

  next_token(); // eat '('

  if (match_nth_separator(1, ')'))
  {
    next_token(); // eat ')'
  }
  else
  {
    if ((ast_FuncFParams = parse_FuncFParams()) == nullptr)
    {
      report_error("In parse_FuncDef, expect FuncFParams");
      goto CLEANUP;
    }

    if (!match_nth_separator(1, ')'))
    {
      report_error("In parse_FuncDef, expect ')'");
      goto CLEANUP;
    }

    next_token(); // eat ')'
  }

  if ((ast_Block = parse_Block()) == nullptr)
  {
    report_error("In parse_FuncDef, expect Block");
    goto CLEANUP;
  }

  ast_FuncDef = new FuncDefASTNode(ast_FuncType, id, ast_FuncFParams, ast_Block);

CLEANUP:
  if (!ast_FuncDef)
  {
    delete ast_Block;
    delete ast_FuncFParams;
    delete ast_FuncType;
  }
  return ast_FuncDef;
}

/**
 * FuncType      ::= "void" | "int";
 */
ASTNode *Parser::parse_FuncType()
{
  ASTNode *ast_FuncType = nullptr;

  if (peek_nth_token(1) == Token::KEYWORD)
  {
    switch (lexer.kw_val())
    {
    case Keyword::VOID:
      next_token(); // eat "void"
      ast_FuncType = new PrimTypeASTNode(PrimTypeASTNode::Type::VOID);
      break;
    case Keyword::INT:
      next_token(); // eat "int"
      ast_FuncType = new PrimTypeASTNode(PrimTypeASTNode::Type::INT);
      break;
    default:
      break;
    }
  }

  if (!ast_FuncType)
    report_error("In parse_FuncType, expect \"void\" / \"int\"");

  return ast_FuncType;
}

/**
 * FuncFParams   ::= FuncFParam {"," FuncFParam};
 */
ASTNode *Parser::parse_FuncFParams()
{
  ASTNode *ast_FuncFParams = nullptr;
  std::vector<ASTNode *> ast_FuncFParam_list;
  ASTNode *ast_FuncFParam = nullptr;

  if ((ast_FuncFParam = parse_FuncFParam()) == nullptr)
  {
    report_error("In parse_FuncFParams, expect first FuncFParam");
    goto CLEANUP;
  }

  ast_FuncFParam_list.push_back(ast_FuncFParam);

  for (;;)
  {
    if (!match_nth_separator(1, ','))
      break;

    next_token(); // ','

    if ((ast_FuncFParam = parse_FuncFParam()) == nullptr)
    {
      report_error("In parse_FuncFParams, expect subsequent FuncFParam");
      goto CLEANUP;
    }

    ast_FuncFParam_list.push_back(ast_FuncFParam);
  }

  ast_FuncFParams = new FuncFParamListASTNode(ast_FuncFParam_list);

CLEANUP:
  if (!ast_FuncFParams)
  {
    for (auto &ast : ast_FuncFParam_list)
      delete ast;
  }
  return ast_FuncFParams;
}

/**
 * FuncFParam    ::= BType IDENT ["[" "]" {"[" ConstExp "]"}];
 */
ASTNode *Parser::parse_FuncFParam()
{
  ASTNode *ast_FuncFParam = nullptr;
  ASTNode *ast_BType = nullptr;
  std::vector<ASTNode *> ast_ConstExp_list;
  ASTNode *ast_ConstExp = nullptr;
  std::string id;

  if ((ast_BType = parse_BType()) == nullptr)
  {
    report_error("In parse_FuncFParam, expect BType");
    goto CLEANUP;
  }

  id = lexer.id_val();
  next_token(); // eat IDENT

  if (match_nth_separator(1, '['))
  {
    next_token(); // eat '['

    if (!match_nth_separator(1, ']'))
    {
      report_error("In parse_FuncFParam, expect ']' after first '['");
      goto CLEANUP;
    }

    next_token(); // eat ']'

    ast_ConstExp_list.push_back(nullptr); // add nullptr to represent []
  }

  for (;;)
  {
    if (!match_nth_separator(1, '['))
      break;

    next_token(); // eat '['

    if ((ast_ConstExp = parse_ConstExp()) == nullptr)
    {
      report_error("In parse_FuncFParam, expect ConstExp after subsequent '['");
      goto CLEANUP;
    }

    ast_ConstExp_list.push_back(ast_ConstExp);

    if (!match_nth_separator(1, ']'))
    {
      report_error("In parse_FuncFParam, expect ']' after ConstExp");
      goto CLEANUP;
    }

    next_token(); // eat ']'
  }

  ast_FuncFParam = new FuncFParamASTNode(ast_BType, id, ast_ConstExp_list);

CLEANUP:
  if (!ast_FuncFParam)
  {
    for (auto &ast : ast_ConstExp_list)
      delete ast;
    delete ast_BType;
  }
  return ast_FuncFParam;
}

/**
 * Block         ::= "{" {BlockItem} "}";
 */
ASTNode *Parser::parse_Block()
{
  ASTNode *ast_Block = nullptr;
  std::vector<ASTNode *> ast_BlockItem_list;
  ASTNode *ast_BlockItem = nullptr;

  if (!match_nth_separator(1, '{'))
  {
    report_error("In parse_Block, expect '{'");
    goto CLEANUP;
  }

  next_token(); // eat '{'

  for (;;)
  {
    if (match_nth_separator(1, '}'))
    {
      next_token(); // eat '}'
      break;
    }

    if ((ast_BlockItem = parse_BlockItem()) == nullptr)
    {
      report_error("In parse_Block, expect BlockItem");
      goto CLEANUP;
    }

    ast_BlockItem_list.push_back(ast_BlockItem);
  }

  ast_Block = new BlockASTNode(ast_BlockItem_list);

CLEANUP:
  if (!ast_Block)
  {
    for (auto &ast : ast_BlockItem_list)
      delete ast;
  }
  return ast_Block;
}

/**
 * BlockItem     ::= Decl | Stmt;
 * Stmt          ::= LVal "=" Exp ";"
 *                 | [Exp] ";"
 *                 | Block
 *                 | "if" "(" Cond ")" Stmt ["else" Stmt]
 *                 | "while" "(" Cond ")" Stmt
 *                 | "break" ";"
 *                 | "continue" ";"
 *                 | "return" [Exp] ";";
 * Decl          ::= ConstDecl | VarDecl;
 * ConstDecl     ::= "const" BType ConstDef {"," ConstDef} ";";
 * VarDecl       ::= BType VarDef {"," VarDef} ";";
 */
ASTNode *Parser::parse_BlockItem()
{
  ASTNode *ast_BlockItem = nullptr;
  ASTNode *ast_Decl = nullptr;
  ASTNode *ast_Stmt = nullptr;

  if (match_nth_keyword(1, Keyword::CONST) ||
      match_nth_keyword(1, Keyword::INT))
  {
    if ((ast_Decl = parse_Decl()) == nullptr)
    {
      report_error("In parse_BlockItem, expect Decl");
      goto CLEANUP;
    }
    ast_BlockItem = ast_Decl;
  }
  else
  {
    if ((ast_Stmt = parse_Stmt()) == nullptr)
    {
      report_error("In parse_BlockItem, expect Stmt");
      goto CLEANUP;
    }
    ast_BlockItem = ast_Stmt;
  }

CLEANUP:
  if (!ast_BlockItem)
  {
    delete ast_Stmt;
    delete ast_Decl;
  }
  return ast_BlockItem;
}

/**
 * Stmt          ::= AssignStmt | BareStmt | Block | IfElseStmt
 *                 | WhileStmt | ControlStmt;
 * AssignStmt    ::= LVal "=" Exp ";";
 * BareStmt      ::= [Exp] ";";
 * IfElseStmt    ::= "if" "(" Cond ")" Stmt ["else" Stmt];
 * WhileStmt     ::= "while" "(" Cond ")" Stmt;
 * ControlStmt   ::= "break" ";"
 *                 | "continue" ";"
 *                 | "return" [Exp] ";";
 */
ASTNode *Parser::parse_Stmt()
{
  ASTNode *ast_Stmt = nullptr;
  ASTNode *ast_AssignStmt_BareStmt = nullptr;
  ASTNode *ast_Block = nullptr;
  ASTNode *ast_IfElseStmt = nullptr;
  ASTNode *ast_WhileStmt = nullptr;
  ASTNode *ast_ControlStmt = nullptr;

  if (peek_nth_token(1) == Token::KEYWORD)
  {
    switch (lexer.kw_val())
    {
    case Keyword::IF:
    {
      if ((ast_IfElseStmt = parse_IfElseStmt()) == nullptr)
      {
        report_error("In parse_Stmt, expect IfElseStmt");
        goto CLEANUP;
      }
      ast_Stmt = ast_IfElseStmt;
      break;
    }
    case Keyword::WHILE:
    {
      if ((ast_WhileStmt = parse_WhileStmt()) == nullptr)
      {
        report_error("In parse_Stmt, expect WhileStmt");
        goto CLEANUP;
      }
      ast_Stmt = ast_WhileStmt;
      break;
    }
    case Keyword::BREAK:
    case Keyword::CONTINUE:
    case Keyword::RETURN:
    {
      if ((ast_ControlStmt = parse_ControlStmt()) == nullptr)
      {
        report_error("In parse_Stmt, expect ControlStmt");
        goto CLEANUP;
      }
      ast_Stmt = ast_ControlStmt;
      break;
    }
    default:
    {
      std::string s_kw;

      if (!__impl::kw_enum2str(lexer.kw_val(), &s_kw))
      {
        report_error("In parse_Stmt, unable to convert Keyword "
                    + std::to_string(static_cast<int>(lexer.kw_val()))
                    + " to std::string");
        goto CLEANUP;
      }

      report_error("In parse_Stmt, unexpected keyword \"" + s_kw + "\"");
      goto CLEANUP;
    }
    }
  }
  else if (match_nth_separator(1, '{'))
  {
    if ((ast_Block = parse_Block()) == nullptr)
    {
      report_error("In parse_Stmt, expect Block");
      goto CLEANUP;
    }
    ast_Stmt = ast_Block;
  }
  else
  {
    if ((ast_AssignStmt_BareStmt = parse_AssignStmt_BareStmt()) == nullptr)
    {
      report_error("In parse_Stmt, expect AssignStmt / BareStmt");
      goto CLEANUP;
    }
    ast_Stmt = ast_AssignStmt_BareStmt;
  }

CLEANUP:
  if (!ast_Stmt)
  {
    delete ast_ControlStmt;
    delete ast_WhileStmt;
    delete ast_IfElseStmt;
    delete ast_Block;
    delete ast_AssignStmt_BareStmt;
  }
  return ast_Stmt;
}

/**
 * AssignStmt    ::= LVal "=" Exp ";";
 * BareStmt      ::= [Exp] ";";
 */
ASTNode *Parser::parse_AssignStmt_BareStmt()
{
  ASTNode *ast_Stmt_result = nullptr;
  ASTNode *ast_LVal = nullptr;
  ASTNode *ast_Exp = nullptr;

  if (match_nth_separator(1, ';'))
  {
    next_token(); // eat ';'
    goto SUCCESS;
  }

  if (peek_nth_token(1) == Token::ID && !match_nth_separator(2, '('))
  {
    if ((ast_LVal = parse_LVal()) == nullptr)
    {
      report_error("In parse_AssignStmt_BareStmt, expect first LVal / Exp");
      goto CLEANUP;
    }
  }
  else
  {
    if ((ast_Exp = parse_Exp()) == nullptr)
    {
      report_error("In parse_AssignStmt_BareStmt, expect first Exp");
      goto CLEANUP;
    }
  }

  if (match_nth_separator(1, ';'))
  {
    next_token(); // eat ';'

    if (ast_LVal != nullptr)
    {
      ast_Exp = ast_LVal;
      ast_LVal = nullptr;
    }
    goto SUCCESS;
  }

  if (!match_nth_operator(1, Operator::ASSIGN))
  {
    report_error("In parse_AssignStmt_BareStmt, expect '=' after LVal");
    goto CLEANUP;
  }

  next_token(); // eat '='

  if ((ast_Exp = parse_Exp()) == nullptr)
  {
    report_error("In parse_AssignStmt_BareStmt, expect Exp after '='");
    goto CLEANUP;
  }

  if (!match_nth_separator(1, ';'))
  {
    report_error("In parse_AssignStmt_BareStmt, expect ';' after LVal = Exp");
    goto CLEANUP;
  }

  next_token(); // eat ';'

SUCCESS:

  if (ast_LVal != nullptr)
  {
    ast_Stmt_result = new BinaryExpASTNode(
      BinaryExpASTNode::Operator::ASSIGN, ast_LVal, ast_Exp);
  }
  else
  {
    ast_Stmt_result = new BareStmtASTNode(ast_Exp);
  }

CLEANUP:
  if (!ast_Stmt_result)
  {
    delete ast_Exp;
    delete ast_LVal;
  }
  return ast_Stmt_result;
}

/**
 * IfElseStmt    ::= "if" "(" Cond ")" Stmt ["else" Stmt];
 */
ASTNode *Parser::parse_IfElseStmt()
{
  ASTNode *ast_IfElseStmt = nullptr;
  ASTNode *ast_Cond = nullptr;
  ASTNode *ast_Stmt_If = nullptr;
  ASTNode *ast_Stmt_Else = nullptr;

  if (!match_nth_keyword(1, Keyword::IF))
  {
    report_error("In parse_IfElseStmt, expect \"if\"");
    goto CLEANUP;
  }

  next_token(); // eat "if"

  if (!match_nth_separator(1, '('))
  {
    report_error("In parse_IfElseStmt, expect '(' before Cond");
    goto CLEANUP;
  }

  next_token(); // eat '('

  if ((ast_Cond = parse_Cond()) == nullptr)
  {
    report_error("In parse_IfElseStmt, expect Cond");
    goto CLEANUP;
  }

  if (!match_nth_separator(1, ')'))
  {
    report_error("In parse_IfElseStmt, expect ')' after Cond");
    goto CLEANUP;
  }

  next_token(); // eat ')'

  if ((ast_Stmt_If = parse_Stmt()) == nullptr)
  {
    report_error("In parse_IfElseStmt, expect Stmt after \"if\"");
    goto CLEANUP;
  }

  if (match_nth_keyword(1, Keyword::ELSE))
  {
    next_token(); // eat "else"

    if ((ast_Stmt_Else = parse_Stmt()) == nullptr)
    {
      report_error("In parse_IfElseStmt, expect Stmt after \"else\"");
      goto CLEANUP;
    }
  }

  ast_IfElseStmt = new IfElseStmtASTNode(ast_Cond, ast_Stmt_If, ast_Stmt_Else);

CLEANUP:
  if (!ast_IfElseStmt)
  {
    delete ast_Stmt_Else;
    delete ast_Stmt_If;
    delete ast_Cond;
  }
  return ast_IfElseStmt;
}

/**
 * WhileStmt     ::= "while" "(" Cond ")" Stmt;
 */
ASTNode *Parser::parse_WhileStmt()
{
  ASTNode *ast_WhileStmt = nullptr;
  ASTNode *ast_Cond = nullptr;
  ASTNode *ast_Stmt = nullptr;

  if (!match_nth_keyword(1, Keyword::WHILE))
  {
    report_error("In parse_WhileStmt, expect \"while\"");
    goto CLEANUP;
  }

  next_token(); // eat "while"

  if (!match_nth_separator(1, '('))
  {
    report_error("In parse_WhileStmt, expect '(' before Cond");
    goto CLEANUP;
  }

  next_token(); // eat '('

  if ((ast_Cond = parse_Cond()) == nullptr)
  {
    report_error("In parse_WhileStmt, expect Cond");
    goto CLEANUP;
  }

  if (!match_nth_separator(1, ')'))
  {
    report_error("In parse_WhileStmt, expect ')' after Cond");
    goto CLEANUP;
  }

  next_token(); // eat ')'

  if ((ast_Stmt = parse_Stmt()) == nullptr)
  {
    report_error("In parse_WhileStmt, expect Stmt");
    goto CLEANUP;
  }

  ast_WhileStmt = new WhileStmtASTNode(ast_Cond, ast_Stmt);

CLEANUP:
  if (!ast_WhileStmt)
  {
    delete ast_Stmt;
    delete ast_Cond;
  }
  return ast_WhileStmt;
}

/**
 * ControlStmt   ::= "break" ";"
 *                 | "continue" ";"
 *                 | "return" [Exp] ";";
 */
ASTNode *Parser::parse_ControlStmt()
{
  using Type = ControlStmtASTNode::Type;

  ASTNode *ast_Control = nullptr;
  ASTNode *ast_Exp = nullptr;
  Type type;

  if (peek_nth_token(1) != Token::KEYWORD)
  {
    report_error("In parse_ControlStmt, expect KEYWORD");
    goto CLEANUP;
  }

  switch (lexer.kw_val())
  {
  case Keyword::BREAK:    type = Type::BREAK;     break;
  case Keyword::CONTINUE: type = Type::CONTINUE;  break;
  case Keyword::RETURN:   type = Type::RETURN;    break;
  default:
    report_error("In parse_ControlStmt, expect \"break\" / \"continue\" / \"return\"");
    goto CLEANUP;
  }

  next_token(); // eat kw

  if (type == Type::RETURN)
  {
    if (!match_nth_separator(1, ';'))
    {
      if ((ast_Exp = parse_Exp()) == nullptr)
      {
        report_error("In parse_ControlStmt, expect Exp after \"return\"");
        goto CLEANUP;
      }

      if (!match_nth_separator(1, ';'))
      {
        report_error("In parse_ControlStmt, expect ';' after \"return\" Exp");
        goto CLEANUP;
      }

      next_token(); // eat ';'
    }
    else
    {
      next_token(); // eat ';'
    }
  }
  else
  {
    if (!match_nth_separator(1, ';'))
    {
      report_error("In parse_ControlStmt, expect ';' after \"break\" / \"continue\"");
      goto CLEANUP;
    }
    next_token(); // eat ';'
  }

  ast_Control = new ControlStmtASTNode(type, ast_Exp);

CLEANUP:
  if (!ast_Control)
  {
    delete ast_Exp;
  }
  return ast_Control;
}

/**
 * Exp           ::= AddExp;
 */
ASTNode *Parser::parse_Exp()
{
  ASTNode *ast_Exp = nullptr;
  ASTNode *ast_AddExp = nullptr;

  if ((ast_AddExp = parse_AddExp()) == nullptr)
  {
    report_error("In parse_Exp, expect AddExp");
    goto CLEANUP;
  }

  ast_Exp = ast_AddExp;

CLEANUP:
  if (!ast_Exp)
  {
    delete ast_AddExp;
  }
  return ast_Exp;
}

/**
 * Cond          ::= LOrExp;
 */
ASTNode *Parser::parse_Cond()
{
  ASTNode *ast_Cond = nullptr;
  ASTNode *ast_LOrExp = nullptr;

  if ((ast_LOrExp = parse_LOrExp()) == nullptr)
  {
    report_error("In parse_Cond, expect LOrExp");
    goto CLEANUP;
  }

  ast_Cond = ast_LOrExp;

CLEANUP:
  if (!ast_Cond)
  {
    delete ast_LOrExp;
  }
  return ast_Cond;
}

/**
 * ConstExp      ::= AddExp;
 */
ASTNode *Parser::parse_ConstExp()
{
  ASTNode *ast_ConstExp = nullptr;
  ASTNode *ast_AddExp = nullptr;

  if ((ast_AddExp = parse_AddExp()) == nullptr)
  {
    report_error("In parse_ConstExp, expect AddExp");
    goto CLEANUP;
  }

  ast_ConstExp = ast_AddExp;

CLEANUP:
  if (!ast_ConstExp)
  {
    delete ast_AddExp;
  }
  return ast_ConstExp;
}

/**
 * LVal          ::= IDENT {"[" Exp "]"};
 */
ASTNode *Parser::parse_LVal()
{
  ASTNode *ast_LVal = nullptr;
  std::vector<ASTNode *> ast_Exp_list;
  ASTNode *ast_Exp = nullptr;
  std::string id;

  if (peek_nth_token(1) != Token::ID)
  {
    report_error("In parse_LVal, expect IDENT");
    goto CLEANUP;
  }

  id = lexer.id_val();
  next_token(); // eat IDENT

  for (;;)
  {
    if (!match_nth_separator(1, '['))
      break;

    next_token(); // eat '['

    if ((ast_Exp = parse_Exp()) == nullptr)
    {
      report_error("In parse_LVal, expect Exp between \"[]\"");
      goto CLEANUP;
    }

    ast_Exp_list.push_back(ast_Exp);

    if (!match_nth_separator(1, ']'))
    {
      report_error("In parse_LVal, expect ']' after Exp");
      goto CLEANUP;
    }

    next_token(); // eat ']'
  }

  ast_LVal = new LValASTNode(id, ast_Exp_list);

CLEANUP:
  if (!ast_LVal)
  {
    for (auto &ast : ast_Exp_list)
      delete ast;
  }
  return ast_LVal;
}

/**
 * PrimaryExp    ::= "(" Exp ")" | LVal | Number;
 */
ASTNode *Parser::parse_PrimaryExp()
{
  ASTNode *ast_PrimaryExp = nullptr;
  ASTNode *ast_Exp = nullptr;
  ASTNode *ast_LVal = nullptr;
  ASTNode *ast_Number = nullptr;

  if (match_nth_separator(1, '('))
  {
    next_token(); // eat '('

    if ((ast_Exp = parse_Exp()) == nullptr)
    {
      report_error("In parse_PrimaryExp, expect Exp after '('");
      goto CLEANUP;
    }

    if (!match_nth_separator(1, ')'))
    {
      report_error("In parse_PrimaryExp, expect ')' after Exp");
      goto CLEANUP;
    }

    next_token(); // eat ')'

    ast_PrimaryExp = ast_Exp;
  }
  else if (peek_nth_token(1) == Token::INTEGER)
  {
    if ((ast_Number = parse_Number()) == nullptr)
    {
      report_error("In parse_PrimaryExp, expect Number");
      goto CLEANUP;
    }
    ast_PrimaryExp = ast_Number;
  }
  else
  {
    if ((ast_LVal = parse_LVal()) == nullptr)
    {
      report_error("In parse_PrimaryExp, expect LVal");
      goto CLEANUP;
    }
    ast_PrimaryExp = ast_LVal;
  }

CLEANUP:
  if (!ast_PrimaryExp)
  {
    delete ast_Number;
    delete ast_LVal;
    delete ast_Exp;
  }
  return ast_PrimaryExp;
}

/**
 * Number        ::= INT_CONST;
 */
ASTNode *Parser::parse_Number()
{
  if (peek_nth_token(1) == Token::INTEGER)
  {
    auto int_ = lexer.int_val();
    next_token(); // eat INT_CONST

    return new NumberASTNode(int_);
  }

  report_error("In parse_Number, expect INT_CONST");
  return nullptr;
}

/**
 * UnaryExp      ::= PrimaryExp | FuncCall | UnaryOp UnaryExp;
 * UnaryOp       ::= "+" | "-" | "!";
 */
ASTNode *Parser::parse_UnaryExp()
{
  ASTNode *ast_UnaryExp_result = nullptr;
  ASTNode *ast_PrimaryExp = nullptr;
  ASTNode *ast_FuncCall = nullptr;
  ASTNode *ast_UnaryOp = nullptr;
  ASTNode *ast_UnaryExp = nullptr;

  if (peek_nth_token(1) == Token::OPERATOR)
  {
    auto op = lexer.op_val();

    if (op != Operator::ADD && op != Operator::SUB && op != Operator::LNOT)
    {
      report_error("In parse_UnaryExp, expect '+' / '-' / '!' as UnaryOp");
      goto CLEANUP;
    }

    next_token(); // eat op

    if ((ast_UnaryExp = parse_UnaryExp()) == nullptr)
    {
      report_error("In parse_UnaryExp, expect UnaryExp after UnaryOp");
      goto CLEANUP;
    }

    switch (op)
    {
    case Operator::ADD:
      ast_UnaryExp_result = new UnaryExpASTNode(
        UnaryExpASTNode::Operator::POS, ast_UnaryExp);
      break;
    case Operator::SUB:
      ast_UnaryExp_result = new UnaryExpASTNode(
        UnaryExpASTNode::Operator::NEG, ast_UnaryExp);
      break;
    case Operator::LNOT:
      ast_UnaryExp_result = new UnaryExpASTNode(
        UnaryExpASTNode::Operator::LNOT, ast_UnaryExp);
      break;
    default:
      assert(false);
    }
  }
  else if (peek_nth_token(1) != Token::ID || !match_nth_separator(2, '('))
  {
    if ((ast_PrimaryExp = parse_PrimaryExp()) == nullptr)
    {
      report_error("In parse_UnaryExp, expect PrimaryExp");
      goto CLEANUP;
    }
    ast_UnaryExp_result = ast_PrimaryExp;
  }
  else
  {
    if ((ast_FuncCall = parse_FuncCall()) == nullptr)
    {
      report_error("In parse_UnaryExp, expect FuncCall");
      goto CLEANUP;
    }
    ast_UnaryExp_result = ast_FuncCall;
  }

CLEANUP:
  if (!ast_UnaryExp_result)
  {
    delete ast_UnaryExp;
    delete ast_UnaryOp;
    delete ast_FuncCall;
    delete ast_PrimaryExp;
  }
  return ast_UnaryExp_result;
}

/**
 * FuncCall      ::= IDENT "(" [FuncRParams] ")";
 */
ASTNode *Parser::parse_FuncCall()
{
  ASTNode *ast_FuncCall = nullptr;
  ASTNode *ast_FuncRParams = nullptr;
  std::string id;

  if (peek_nth_token(1) != Token::ID)
  {
    report_error("In parse_FuncCall, expect IDENT");
    goto CLEANUP;
  }

  id = lexer.id_val();
  next_token(); // eat IDENT

  if (!match_nth_separator(1, '('))
  {
    report_error("In parse_FuncCall, expect '('");
    goto CLEANUP;
  }

  next_token(); // eat '('

  if (match_nth_separator(1, ')'))
  {
    next_token(); // eat ')'
  }
  else
  {
    if ((ast_FuncRParams = parse_FuncRParams()) == nullptr)
    {
      report_error("In parse_FuncCall, expect FuncRParams");
      goto CLEANUP;
    }

    if (!match_nth_separator(1, ')'))
    {
      report_error("In parse_FuncCall, expect ')' after FuncRParams");
      goto CLEANUP;
    }

    next_token(); // eat ')'
  }

  ast_FuncCall = new FuncCallASTNode(id, ast_FuncRParams);

CLEANUP:
  if (!ast_FuncCall)
  {
    delete ast_FuncRParams;
  }
  return ast_FuncCall;
}

/**
 * FuncRParams   ::= Exp {"," Exp};
 */
ASTNode *Parser::parse_FuncRParams()
{
  ASTNode *ast_FuncRParams = nullptr;
  std::vector<ASTNode *> ast_Exp_list;
  ASTNode *ast_Exp = nullptr;

  if ((ast_Exp = parse_Exp()) == nullptr)
  {
    report_error("In parse_FuncRParams, expect first Exp");
    goto CLEANUP;
  }

  ast_Exp_list.push_back(ast_Exp);

  for (;;)
  {
    if (!match_nth_separator(1, ','))
      break;

    next_token(); // eat ','

    if ((ast_Exp = parse_Exp()) == nullptr)
    {
      report_error("In parse_FuncRParams, expect subsequent Exp");
      goto CLEANUP;
    }

    ast_Exp_list.push_back(ast_Exp);
  }

  ast_FuncRParams = new FuncRParamListASTNode(ast_Exp_list);

CLEANUP:
  if (!ast_FuncRParams)
  {
    for (auto &ast : ast_Exp_list)
      delete ast;
  }
  return ast_FuncRParams;
}

/**
 * MulExp        ::= UnaryExp {("*" | "/" | "%") UnaryExp};
 */
ASTNode *Parser::parse_MulExp()
{
  ASTNode *ast_MulExp = nullptr;
  std::vector<ASTNode *> ast_UnaryExp_list;
  ASTNode *ast_UnaryExp = nullptr;
  std::vector<Operator> op_list;

  if ((ast_UnaryExp = parse_UnaryExp()) == nullptr)
  {
    report_error("In parse_MulExp, expect first UnaryExp");
    goto CLEANUP;
  }

  ast_UnaryExp_list.push_back(ast_UnaryExp);

  for (;;)
  {
    if (peek_nth_token(1) != Token::OPERATOR)
      break;

    auto op = lexer.op_val();

    if (op != Operator::MUL && op != Operator::DIV && op != Operator::MOD)
      break;

    op_list.push_back(op);
    next_token(); // eat op

    if ((ast_UnaryExp = parse_UnaryExp()) == nullptr)
    {
      report_error("In parse_MulExp, expect subsequent UnaryExp");
      goto CLEANUP;
    }

    ast_UnaryExp_list.push_back(ast_UnaryExp);
  }

  ast_MulExp = ast_UnaryExp_list[0];

  for (std::size_t i = 1, n = ast_UnaryExp_list.size(); i < n; ++i)
  {
    switch (op_list[i - 1])
    {
    case Operator::MUL:
      ast_MulExp = new BinaryExpASTNode(
        BinaryExpASTNode::Operator::MUL, ast_MulExp, ast_UnaryExp_list[i]);
      break;
    case Operator::DIV:
      ast_MulExp = new BinaryExpASTNode(
        BinaryExpASTNode::Operator::DIV, ast_MulExp, ast_UnaryExp_list[i]);
      break;
    case Operator::MOD:
      ast_MulExp = new BinaryExpASTNode(
        BinaryExpASTNode::Operator::MOD, ast_MulExp, ast_UnaryExp_list[i]);
      break;
    default:
      assert(false);
    }
  }

CLEANUP:
  if (!ast_MulExp)
  {
    for (auto &ast : ast_UnaryExp_list)
      delete ast;
  }
  return ast_MulExp;
}

/**
 * AddExp        ::= MulExp {("+" | "-") MulExp};
 */
ASTNode *Parser::parse_AddExp()
{
  ASTNode *ast_AddExp = nullptr;
  std::vector<ASTNode *> ast_MulExp_list;
  ASTNode *ast_MulExp = nullptr;
  std::vector<Operator> op_list;

  if ((ast_MulExp = parse_MulExp()) == nullptr)
  {
    report_error("In parse_AddExp, expect first MulExp");
    goto CLEANUP;
  }

  ast_MulExp_list.push_back(ast_MulExp);

  for (;;)
  {
    if (peek_nth_token(1) != Token::OPERATOR)
      break;

    auto op = lexer.op_val();

    if (op != Operator::ADD && op != Operator::SUB)
      break;

    op_list.push_back(op);
    next_token(); // eat op

    if ((ast_MulExp = parse_MulExp()) == nullptr)
    {
      report_error("In parse_AddExp, expect subsequent MulExp");
      goto CLEANUP;
    }

    ast_MulExp_list.push_back(ast_MulExp);
  }

  ast_AddExp = ast_MulExp_list[0];

  for (std::size_t i = 1, n = ast_MulExp_list.size(); i < n; ++i)
  {
    switch (op_list[i - 1])
    {
    case Operator::ADD:
      ast_AddExp = new BinaryExpASTNode(
        BinaryExpASTNode::Operator::ADD, ast_AddExp, ast_MulExp_list[i]);
      break;
    case Operator::SUB:
      ast_AddExp = new BinaryExpASTNode(
        BinaryExpASTNode::Operator::SUB, ast_AddExp, ast_MulExp_list[i]);
      break;
    default:
      assert(false);
    }
  }

CLEANUP:
  if (!ast_AddExp)
  {
    for (auto &ast : ast_MulExp_list)
      delete ast;
  }
  return ast_AddExp;
}

/**
 * RelExp        ::= AddExp {("<" | ">" | "<=" | ">=") AddExp};
 */
ASTNode *Parser::parse_RelExp()
{
  ASTNode *ast_RelExp = nullptr;
  std::vector<ASTNode *> ast_AddExp_list;
  ASTNode *ast_AddExp = nullptr;
  std::vector<Operator> op_list;

  if ((ast_AddExp = parse_AddExp()) == nullptr)
  {
    report_error("In parse_RelExp, expect first AddExp");
    goto CLEANUP;
  }

  ast_AddExp_list.push_back(ast_AddExp);

  for (;;)
  {
    if (peek_nth_token(1) != Token::OPERATOR)
      break;

    auto op = lexer.op_val();

    if (op != Operator::LT && op != Operator::GT &&
        op != Operator::LE && op != Operator::GE)
      break;

    op_list.push_back(op);
    next_token(); // eat op

    if ((ast_AddExp = parse_AddExp()) == nullptr)
    {
      report_error("In parse_RelExp, parse subsequent AddExp");
      goto CLEANUP;
    }

    ast_AddExp_list.push_back(ast_AddExp);
  }

  ast_RelExp = ast_AddExp_list[0];

  for (std::size_t i = 1, n = ast_AddExp_list.size(); i < n; ++i)
  {
    switch (op_list[i - 1])
    {
    case Operator::LT:
      ast_RelExp = new BinaryExpASTNode(
        BinaryExpASTNode::Operator::LT, ast_RelExp, ast_AddExp_list[i]);
      break;
    case Operator::GT:
      ast_RelExp = new BinaryExpASTNode(
        BinaryExpASTNode::Operator::GT, ast_RelExp, ast_AddExp_list[i]);
      break;
    case Operator::LE:
      ast_RelExp = new BinaryExpASTNode(
        BinaryExpASTNode::Operator::LE, ast_RelExp, ast_AddExp_list[i]);
      break;
    case Operator::GE:
      ast_RelExp = new BinaryExpASTNode(
        BinaryExpASTNode::Operator::GE, ast_RelExp, ast_AddExp_list[i]);
      break;
    default:
      assert(false);
    }
  }

CLEANUP:
  if (!ast_RelExp)
  {
    for (auto &ast : ast_AddExp_list)
      delete ast;
  }
  return ast_RelExp;
}

/**
 * EqExp         ::= RelExp {("==" | "!=") RelExp};
 */
ASTNode *Parser::parse_EqExp()
{
  ASTNode *ast_EqExp = nullptr;
  std::vector<ASTNode *> ast_RelExp_list;
  ASTNode *ast_RelExp = nullptr;
  std::vector<Operator> op_list;

  if ((ast_RelExp = parse_RelExp()) == nullptr)
  {
    report_error("In parse_EqExp, expect first RelExp");
    goto CLEANUP;
  }

  ast_RelExp_list.push_back(ast_RelExp);

  for (;;)
  {
    if (peek_nth_token(1) != Token::OPERATOR)
      break;

    auto op = lexer.op_val();

    if (op != Operator::EQ && op != Operator::NE)
      break;

    op_list.push_back(op);
    next_token(); // eat op

    if ((ast_RelExp = parse_RelExp()) == nullptr)
    {
      report_error("In parse_EqExp, expect subsequent RelExp");
      goto CLEANUP;
    }

    ast_RelExp_list.push_back(ast_RelExp);
  }

  ast_EqExp = ast_RelExp_list[0];

  for (std::size_t i = 1, n = ast_RelExp_list.size(); i < n; ++i)
  {
    switch (op_list[i - 1])
    {
    case Operator::EQ:
      ast_EqExp = new BinaryExpASTNode(
        BinaryExpASTNode::Operator::EQ, ast_EqExp, ast_RelExp_list[i]);
      break;
    case Operator::NE:
      ast_EqExp = new BinaryExpASTNode(
        BinaryExpASTNode::Operator::NE, ast_EqExp, ast_RelExp_list[i]);
      break;
    default:
      assert(false);
    }
  }

CLEANUP:
  if (!ast_EqExp)
  {
    for (auto &ast : ast_RelExp_list)
      delete ast;
  }
  return ast_EqExp;
}

/**
 * LAndExp       ::= EqExp {"&&" EqExp};
 */
ASTNode *Parser::parse_LAndExp()
{
  ASTNode *ast_LAndExp = nullptr;
  std::vector<ASTNode *> ast_EqExp_list;
  ASTNode *ast_EqExp = nullptr;
  std::vector<Operator> op_list;

  if ((ast_EqExp = parse_EqExp()) == nullptr)
  {
    report_error("In parse_LAndExp, expect first EqExp");
    goto CLEANUP;
  }

  ast_EqExp_list.push_back(ast_EqExp);

  for (;;)
  {
    if (peek_nth_token(1) != Token::OPERATOR)
      break;

    auto op = lexer.op_val();

    if (op != Operator::LAND)
      break;

    op_list.push_back(op);
    next_token(); // eat op

    if ((ast_EqExp = parse_EqExp()) == nullptr)
    {
      report_error("In parse_LAndExp, expect subsequent EqExp");
      goto CLEANUP;
    }

    ast_EqExp_list.push_back(ast_EqExp);
  }

  ast_LAndExp = ast_EqExp_list[0];

  for (std::size_t i = 1, n = ast_EqExp_list.size(); i < n; ++i)
  {
    switch (op_list[i - 1])
    {
    case Operator::LAND:
      ast_LAndExp = new BinaryExpASTNode(
        BinaryExpASTNode::Operator::LAND, ast_LAndExp, ast_EqExp_list[i]);
      break;
    default:
      assert(false);
    }
  }

CLEANUP:
  if (!ast_LAndExp)
  {
    for (auto &ast : ast_EqExp_list)
      delete ast;
  }
  return ast_LAndExp;
}

/**
 * LOrExp        ::= LAndExp {"||" LAndExp};
 */
ASTNode *Parser::parse_LOrExp()
{
  ASTNode *ast_LOrExp = nullptr;
  std::vector<ASTNode *> ast_LAndExp_list;
  ASTNode *ast_LAndExp = nullptr;
  std::vector<Operator> op_list;

  if ((ast_LAndExp = parse_LAndExp()) == nullptr)
  {
    report_error("In parse_LOrExp, expect first LAndExp");
    goto CLEANUP;
  }

  ast_LAndExp_list.push_back(ast_LAndExp);

  for (;;)
  {
    if (peek_nth_token(1) != Token::OPERATOR)
      break;

    auto op = lexer.op_val();

    if (op != Operator::LOR)
      break;

    op_list.push_back(op);
    next_token(); // eat op

    if ((ast_LAndExp = parse_LAndExp()) == nullptr)
    {
      report_error("In parse_LOrExp, expect subsequent LAndExp");
      goto CLEANUP;
    }

    ast_LAndExp_list.push_back(ast_LAndExp);
  }

  ast_LOrExp = ast_LAndExp_list[0];

  for (std::size_t i = 1, n = ast_LAndExp_list.size(); i < n; ++i)
  {
    switch (op_list[i - 1])
    {
    case Operator::LOR:
      ast_LOrExp = new BinaryExpASTNode(
        BinaryExpASTNode::Operator::LOR, ast_LOrExp, ast_LAndExp_list[i]);
      break;
    default:
      assert(false);
    }
  }

CLEANUP:
  if (!ast_LOrExp)
  {
    for (auto &ast : ast_LAndExp_list)
      delete ast;
  }
  return ast_LOrExp;
}

} // namespace frontend

} // namespace sysyc
