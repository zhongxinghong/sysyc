//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: ast2quad.cc
//  Created Date: 2021-05-22
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "backend/quad/ast2quad/ast2quad.h"
#include "backend/quad/ast2quad/ivlist.h"

namespace sysyc
{
namespace backend
{
namespace quad
{
using namespace middle::ast;
using namespace middle::quad;

namespace __impl
{
inline void assert_type_int(const ASTNode *type)
{
  const auto ast = dynamic_cast<const PrimTypeASTNode *>(type);
  assert(ast && ast->get_type() == PrimTypeASTNode::Type::INT);
}

inline bool is_type_void(const ASTNode *type)
{
  const auto ast = dynamic_cast<const PrimTypeASTNode *>(type);
  assert(ast != nullptr);
  return ast->get_type() == PrimTypeASTNode::Type::VOID;
}

inline sysystd::int_t ast2int(const ASTNode *num)
{
  const auto ast = dynamic_cast<const NumberASTNode *>(num);
  assert(ast != nullptr);
  return ast->get_val();
}

inline bool ast2bool(const ASTNode *num)
{
  return sysystd::int2bool(ast2int(num));
}

inline bool is_literal(const ASTNode *ast)
{
  return ast->node_type() == ASTNodeType::Number;
}

inline bool is_literal(const std::vector<ASTNode *> &ast_list)
{
  for (auto &ast : ast_list)
  {
    if (!is_literal(ast))
      return false;
  }
  return true;
}

inline bool is_ivlist(const ASTNode *ast)
{
  return ast->node_type() == ASTNodeType::InitValList;
}

} // namespace __impl

QuadrupleGenerator::QuadrupleGenerator(InstructionList &instseq, misc::Logger &logger) :
  logger(logger), instseq(instseq), env(nullptr), has_prebuilt_env(false),
  fparam_cnt(0), last_vardef(nullptr), last_addr(nullptr), T_addr_id(0),
  p_addr_id(0), t_addr_id(0), f_addr_id(0), label_id(0)
{
  push_scope();
}

void QuadrupleGenerator::report_error(const std::string &errmsg)
{
  logger.error("[QuadrupleGenerator] " + errmsg);
}

bool QuadrupleGenerator::generate(const ProgramASTNode *root)
{
  for (auto &cunit : root->get_comp_unit_list())
  {
    if (!cunit->generate_on(*this))
    {
      report_error("In ProgramASTNode, failed to generate CompUnit");
      return false;
    }
  }

  return true;
}

bool QuadrupleGenerator::generate(const VarDeclASTNode *root)
{
  __impl::assert_type_int(root->get_type());

  for (auto &def : root->get_def_list())
  {
    if (!def->generate_on(*this))
    {
      report_error("In VarDeclASTNode, failed to generate VarDef");
      return false;
    }
  }

  return true;
}

bool QuadrupleGenerator::generate(const VarDefASTNode *root)
{
  auto &list = instseq.list;

  std::vector<std::size_t> dim_list;
  std::size_t array_size = 1;

  for (auto &expr : root->get_dim_list())
  {
    dim_list.push_back(static_cast<std::size_t>(__impl::ast2int(expr)));
    array_size *= dim_list.back();
  }

  last_vardef = add_vardef(root->get_id(), dim_list);

  if (last_vardef == nullptr)
  {
    report_error("In VarDefASTNode, id \"" + root->get_id() +
                  "\" has multiple definitions");
    return false;
  }

  Operand *opnd_array_size = nullptr;

  if (!dim_list.empty())
  {
    const auto &type_size = last_vardef->type_size;
    opnd_array_size = new Operand(Operand::Type::NUM, array_size * type_size);
  }

  list.push_back(Instruction::new_vdef(last_vardef->addr, opnd_array_size));

  const auto &init_val = root->get_init_val();

  if (init_val)
  {
    if (opnd_array_size != nullptr && env->parent != nullptr)
    {
      assert(array_size > 0);

      auto label_begin = new_label();
      auto addr_i = new_temp();

      list.push_back(new Instruction(
        Opcode::SWORD,
        new Operand(Operand::Type::ADDR, addr_i),
        new Operand(Operand::Type::NUM, 0),
        nullptr
      ));

      list.push_back(Instruction::new_label(label_begin));

      list.push_back(new Instruction(
        Opcode::SWORD,
        new Operand(Operand::Type::ADDR, last_vardef->addr),
        new Operand(Operand::Type::NUM, 0),
        new Operand(Operand::Type::ADDR, addr_i)
      ));

      list.push_back(new Instruction(
        Opcode::ADD,
        new Operand(Operand::Type::ADDR, addr_i),
        new Operand(Operand::Type::ADDR, addr_i),
        new Operand(Operand::Type::NUM, 4)
      ));

      list.push_back(new Instruction(
        Opcode::JLT,
        new Operand(Operand::Type::ADDR, label_begin),
        new Operand(Operand::Type::ADDR, addr_i),
        new Operand(Operand::Type::NUM, opnd_array_size->val)
      ));

      free_temp(addr_i);
    }

    if (__impl::is_literal(init_val))
    {
      if (!dim_list.empty())
      {
        report_error("In VarDefASTNode, illegal InitVal, id: " + root->get_id());
        return false;
      }

      assert(init_val->generate_on(*this));
      assert(last_addr != nullptr);

      list.push_back(new Instruction(
        Opcode::SWORD,
        new Operand(Operand::Type::ADDR, last_vardef->addr),
        last_addr,
        nullptr
      ));

      free_temp(last_addr);
    }
    else if (!__impl::is_ivlist(init_val))
    {
      if (!dim_list.empty())
      {
        report_error("In VarDefASTNode, illegal InitVal, id: " + root->get_id());
        return false;
      }

      if (root->is_const())
      {
        // evaluator need be run first
        report_error("In VarDefASTNode, value of id: \"" + root->get_id() +
                      "\" can't be determined at compile time");
        return false;
      }

      if (!init_val->generate_on(*this))
      {
        report_error("In VarDefASTNode, failed to generate InitVal");
        return false;
      }

      if (last_addr == nullptr)
      {
        report_error("In VarDefASTNode, address of InitVal is missing");
        return false;
      }

      list.push_back(new Instruction(
        Opcode::SWORD,
        new Operand(Operand::Type::ADDR, last_vardef->addr),
        last_addr,
        nullptr
      ));

      free_temp(last_addr);
    }
    else
    {
      if (dim_list.empty())
      {
        report_error("In VarDefASTNode, illegal InitVal, id: " + root->get_id());
        return false;
      }

      if (!init_val->generate_on(*this))
      {
        report_error("In VarDefASTNode, failed to generate InitValList");
        return false;
      }
    }
  }

  return true;
}

bool QuadrupleGenerator::generate(const InitValListASTNode *root)
{
  auto &list = instseq.list;
  auto it = __impl::quadgen_ivlist_iterator(root, last_vardef->dim_list, this);

  try
  {
    while (it.next())
    {
      if (last_addr == nullptr)
      {
        report_error("In InitValListASTNode, address is missing at index " +
                      std::to_string(it.get_ix()));
        return false;
      }

      list.push_back(new Instruction(
        Opcode::SWORD,
        new Operand(Operand::Type::ADDR, last_vardef->addr),
        last_addr,
        new Operand(Operand::Type::NUM, it.get_ix() * last_vardef->type_size)
      ));

      free_temp(last_addr);
    }
  }
  catch(const __impl::quadgen_ivlist_iterator::exception& e)
  {
    report_error("In InitValListASTNode, " + std::string(e.what()));
    return false;
  }

  return true;
}

bool QuadrupleGenerator::generate(const FuncDefASTNode *root)
{
  auto &list = instseq.list;

  if (env->parent != nullptr)
  {
    report_error("In FuncDefASTNode, function \"" + root->get_id() +
                  "\" can't be defined in inner scope");
    return false;
  }

  bool has_retval = !__impl::is_type_void(root->get_type());

  if (has_retval)
    __impl::assert_type_int(root->get_type());

  auto funcdef = add_funcdef(root->get_id(), has_retval);

  if (funcdef == nullptr)
  {
    report_error("In FuncDefASTNode, id \"" + root->get_id() +
                  "\" has multiple definitions");
    return false;
  }

  list.push_back(Instruction::new_fdef(funcdef->addr));
  auto inst_fdef = list.back();

  fparam_cnt = 0;
  p_addr_id = 0;

  if (root->get_params())
  {
    push_scope();

    if (!root->get_params()->generate_on(*this))
    {
      report_error("In FuncDefASTNode, failed to generate params");
      return false;
    }

    has_prebuilt_env = true;
  }

  t_addr_unused.clear();

  inst_fdef->arg1 = new Operand(Operand::Type::NUM, fparam_cnt);
  fparam_cnt = 0;

  if (!root->get_block()->generate_on(*this))
  {
    report_error("In FuncDefASTNode, failed to generate block");
    return false;
  }

  if (!has_retval && list.back()->op != Opcode::RET)
    list.push_back(Instruction::new_ret(nullptr));

  list.push_back(Instruction::new_fend(funcdef->addr));

  return true;
}

bool QuadrupleGenerator::generate(const FuncFParamListASTNode *root)
{
  for (auto &param : root->get_param_list())
  {
    if (!param->generate_on(*this))
    {
      report_error("In FuncFParamListASTNode, failed to generate param");
      return false;
    }
  }

  return true;
}

bool QuadrupleGenerator::generate(const FuncFParamASTNode *root)
{
  __impl::assert_type_int(root->get_type());

  std::vector<std::size_t> dim_list;

  for (auto &expr : root->get_dim_list())
  {
    if (expr == nullptr)
      dim_list.push_back(0);
    else
      dim_list.push_back(static_cast<std::size_t>(__impl::ast2int(expr)));
  }

  const auto paramdef = add_paramdef(root->get_id(), dim_list);

  if (paramdef == nullptr)
  {
    report_error("In FuncFParamASTNode, id \"" + root->get_id() +
                  "\" has multiple definitions");
    return false;
  }

  fparam_cnt++;

  return true;
}

bool QuadrupleGenerator::generate(const BlockASTNode *root)
{
  if (!has_prebuilt_env)
    push_scope();

  has_prebuilt_env = false;

  for (auto &item : root->get_block_item_list())
  {
    if (!item->generate_on(*this))
    {
      report_error("In BlockASTNode, failed to generate BlockItem");
      return false;
    }
  }

  pop_scope();

  return true;
}

bool QuadrupleGenerator::generate(const IfElseStmtASTNode *root)
{
  auto &list = instseq.list;

  if (__impl::is_literal(root->get_cond()))
  {
    if (__impl::ast2bool(root->get_cond()))
    {
      if (!root->get_stmt_if()->generate_on(*this))
      {
        report_error("In IfElseStmtASTNode, failed to generate stmt_if");
        return false;
      }
    }
    else if (root->get_stmt_else())
    {
      if (!root->get_stmt_else()->generate_on(*this))
      {
        report_error("In IfElseStmtASTNode, failed to generate stmt_else");
        return false;
      }
    }

    return true;
  }

  auto label_true = new_label();
  auto label_false = new_label();
  auto label_else_end = -1;

  cond_ctx.push({label_true, label_false});

  if (!root->get_cond()->generate_on(*this))
  {
    report_error("In IfElseStmtASTNode, failed to generate cond");
    return false;
  }

  cond_ctx.pop();

  if (last_addr == nullptr)
  {
    report_error("In IfElseStmtASTNode, address of cond is missing");
    return false;
  }

  list.push_back(Instruction::new_jeqz(last_addr, label_false));
  list.push_back(Instruction::new_label(label_true));

  free_temp(last_addr);

  if (!root->get_stmt_if()->generate_on(*this))
  {
    report_error("In IfElseStmtASTNode, failed to generate stmt_if");
    return false;
  }

  if (root->get_stmt_else())
  {
    label_else_end = new_label();
    list.push_back(Instruction::new_jmp(label_else_end));
  }

  list.push_back(Instruction::new_label(label_false));

  if (root->get_stmt_else())
  {
    if (!root->get_stmt_else()->generate_on(*this))
    {
      report_error("In IfElseStmtASTNode, failed to generate stmt_else");
      return false;
    }

    list.push_back(Instruction::new_label(label_else_end));
  }

  return true;
}

bool QuadrupleGenerator::generate(const WhileStmtASTNode *root)
{
  auto &list = instseq.list;

  if (__impl::is_literal(root->get_cond()) && !__impl::ast2bool(root->get_cond()))
    return true;

  auto label_begin = new_label();
  auto label_end = new_label();
  auto label_true = new_label();
  auto label_false = label_end;

  loop_ctx.push({label_begin, label_end});

  list.push_back(Instruction::new_label(label_begin));

  cond_ctx.push({label_true, label_false});

  if (!root->get_cond()->generate_on(*this))
  {
    report_error("In WhileStmtASTNode, failed to generate cond");
    return false;
  }

  cond_ctx.pop();

  if (last_addr == nullptr)
  {
    report_error("In WhileStmtASTNode, address of cond is missing");
    return false;
  }

  list.push_back(Instruction::new_jeqz(last_addr, label_false));
  list.push_back(Instruction::new_label(label_true));

  free_temp(last_addr);

  if (!root->get_stmt()->generate_on(*this))
  {
    report_error("In WhileStmtASTNode, failed to generate stmt");
    return false;
  }

  list.push_back(Instruction::new_jmp(label_begin));

  list.push_back(Instruction::new_label(label_end));

  loop_ctx.pop();

  return true;
}

bool QuadrupleGenerator::generate(const ControlStmtASTNode *root)
{
  auto &list = instseq.list;

  switch (root->get_type())
  {
  case ControlStmtASTNode::Type::CONTINUE:
  {
    if (loop_ctx.empty())
    {
      report_error("In ControlStmtASTNode, loop context is required for \"continue\"");
      return false;
    }
    list.push_back(Instruction::new_jmp(loop_ctx.top().L_begin));
    break;
  }
  case ControlStmtASTNode::Type::BREAK:
  {
    if (loop_ctx.empty())
    {
      report_error("In ControlStmtASTNode, loop context is required for \"break\"");
      return false;
    }
    list.push_back(Instruction::new_jmp(loop_ctx.top().L_end));
    break;
  }
  case ControlStmtASTNode::Type::RETURN:
  {
    if (root->get_expr())
    {
      if (!root->get_expr()->generate_on(*this))
      {
        report_error("In ControlStmtASTNode, failed to generate Exp for \"return\"");
        return false;
      }

      if (last_addr == nullptr)
      {
        report_error("In ControlStmtASTNode, address of Exp is missing for \"return\"");
        return false;
      }

      list.push_back(Instruction::new_ret(last_addr));

      free_temp(last_addr);
    }
    else
    {
      list.push_back(Instruction::new_ret(nullptr));
    }
    break;
  }
  default:
    assert(false);
  }

  return true;
}

bool QuadrupleGenerator::generate(const BinaryExpASTNode *root)
{
  auto &list = instseq.list;

  if (root->is_assign())
  {
    assert(root->get_lhs()->node_type() == ASTNodeType::LVal);
    const auto &lval = dynamic_cast<const LValASTNode *>(root->get_lhs());

    if (!root->get_lhs()->generate_on(*this))
    {
      report_error("In BinaryExpASTNode, failed to generate lhs, op: " +
                    std::to_string(root->get_op()));
      return false;
    }

    assert(last_addr != nullptr);

    Operand *lval_addr = nullptr;
    Operand *lval_offset = nullptr;

    if (lval->get_ix_list().empty())
    {
      lval_addr = last_addr;
    }
    else
    {
      auto inst = list.back();
      list.pop_back();

      assert(inst->op == Opcode::LWORD);

      lval_addr = inst->arg1;
      lval_offset = inst->arg2;

      inst->arg1 = nullptr;
      inst->arg2 = nullptr;
      delete inst;

      assert(lval_addr->val != last_addr->val);

      if (lval_offset->val != last_addr->val)
        free_temp(last_addr);

      delete last_addr;
      last_addr = nullptr;
    }

    if (!root->get_rhs()->generate_on(*this))
    {
      report_error("In BinaryExpASTNode, failed to generate rhs, op: " +
                    std::to_string(root->get_op()));
      return false;
    }

    if (last_addr == nullptr)
    {
      report_error("In BinaryExpASTNode, address of rhs is missing, op: " +
                    std::to_string(root->get_op()));
      return false;
    }

    list.push_back(new Instruction(
      Opcode::SWORD,
      lval_addr,
      last_addr,
      lval_offset
    ));

    assert(!lval_addr || !lval_addr->is_num());
    assert(!addrinfo::is_t(lval_addr->val));

    free_temp(last_addr);
    free_temp(lval_offset);
  }
  else if (root->is_logical())
  {
    if (cond_ctx.empty())
    {
      report_error("In BinaryExpASTNode, condition context is required, op: " +
                    std::to_string(root->get_op()));
      return false;
    }

    auto label_true = cond_ctx.top().L_true;
    auto label_false = cond_ctx.top().L_false;

    switch (root->get_op())
    {
    case BinaryExpASTNode::Operator::LAND:
      label_true = new_label();
      break;
    case BinaryExpASTNode::Operator::LOR:
      label_false = new_label();
      break;
    default:
      assert(false);
    }

    cond_ctx.push({label_true, label_false});

    if (!root->get_lhs()->generate_on(*this))
    {
      report_error("In BinaryExpASTNode, failed to generate lhs, op: " +
                    std::to_string(root->get_op()));
      return false;
    }

    if (last_addr == nullptr)
    {
      report_error("In BinaryExpASTNode, address of lhs is missing, op: " +
                    std::to_string(root->get_op()));
      return false;
    }

    auto addr = new_temp();
    auto dest = new Operand(Operand::Type::ADDR, addr);

    switch (root->get_op())
    {
    case BinaryExpASTNode::Operator::LAND:
    {
      list.push_back(new Instruction(
        Opcode::SWORD,
        new Operand(dest),
        new Operand(Operand::Type::NUM, sysystd::bool2int(false)),
        nullptr
      ));

      list.push_back(Instruction::new_jeqz(last_addr, label_false));
      list.push_back(Instruction::new_label(label_true));
      break;
    }
    case BinaryExpASTNode::Operator::LOR:
    {
      list.push_back(new Instruction(
        Opcode::SWORD,
        new Operand(dest),
        new Operand(Operand::Type::NUM, sysystd::bool2int(true)),
        nullptr
      ));

      list.push_back(Instruction::new_jnez(last_addr, label_true));
      list.push_back(Instruction::new_label(label_false));
      break;
    }
    default:
      assert(false);
    }

    auto lhs = new Operand(last_addr);

    cond_ctx.pop();

    if (!root->get_rhs()->generate_on(*this))
    {
      report_error("In BinaryExpASTNode, failed to generate rhs, op: " +
                    std::to_string(root->get_op()));
      return false;
    }

    if (last_addr == nullptr)
    {
      report_error("In BinaryExpASTNode, address of rhs is missing, op: " +
                    std::to_string(root->get_op()));
      return false;
    }

    auto rhs = last_addr;

    switch (root->get_op())
    {
    case BinaryExpASTNode::Operator::LAND:
      list.push_back(new Instruction(Opcode::LAND, dest, lhs, rhs));
      break;
    case BinaryExpASTNode::Operator::LOR:
      list.push_back(new Instruction(Opcode::LOR, dest, lhs, rhs));
      break;
    default:
      assert(false);
    }

    last_addr = new Operand(dest);

    free_temp(lhs);
    free_temp(rhs);
  }
  else
  {
    if (!root->get_lhs()->generate_on(*this))
    {
      report_error("In BinaryExpASTNode, failed to generate lhs, op: " +
                    std::to_string(root->get_op()));
      return false;
    }

    if (last_addr == nullptr)
    {
      report_error("In BinaryExpASTNode, address of lhs is missing, op: " +
                    std::to_string(root->get_op()));
      return false;
    }

    auto lhs = last_addr;

    if (!root->get_rhs()->generate_on(*this))
    {
      report_error("In BinaryExpASTNode, failed to generate rhs, op: " +
                    std::to_string(root->get_op()));
      return false;
    }

    if (last_addr == nullptr)
    {
      report_error("In BinaryExpASTNode, address of rhs is missing, op: " +
                    std::to_string(root->get_op()));
      return false;
    }

    auto rhs = last_addr;

    Opcode op;

    switch (root->get_op())
    {
    case BinaryExpASTNode::Operator::MUL:   op = Opcode::MUL;   break;
    case BinaryExpASTNode::Operator::DIV:   op = Opcode::DIV;   break;
    case BinaryExpASTNode::Operator::MOD:   op = Opcode::MOD;   break;
    case BinaryExpASTNode::Operator::ADD:   op = Opcode::ADD;   break;
    case BinaryExpASTNode::Operator::SUB:   op = Opcode::SUB;   break;
    case BinaryExpASTNode::Operator::LT:    op = Opcode::LT;    break;
    case BinaryExpASTNode::Operator::GT:    op = Opcode::GT;    break;
    case BinaryExpASTNode::Operator::LE:    op = Opcode::LE;    break;
    case BinaryExpASTNode::Operator::GE:    op = Opcode::GE;    break;
    case BinaryExpASTNode::Operator::EQ:    op = Opcode::EQ;    break;
    case BinaryExpASTNode::Operator::NE:    op = Opcode::NE;    break;
    default:
      assert(false);
    }

    free_temp(lhs);
    free_temp(rhs);

    auto addr = new_temp();
    auto dest = new Operand(Operand::Type::ADDR, addr);

    list.push_back(new Instruction(op, dest, lhs, rhs));

    last_addr = new Operand(dest);
  }

  return true;
}

bool QuadrupleGenerator::generate(const UnaryExpASTNode *root)
{
  auto &list = instseq.list;

  if (!root->get_expr()->generate_on(*this))
  {
    report_error("In UnaryExpASTNode, failed to generate inner UnaryExp");
    return false;
  }

  if (last_addr == nullptr)
  {
    report_error("In UnaryExpASTNode, address of inner UnaryExp is missing");
    return false;
  }

  if (root->get_op() != UnaryExpASTNode::Operator::POS)
  {
    Opcode op;

    switch (root->get_op())
    {
    case UnaryExpASTNode::Operator::NEG:    op = Opcode::NEG;   break;
    case UnaryExpASTNode::Operator::LNOT:   op = Opcode::LNOT;  break;
    default:
      assert(false);
    }

    free_temp(last_addr);

    auto addr = new_temp();
    auto dest = new Operand(Operand::Type::ADDR, addr);

    list.push_back(new Instruction(op, dest, last_addr, nullptr));

    last_addr = new Operand(dest);
  }

  return true;
}

bool QuadrupleGenerator::generate(const FuncCallASTNode *root)
{
  auto &list = instseq.list;
  auto funcdef = get_funcdef(root->get_id());

  if (funcdef == nullptr)
  {
    report_error("In FuncCallASTNode, function \"" + root->get_id() +
                  "\" is undefined");
    return false;
  }

  // we don't check params type

  if (root->get_params())
  {
    st_fparams.push({});

    if (!root->get_params()->generate_on(*this))
    {
      report_error("In FuncCallASTNode, failed to generate params");
      return false;
    }

    for (auto &inst : st_fparams.top())
      list.push_back(inst);

    st_fparams.pop();
  }

  // patch for starttime / stoptime

  if (root->get_id() == "starttime" || root->get_id() == "stoptime")
  {
    auto p0 = new Operand(Operand::Type::NUM, 0);
    list.push_back(Instruction::new_param(p0));
  }

  if (funcdef->has_retval)
  {
    auto addr = new_temp();
    auto dest = new Operand(Operand::Type::ADDR, addr);

    list.push_back(Instruction::new_call(dest, funcdef->addr));
    last_addr = new Operand(dest);
  }
  else
  {
    list.push_back(Instruction::new_call(nullptr, funcdef->addr));
    last_addr = nullptr;
  }

  return true;
}

bool QuadrupleGenerator::generate(const FuncRParamListASTNode *root)
{
  const auto &param_list = root->get_param_list();

  for (std::size_t i = 0, n = param_list.size(); i < n; ++i)
  {
    if (!param_list[i]->generate_on(*this))
    {
      report_error("In FuncRParamListASTNode, failed to generate param " +
                    std::to_string(i));
      return false;
    }

    if (last_addr == nullptr)
    {
      report_error("In FuncRParamListASTNode, address of param " +
                    std::to_string(i) + " is missing");
      return false;
    }

    st_fparams.top().push_back(Instruction::new_param(last_addr));
  }

  return true;
}

bool QuadrupleGenerator::generate(const BareStmtASTNode *root)
{
  if (root->get_expr())
  {
    if (!root->get_expr()->generate_on(*this))
    {
      report_error("In BareStmtASTNode, failed to genrate Exp");
      return false;
    }

    delete last_addr;
    last_addr = nullptr;
  }

  return true;
}

bool QuadrupleGenerator::generate(const LValASTNode *root)
{
  auto &list = instseq.list;
  auto vardef = get_vardef(root->get_id());

  if (vardef == nullptr)
  {
    report_error("In LValASTNode, id \"" + root->get_id() + "\" is undefined");
    return false;
  }

  if (root->get_ix_list().empty())
  {
    last_addr = new Operand(Operand::Type::ADDR, vardef->addr);
  }
  else
  {
    const auto &dim_list = vardef->dim_list;
    const auto &ix_list = root->get_ix_list();

    if (ix_list.size() != dim_list.size())
    {
      report_error("In LValASTNode, incorrect number of dimensions of id \"" +
                    root->get_id() + "\", expect: " + std::to_string(dim_list.size()) +
                    ", current: " + std::to_string(ix_list.size()));
      return false;
    }

    std::size_t array_size = 1;
    std::size_t literal_offset = 0;
    std::vector<Operand *> addr_list;

    for (std::size_t i = ix_list.size() - 1, n = ix_list.size(); i < n; --i)
    {
      if (!ix_list[i]->generate_on(*this))
      {
        report_error("In LValASTNode, failed to generate Exp at dim " +
                      std::to_string(i));
        for (auto &opnd : addr_list)
          delete opnd;
        return false;
      }

      if (last_addr == nullptr)
      {
        report_error("In LValASTNode, address of Exp at dim " +
                      std::to_string(i) + " is missing");
        for (auto &opnd : addr_list)
          delete opnd;
        return false;
      }

      if (last_addr->is_num())
      {
        literal_offset += last_addr->val * array_size;
        delete last_addr;
        last_addr = nullptr;
      }
      else if (array_size == 1)
      {
        addr_list.push_back(last_addr);
      }
      else
      {
        auto lhs = last_addr;
        auto rhs = new Operand(Operand::Type::NUM, array_size);

        free_temp(lhs);

        auto addr = new_temp();
        auto dest = new Operand(Operand::Type::ADDR, addr);

        list.push_back(new Instruction(Opcode::MUL, dest, lhs, rhs));

        addr_list.push_back(new Operand(dest));
      }

      array_size *= dim_list[i];
    }

    auto last_dest = new Operand(Operand::Type::NUM, literal_offset);

    for (auto lhs : addr_list)
    {
      if (last_dest->is_num() && last_dest->val == 0)
      {
        delete last_dest;
        last_dest = lhs;
      }
      else
      {
        free_temp(lhs);
        free_temp(last_dest);

        auto addr = new_temp();
        auto dest = new Operand(Operand::Type::ADDR, addr);

        list.push_back(new Instruction(Opcode::ADD, dest, lhs, last_dest));

        last_dest = new Operand(dest);
      }
    }

    if (last_dest->is_num())
    {
      last_dest->val *= vardef->type_size;
    }
    else
    {
      free_temp(last_dest);

      auto addr = new_temp();
      auto dest = new Operand(Operand::Type::ADDR, addr);

      list.push_back(new Instruction(
        Opcode::MUL,
        dest,
        last_dest,
        new Operand(Operand::Type::NUM, vardef->type_size)
      ));

      last_dest = new Operand(dest);
    }

    free_temp(last_dest);

    auto addr = new_temp();
    auto dest = new Operand(Operand::Type::ADDR, addr);

    list.push_back(new Instruction(
      Opcode::LWORD,
      dest,
      new Operand(Operand::Type::ADDR, vardef->addr),
      last_dest
    ));

    last_addr = new Operand(dest);
  }

  return true;
}

bool QuadrupleGenerator::generate(const NumberASTNode *root)
{
  last_addr = new Operand(Operand::Type::NUM, root->get_val());
  return true;
}

bool QuadrupleGenerator::generate(const PrimTypeASTNode *root)
{
  assert(false);
  return true;
}

} // namespace quad

} // namespace backend

} // namespace sysyc
