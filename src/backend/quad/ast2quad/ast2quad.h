//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: ast2quad.h
//  Created Date: 2021-05-22
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_BACKEND_QUAD_AST2QUAD_AST2QUAD_H__
#define __SYSYC_BACKEND_QUAD_AST2QUAD_AST2QUAD_H__

#include "misc/logger.h"
#include "middle/ast/generator.h"
#include "backend/quad/ast2quad/context.h"

namespace sysyc
{
namespace backend
{
namespace quad
{
class QuadrupleGenerator : public middle::ast::IASTGenerator
{
public:
  QuadrupleGenerator(middle::quad::InstructionList &instseq, misc::Logger &logger);

  void link(const std::string &func_id, bool has_retval)
    { add_funcdef(func_id, has_retval); }

  void link(const std::string &func_id, const std::string &link_id, bool has_retval)
    { add_funcdef(func_id, link_id, has_retval); }

private:
  using vardef_t = __impl::vardef_s;
  using funcdef_t = __impl::funcdef_s;

  void report_error(const std::string &errmsg);

  void push_scope();
  void pop_scope();

  const vardef_t *add_vardef(
    const std::string &id, const std::vector<std::size_t> &dim_list);

  const vardef_t *add_paramdef(
    const std::string &id, const std::vector<std::size_t> &dim_list);

  const funcdef_t *add_funcdef(
    const std::string &id, const std::string &link_id, bool has_retval);

  const vardef_t *add_vardef(const std::string &id)
    { return add_vardef(id, {}); }

  const funcdef_t *add_funcdef(const std::string &id, bool has_retval)
    { return add_funcdef(id, id, has_retval); }

  const vardef_t *get_vardef(const std::string &id);

  const funcdef_t *get_funcdef(const std::string &id);

  middle::quad::label_t new_label() { return label_id++; }

  middle::quad::addr_t new_temp();

  void free_temp(const middle::quad::addr_t &addr);
  void free_temp(const middle::quad::Operand *opnd);

  misc::Logger &logger;

  middle::quad::InstructionList &instseq;
  __impl::quadgen_scope_s *env;

  bool has_prebuilt_env;
  std::size_t fparam_cnt;
  const vardef_t *lval_vardef;
  std::stack<__impl::loop_ctx_s> loop_ctx;
  std::stack<__impl::cond_ctx_s> cond_ctx;
  std::stack<std::vector<middle::quad::Instruction *> > st_fparams;

  const vardef_t *last_vardef;
  middle::quad::Operand *last_addr;

  middle::quad::addr_t T_addr_id;
  middle::quad::addr_t p_addr_id;
  middle::quad::addr_t t_addr_id;
  middle::quad::addr_t f_addr_id;
  middle::quad::label_t label_id;
  std::vector<middle::quad::addr_t> t_addr_unused;

public:
  bool generate(const middle::ast::ProgramASTNode *root) override;
  bool generate(const middle::ast::VarDeclASTNode *root) override;
  bool generate(const middle::ast::VarDefASTNode *root) override;
  bool generate(const middle::ast::InitValListASTNode *root) override;
  bool generate(const middle::ast::FuncDefASTNode *root) override;
  bool generate(const middle::ast::FuncFParamListASTNode *root) override;
  bool generate(const middle::ast::FuncFParamASTNode *root) override;
  bool generate(const middle::ast::BlockASTNode *root) override;
  bool generate(const middle::ast::IfElseStmtASTNode *root) override;
  bool generate(const middle::ast::WhileStmtASTNode *root) override;
  bool generate(const middle::ast::ControlStmtASTNode *root) override;
  bool generate(const middle::ast::BinaryExpASTNode *root) override;
  bool generate(const middle::ast::UnaryExpASTNode *root) override;
  bool generate(const middle::ast::FuncCallASTNode *root) override;
  bool generate(const middle::ast::FuncRParamListASTNode *root) override;
  bool generate(const middle::ast::BareStmtASTNode *root) override;
  bool generate(const middle::ast::LValASTNode *root) override;
  bool generate(const middle::ast::NumberASTNode *root) override;
  bool generate(const middle::ast::PrimTypeASTNode *root) override;
};

} // namespace quad

} // namespace backend

} // namespace sysyc

#endif // __SYSYC_BACKEND_QUAD_AST2QUAD_AST2QUAD_H__
