//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: node.h
//  Created Date: 2021-04-29
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_MIDDLE_AST_NODE_H__
#define __SYSYC_MIDDLE_AST_NODE_H__

#include <iostream>
#include <vector>
#include <string>

#include "frontend/sysystd.h"
#include "middle/ast/node_type.h"

namespace sysyc
{
namespace middle
{
namespace ast
{
class IASTGenerator;
class IASTAnalyzer;

class ASTNode
{
public:
  virtual ~ASTNode() = default;

protected:
  void replace(ASTNode **p_ast, ASTNode *repl_ast);
  void replace(std::vector<ASTNode *> *p_ast_list, std::size_t i, ASTNode *repl_ast);

public:
  virtual ASTNodeType node_type() const = 0;
  virtual bool analyze_on(IASTAnalyzer &analyzer) = 0;
  virtual bool generate_on(IASTGenerator &generator) const = 0;
};

/**
 * Program       ::= {CompUnit};
 * CompUnit      ::= Decl | FuncDef;
 */
class ProgramASTNode : public ASTNode
{
public:
  ProgramASTNode(std::vector<ASTNode *> &comp_unit_list)
    : comp_unit_list(comp_unit_list) {}

  ~ProgramASTNode()
  {
    for (auto &ast : comp_unit_list)
      delete ast;
  }

  const std::vector<ASTNode *> &get_comp_unit_list() const { return comp_unit_list; }

private:
  std::vector<ASTNode *> comp_unit_list;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * Decl          ::= ConstDecl | VarDecl;
 * ConstDecl     ::= "const" BType ConstDef {"," ConstDef} ";";
 * VarDecl       ::= BType VarDef {"," VarDef} ";";
 */
class VarDeclASTNode : public ASTNode
{
public:
  VarDeclASTNode(ASTNode *type, std::vector<ASTNode *> &def_list, bool const_)
    : type(type), def_list(def_list), const_(const_) {}

  ~VarDeclASTNode()
  {
    for (auto &ast : def_list)
      delete ast;
    delete type;
  }

  const ASTNode *get_type() const { return type; }
  const std::vector<ASTNode *> &get_def_list() const { return def_list; }
  bool is_const() const { return const_; }

  ASTNode *get_type() { return type; }

private:
  ASTNode *type;
  std::vector<ASTNode *> def_list;
  bool const_;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * ConstDef      ::= IDENT {"[" ConstExp "]"} "=" ConstInitVal;
 * VarDef        ::= IDENT {"[" ConstExp "]"}
 *                 | IDENT {"[" ConstExp "]"} "=" InitVal;
 */
class VarDefASTNode : public ASTNode
{
public:
  VarDefASTNode(const std::string &id, std::vector<ASTNode *> &dim_list,
                ASTNode *init_val, bool const_)
    : id(id), dim_list(dim_list), init_val(init_val), const_(const_) {}

  ~VarDefASTNode()
  {
    delete init_val;
    for (auto &ast : dim_list)
      delete ast;
  }

  const std::string &get_id() const { return id; }
  const std::vector<ASTNode *> &get_dim_list() const { return dim_list; }
  const ASTNode *get_init_val() const { return init_val; }
  bool is_const() const { return const_; }

  ASTNode *get_init_val() { return init_val; }

  void set_dim(std::size_t i, ASTNode *ast) { replace(&dim_list, i, ast); }
  void set_init_val(ASTNode *ast) { replace(&init_val, ast); }

private:
  std::string id;
  std::vector<ASTNode *> dim_list;
  ASTNode *init_val;
  bool const_;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * InitVal       ::= Exp | "{" [InitVal {"," InitVal}] "}";
 * ConstInitVal  ::= ConstExp | "{" [ConstInitVal {"," ConstInitVal}] "}";
 */
class InitValListASTNode : public ASTNode
{
public:
  InitValListASTNode(std::vector<ASTNode *> &init_val_list, bool const_)
    : init_val_list(init_val_list), const_(const_) {}

  ~InitValListASTNode()
  {
    for (auto &ast : init_val_list)
      delete ast;
  }

  const std::vector<ASTNode *> &get_init_val_list() const { return init_val_list; }
  bool is_const() const { return const_; }

  void set_init_val(std::size_t i, ASTNode *ast) { replace(&init_val_list, i, ast); }

private:
  std::vector<ASTNode *> init_val_list;
  bool const_;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * FuncDef       ::= FuncType IDENT "(" [FuncFParams] ")" Block;
 */
class FuncDefASTNode : public ASTNode
{
public:
  FuncDefASTNode(ASTNode *type, const std::string &id, ASTNode *params, ASTNode *block)
    : type(type), id(id), params(params), block(block) {}

  ~FuncDefASTNode()
  {
    delete block;
    delete params;
    delete type;
  }

  const ASTNode *get_type() const { return type; }
  const std::string &get_id() const { return id; }
  const ASTNode *get_params() const { return params; }
  const ASTNode *get_block() const { return block; }

  ASTNode *get_type() { return type; }
  ASTNode *get_params() { return params; }
  ASTNode *get_block() { return block; }

private:
  ASTNode *type;
  std::string id;
  ASTNode *params; // NOTE: params may be nullptr
  ASTNode *block;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * FuncFParams   ::= FuncFParam {"," FuncFParam};
 */
class FuncFParamListASTNode : public ASTNode
{
public:
  FuncFParamListASTNode(std::vector<ASTNode *> &param_list)
    : param_list(param_list) {}

  ~FuncFParamListASTNode()
  {
    for (auto &ast : param_list)
      delete ast;
  }

  const std::vector<ASTNode *> &get_param_list() const { return param_list; }

private:
  std::vector<ASTNode *> param_list;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * FuncFParam    ::= BType IDENT ["[" "]" {"[" ConstExp "]"}];
 */
class FuncFParamASTNode : public ASTNode
{
public:
  FuncFParamASTNode(ASTNode *type, std::string &id, std::vector<ASTNode *> &dim_list)
    : type(type), id(id), dim_list(dim_list) {}

  ~FuncFParamASTNode()
  {
    for (auto &ast : dim_list)
      delete ast;
    delete type;
  }

  const ASTNode *get_type() const { return type; }
  const std::string &get_id() const { return id; }
  const std::vector<ASTNode *> &get_dim_list() const { return dim_list; }

  ASTNode *get_type() { return type; }

  void set_dim(std::size_t i, ASTNode *ast) { replace(&dim_list, i, ast); }

private:
  ASTNode *type;
  std::string id;
  std::vector<ASTNode *> dim_list; // NOTE: dim_list[0] may be nullptr

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * Block         ::= "{" {BlockItem} "}";
 * BlockItem     ::= Decl | Stmt;
 */
class BlockASTNode : public ASTNode
{
public:
  BlockASTNode(std::vector<ASTNode *> &block_item_list)
    : block_item_list(block_item_list) {}

  ~BlockASTNode()
  {
    for (auto &ast : block_item_list)
      delete ast;
  }

  const std::vector<ASTNode *> &get_block_item_list() const { return block_item_list; }

private:
  std::vector<ASTNode *> block_item_list;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * IfElseStmt    ::= "if" "(" Cond ")" Stmt ["else" Stmt];
 */
class IfElseStmtASTNode : public ASTNode
{
public:
  IfElseStmtASTNode(ASTNode *cond, ASTNode *stmt_if, ASTNode *stmt_else)
    : cond(cond), stmt_if(stmt_if), stmt_else(stmt_else) {}

  ~IfElseStmtASTNode()
  {
    delete stmt_else;
    delete stmt_if;
    delete cond;
  }

  const ASTNode *get_cond() const { return cond; }
  const ASTNode *get_stmt_if() const { return stmt_if; }
  const ASTNode *get_stmt_else() const { return stmt_else; }

  ASTNode *get_cond() { return cond; }
  ASTNode *get_stmt_if() { return stmt_if; }
  ASTNode *get_stmt_else() { return stmt_else; }

  void set_cond(ASTNode *ast) { replace(&cond, ast); }

private:
  ASTNode *cond;
  ASTNode *stmt_if;
  ASTNode *stmt_else;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * WhileStmt     ::= "while" "(" Cond ")" Stmt;
 */
class WhileStmtASTNode : public ASTNode
{
public:
  WhileStmtASTNode(ASTNode *cond, ASTNode *stmt) : cond(cond), stmt(stmt) {}

  ~WhileStmtASTNode()
  {
    delete stmt;
    delete cond;
  }

  const ASTNode *get_cond() const { return cond; }
  const ASTNode *get_stmt() const { return stmt; }

  ASTNode *get_cond() { return cond; }
  ASTNode *get_stmt() { return stmt; }

  void set_cond(ASTNode *ast) { replace(&cond, ast); }

private:
  ASTNode *cond;
  ASTNode *stmt;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * ControlStmt   ::= "break" ";"
 *                | "continue" ";"
 *                | "return" [Exp] ";";
 */
class ControlStmtASTNode : public ASTNode
{
public:
  enum class Type { BREAK, CONTINUE, RETURN };

public:
  ControlStmtASTNode(Type type, ASTNode *expr) : type(type), expr(expr) {}

  ~ControlStmtASTNode() { delete expr; }

  Type get_type() const { return type; }
  const ASTNode *get_expr() const { return expr; }

  ASTNode *get_expr() { return expr; }

  void set_expr(ASTNode *ast) { replace(&expr, ast); }

private:
  Type type;
  ASTNode *expr;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * AssignStmt    ::= LVal "=" Exp ";";
 * MulExp        ::= UnaryExp {("*" | "/" | "%") UnaryExp};
 * AddExp        ::= MulExp {("+" | "-") MulExp};
 * RelExp        ::= AddExp {("<" | ">" | "<=" | ">=") AddExp};
 * EqExp         ::= RelExp {("==" | "!=") RelExp};
 * LAndExp       ::= EqExp {"&&" EqExp};
 * LOrExp        ::= LAndExp {"||" LAndExp};
 */
class BinaryExpASTNode : public ASTNode
{
public:
  enum class Operator : int
  {
    MUL = 30, DIV = 31, MOD = 32,
    ADD = 40, SUB = 41,
    LT = 60, GT = 61, LE = 62, GE = 63,
    EQ = 70, NE = 71,
    LAND = 110,
    LOR = 120,
    ASSIGN = 140,
  };

public:
  BinaryExpASTNode(Operator op, ASTNode *lhs, ASTNode *rhs)
    : op(op), lhs(lhs), rhs(rhs) {}

  ~BinaryExpASTNode()
  {
    delete rhs;
    delete lhs;
  }

  Operator get_op() const { return op; }
  const ASTNode *get_lhs() const { return lhs; }
  const ASTNode *get_rhs() const { return rhs; }

  ASTNode *get_lhs() { return lhs; }
  ASTNode *get_rhs() { return rhs; }

  bool is_assign() const { return op == Operator::ASSIGN; }
  bool is_logical() const { return op == Operator::LAND || op == Operator::LOR; }

  int get_op_prec() const { return -(static_cast<int>(op) / 10); }

  void set_lhs(ASTNode *ast) { replace(&lhs, ast); }
  void set_rhs(ASTNode *ast) { replace(&rhs, ast); }

private:
  Operator op;
  ASTNode *lhs;
  ASTNode *rhs;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * PrimaryExp    ::= "(" Exp ")" | LVal | Number;
 * UnaryExp      ::= PrimaryExp | FuncCall | UnaryOp UnaryExp;
 * UnaryOp       ::= "+" | "-" | "!";
 */
class UnaryExpASTNode : public ASTNode
{
public:
  enum class Operator { POS, NEG, LNOT };

public:
  UnaryExpASTNode(Operator op, ASTNode *expr) : op(op), expr(expr) {}

  ~UnaryExpASTNode() { delete expr; }

  Operator get_op() const { return op; }
  const ASTNode *get_expr() const { return expr; }

  ASTNode *get_expr() { return expr; }

  void set_op(Operator op) { this->op = op; }
  void set_expr(ASTNode *ast) { replace(&expr, ast); }

private:
  Operator op;
  ASTNode *expr;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * FuncCall      ::= IDENT "(" [FuncRParams] ")";
 */
class FuncCallASTNode : public ASTNode
{
public:
  FuncCallASTNode(const std::string &id, ASTNode *params)
    : id(id), params(params) {}

  ~FuncCallASTNode() { delete params; }

  const std::string &get_id() const { return id; }
  const ASTNode *get_params() const { return params; }

  ASTNode *get_params() { return params; }

private:
  std::string id;
  ASTNode *params; // NOTE: params may be nullptr

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * FuncRParams   ::= Exp {"," Exp};
 */
class FuncRParamListASTNode : public ASTNode
{
public:
  FuncRParamListASTNode(std::vector<ASTNode *> &param_list)
    : param_list(param_list) {}

  ~FuncRParamListASTNode()
  {
    for (auto &ast : param_list)
      delete ast;
  }

  const std::vector<ASTNode *> &get_param_list() const { return param_list; }

  void set_param(std::size_t i, ASTNode *ast) { replace(&param_list, i, ast); }

private:
  std::vector<ASTNode *> param_list;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * BareStmt      ::= [Exp] ";";
 */
class BareStmtASTNode : public ASTNode
{
public:
  BareStmtASTNode(ASTNode *expr) : expr(expr) {}

  ~BareStmtASTNode() { delete expr; }

  const ASTNode *get_expr() const { return expr; }

  ASTNode *get_expr() { return expr; }

  void set_expr(ASTNode *ast) { replace(&expr, ast); }

private:
  ASTNode *expr;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

/**
 * LVal          ::= IDENT {"[" Exp "]"};
 */
class LValASTNode : public ASTNode
{
public:
  LValASTNode(const std::string &id, std::vector<ASTNode *> &ix_list)
    : id(id), ix_list(ix_list) {}

  ~LValASTNode()
  {
    for (auto &ast : ix_list)
      delete ast;
  }

  const std::string &get_id() const { return id; }
  const std::vector<ASTNode *> &get_ix_list() const { return ix_list; }

  void set_ix(std::size_t i, ASTNode *ast) { replace(&ix_list, i, ast); }

private:
  std::string id;
  std::vector<ASTNode *> ix_list;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};


class NumberASTNode : public ASTNode
{
public:
  NumberASTNode(sysystd::int_t val) : val(val) {}

  ~NumberASTNode() {}

  sysystd::int_t get_val() const { return val; }

private:
  sysystd::int_t val;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};


class PrimTypeASTNode : public ASTNode
{
public:
  enum class Type { VOID, INT };

public:
  PrimTypeASTNode(Type type) : type(type) {}

  ~PrimTypeASTNode() {}

  Type get_type() const { return type; }

private:
  Type type;

public:
  ASTNodeType node_type() const override;
  bool analyze_on(IASTAnalyzer &analyzer) override;
  bool generate_on(IASTGenerator &generator) const override;
};

} // namespace ast

} // namespace middle

} // namespace sysyc

namespace std
{
std::string to_string(const sysyc::middle::ast::BinaryExpASTNode::Operator &op);
std::string to_string(const sysyc::middle::ast::UnaryExpASTNode::Operator &op);

} // namespace std

#endif // __SYSYC_MIDDLE_AST_NODE_H__
