//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: evaluator.cc
//  Created Date: 2021-05-06
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "backend/ast/evaluator/evaluator.h"
#include "backend/ast/evaluator/ivlist.h"

namespace sysyc
{
namespace backend
{
namespace ast
{
using namespace middle::ast;

namespace __impl
{
inline bool is_literal(const ASTNode *ast)
{
  return ast->node_type() == ASTNodeType::Number;
}

inline bool is_ivlist(const ASTNode *ast)
{
  return ast->node_type() == ASTNodeType::InitValList;
}

} // namespace __impl

ASTEvaluator::ASTEvaluator(misc::Logger &logger)
  : logger(logger), errcnt(0), env(nullptr), has_prebuilt_env(false)
{
  push_scope();
}

void ASTEvaluator::report_error(const std::string &errmsg)
{
  logger.error("[ASTEvaluator] " + errmsg);
  errcnt++;
}

void ASTEvaluator::push_scope()
{
  env = new __impl::evaluator_scope_s(env);
}

void ASTEvaluator::pop_scope()
{
  assert(env != nullptr);

  auto old_env = env;
  env = env->parent;
  delete old_env;
}

bool ASTEvaluator::analyze(ProgramASTNode *root)
{
  for (auto &cunit : root->get_comp_unit_list())
    cunit->analyze_on(*this);
  return false;
}

bool ASTEvaluator::analyze(VarDeclASTNode *root)
{
  for (auto &def : root->get_def_list())
    def->analyze_on(*this);
  return false;
}

bool ASTEvaluator::analyze(VarDefASTNode *root)
{
  if (root->is_const() && root->get_init_val() == nullptr)
  {
    report_error("In VarDefASTNode, ConstInitVal is missing, is_const = true");
    return false;
  }

  auto entry = env->alloc(root->get_id());

  if (entry == nullptr)
  {
    report_error("In VarDefASTNode, multiple definitions, id: " + root->get_id());
    return false;
  }

  auto init_val = root->get_init_val();

  if (init_val && !__impl::is_literal(init_val) && init_val->analyze_on(*this))
  {
    init_val = new NumberASTNode(eval_result);
    root->set_init_val(init_val);
  }

  const auto &dim_list = root->get_dim_list();
  bool failed = false;

  for (std::size_t i = 0, n = dim_list.size(); i < n; ++i)
  {
    if (!dim_list[i]->analyze_on(*this))
    {
      failed = true;
      report_error("In VarDefASTNode, unable to eval dim_list[" +
                    std::to_string(i) + "], id: " + root->get_id());
      continue;
    }

    if (eval_result < 0)
    {
      failed = true;
      report_error("In VarDefASTNode, dim_list[" +
                    std::to_string(i) + "] < 0, id: " + root->get_id());
      continue;
    }

    if (!__impl::is_literal(dim_list[i]))
      root->set_dim(i, new NumberASTNode(eval_result));

    entry->dim_list.push_back(eval_result);
  }

  if (failed)
  {
    entry->dim_list.clear();
    return false;
  }

  if (!root->is_const())
    return false;

  entry->const_ = true;

  assert(init_val != nullptr);

  if (__impl::is_literal(init_val))
  {
    entry->value = dynamic_cast<NumberASTNode *>(init_val)->get_val();
  }
  else if (__impl::is_ivlist(init_val))
  {
    auto it = __impl::evaluator_ivlist_iterator(init_val, entry->dim_list);

    try
    {
      while (it.next())
      {
        auto ret = entry->value_map.insert({it.get_ix(), it.get_val()});
        assert(ret.second);
      }
    }
    catch(const __impl::evaluator_ivlist_iterator::exception& e)
    {
      report_error("In VarDefASTNode, " + std::string(e.what()));
      return false;
    }
  }
  else
  {
    assert(false);
  }

  return false;
}

bool ASTEvaluator::analyze(InitValListASTNode *root)
{
  const auto &init_val_list = root->get_init_val_list();

  for (std::size_t i = 0, n = init_val_list.size(); i < n; ++i)
  {
    if (__impl::is_literal(init_val_list[i]))
      continue;

    if (__impl::is_ivlist(init_val_list[i]))
    {
      init_val_list[i]->analyze_on(*this);
      continue;
    }

    if (!init_val_list[i]->analyze_on(*this))
    {
      if (root->is_const())
      {
        report_error("In InitValListASTNode, unable to eval init_val_list[" +
                      std::to_string(i) + "], is_const = true");
      }
      continue;
    }

    root->set_init_val(i, new NumberASTNode(eval_result));
  }

  return false;
}

bool ASTEvaluator::analyze(FuncDefASTNode *root)
{
  const auto &params = root->get_params();

  if (params != nullptr)
  {
    push_scope();

    params->analyze_on(*this);

    has_prebuilt_env = true;
  }

  root->get_block()->analyze_on(*this);
  return false;
}

bool ASTEvaluator::analyze(FuncFParamListASTNode *root)
{
  for (auto &param : root->get_param_list())
    param->analyze_on(*this);
  return false;
}

bool ASTEvaluator::analyze(FuncFParamASTNode *root)
{
  const auto &dim_list = root->get_dim_list();

  for (std::size_t i = 0, n = dim_list.size(); i < n; ++i)
  {
    if (dim_list[i] == nullptr || __impl::is_literal(dim_list[i]))
      continue;

    if (!dim_list[i]->analyze_on(*this))
    {
      report_error("In FuncFParamASTNode, unable to eval dim_list[" +
                    std::to_string(i) + "], id: " + root->get_id());
      continue;
    }

    if (eval_result < 0)
    {
      report_error("In FuncFParamASTNode, dim_list[" +
                    std::to_string(i) + "] < 0, id: " + root->get_id());
      continue;
    }

    root->set_dim(i, new NumberASTNode(eval_result));
  }

  auto entry = env->alloc(root->get_id());
  assert(entry != nullptr);

  return false;
}

bool ASTEvaluator::analyze(BlockASTNode *root)
{
  if (!has_prebuilt_env)
    push_scope();

  has_prebuilt_env = false;

  for (auto &bitem : root->get_block_item_list())
    bitem->analyze_on(*this);

  pop_scope();

  return false;
}

bool ASTEvaluator::analyze(IfElseStmtASTNode *root)
{
  const auto &cond = root->get_cond();

  if (!__impl::is_literal(cond) && cond->analyze_on(*this))
    root->set_cond(new NumberASTNode(eval_result));

  root->get_stmt_if()->analyze_on(*this);

  if (root->get_stmt_else() != nullptr)
    root->get_stmt_else()->analyze_on(*this);

  return false;
}

bool ASTEvaluator::analyze(WhileStmtASTNode *root)
{
  const auto &cond = root->get_cond();

  if (!__impl::is_literal(cond) && cond->analyze_on(*this))
    root->set_cond(new NumberASTNode(eval_result));

  root->get_stmt()->analyze_on(*this);

  return false;
}

bool ASTEvaluator::analyze(ControlStmtASTNode *root)
{
  const auto &expr = root->get_expr();

  if (expr && !__impl::is_literal(expr) && expr->analyze_on(*this))
    root->set_expr(new NumberASTNode(eval_result));

  return false;
}

bool ASTEvaluator::analyze(BinaryExpASTNode *root)
{
  if (!root->get_rhs()->analyze_on(*this))
    return false;

  auto rhs_val = eval_result;

  if (!__impl::is_literal(root->get_rhs()))
    root->set_rhs(new NumberASTNode(rhs_val));

  if (root->is_assign())
  {
    root->get_lhs()->analyze_on(*this);
    return false;
  }

  if (!root->get_lhs()->analyze_on(*this))
    return false;

  auto lhs_val = eval_result;

  if (!__impl::is_literal(root->get_lhs()))
    root->set_lhs(new NumberASTNode(lhs_val));

  switch (root->get_op())
  {
  case BinaryExpASTNode::Operator::MUL:
    eval_result = lhs_val * rhs_val;
    break;
  case BinaryExpASTNode::Operator::DIV:
    eval_result = lhs_val / rhs_val;
    break;
  case BinaryExpASTNode::Operator::MOD:
    eval_result = lhs_val % rhs_val;
    break;
  case BinaryExpASTNode::Operator::ADD:
    eval_result = lhs_val + rhs_val;
    break;
  case BinaryExpASTNode::Operator::SUB:
    eval_result = lhs_val - rhs_val;
    break;

  case BinaryExpASTNode::Operator::LT:
    eval_result = sysystd::bool2int(lhs_val < rhs_val);
    break;
  case BinaryExpASTNode::Operator::GT:
    eval_result = sysystd::bool2int(lhs_val > rhs_val);
    break;
  case BinaryExpASTNode::Operator::LE:
    eval_result = sysystd::bool2int(lhs_val <= rhs_val);
    break;
  case BinaryExpASTNode::Operator::GE:
    eval_result = sysystd::bool2int(lhs_val >= rhs_val);
    break;
  case BinaryExpASTNode::Operator::EQ:
    eval_result = sysystd::bool2int(lhs_val == rhs_val);
    break;
  case BinaryExpASTNode::Operator::NE:
    eval_result = sysystd::bool2int(lhs_val != rhs_val);
    break;

  case BinaryExpASTNode::Operator::LAND:
    eval_result = sysystd::bool2int(
      sysystd::int2bool(lhs_val) && sysystd::int2bool(rhs_val));
    break;
  case BinaryExpASTNode::Operator::LOR:
    eval_result = sysystd::bool2int(
      sysystd::int2bool(lhs_val) || sysystd::int2bool(rhs_val));
    break;

  default:
    assert(false);
  }

  return true;
}

bool ASTEvaluator::analyze(UnaryExpASTNode *root)
{
  if (!root->get_expr()->analyze_on(*this))
    return false;

  switch (root->get_op())
  {
  case UnaryExpASTNode::Operator::POS:
    break;
  case UnaryExpASTNode::Operator::NEG:
    eval_result = -eval_result;
    break;
  case UnaryExpASTNode::Operator::LNOT:
    eval_result = sysystd::bool2int(!sysystd::int2bool(eval_result));
    break;
  default:
    assert(false);
  }

  if (!__impl::is_literal(root->get_expr()) ||
      root->get_op() != UnaryExpASTNode::Operator::POS)
  {
    root->set_op(UnaryExpASTNode::Operator::POS);
    root->set_expr(new NumberASTNode(eval_result));
  }

  return true;
}

bool ASTEvaluator::analyze(FuncCallASTNode *root)
{
  if (root->get_params() != nullptr)
    root->get_params()->analyze_on(*this);
  return false;
}

bool ASTEvaluator::analyze(FuncRParamListASTNode *root)
{
  const auto &param_list = root->get_param_list();

  for (std::size_t i = 0, n = param_list.size(); i < n; ++i)
  {
    if (__impl::is_literal(param_list[i]))
      continue;

    if (!param_list[i]->analyze_on(*this))
      continue;

    root->set_param(i, new NumberASTNode(eval_result));
  }

  return false;
}

bool ASTEvaluator::analyze(BareStmtASTNode *root)
{
  const auto &expr = root->get_expr();

  if (expr && !__impl::is_literal(expr) && expr->analyze_on(*this))
    root->set_expr(new NumberASTNode(eval_result));

  return false;
}

bool ASTEvaluator::analyze(LValASTNode *root)
{
  bool failed = false;

  const auto &ix_list = root->get_ix_list();

  for (std::size_t i = 0, n = ix_list.size(); i < n; ++i)
  {
    if (__impl::is_literal(ix_list[i]))
      continue;

    if (!ix_list[i]->analyze_on(*this))
    {
      failed = true;
      continue;
    }

    root->set_ix(i, new NumberASTNode(eval_result));
  }

  if (failed)
    return false;

  const auto entry = env->find(root->get_id());

  if (entry == nullptr)
  {
    report_error("In LValASTNode, undefined id \"" + root->get_id() + "\"");
    return false;
  }

  if (!entry->is_const())
    return false;

  if (ix_list.empty())
  {
    if (entry->is_array())
    {
      report_error("In LValASTNode, id \"" + root->get_id() +
                    "\" is declared to be an array");
      return false;
    }

    eval_result = entry->value;
  }
  else
  {
    if (!entry->is_array())
    {
      report_error("In LValASTNode, id \"" + root->get_id() +
                    "\" is declared to be a scalar");
      return false;
    }

    if (entry->dim_list.size() != ix_list.size())
    {
      report_error("In LValASTNode, incorrect dimensions of id \"" +
                    root->get_id() + "\", expect: " +
                    std::to_string(entry->dim_list.size()) + ", current: " +
                    std::to_string(ix_list.size()));
      return false;
    }

    std::size_t ix = 0;

    for (std::size_t i = 0, n = entry->dim_list.size(); i < n; ++i)
    {
      assert(__impl::is_literal(ix_list[i]));

      ix *= entry->dim_list[i];
      ix += dynamic_cast<const NumberASTNode *>(ix_list[i])->get_val();
    }

    auto it = entry->value_map.find(ix);
    eval_result = (it == entry->value_map.end()) ? 0 : it->second;
  }

  return true;
}

bool ASTEvaluator::analyze(NumberASTNode *root)
{
  eval_result = root->get_val();
  return true;
}

bool ASTEvaluator::analyze(PrimTypeASTNode *root)
{
  return false;
}

} // namespace ast

} // namespace backend

} // namespace sysyc

