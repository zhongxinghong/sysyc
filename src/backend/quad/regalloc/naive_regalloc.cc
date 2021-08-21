//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: naive_regalloc.cc
//  Created Date: 2021-05-25
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "middle/quad/address.h"
#include "middle/quad/register.h"
#include "backend/quad/regalloc/naive_regalloc.h"

namespace sysyc
{
namespace backend
{
namespace quad
{
using namespace middle::quad;

namespace __impl
{
inline bool is_reg_temp(const Operand *opnd)
{
  return opnd->is_num(); // || (opnd->is_addr() && addrinfo::is_t(opnd->val));
}

inline bool is_num_int10(const Operand *opnd)
{
  return opnd->is_num() && sysystd::is_int10(opnd->val);
}

inline bool is_reg_zero(const std::size_t &reg)
{
  return reg == Register::x0;
}

inline bool is_reg_reserved(const std::size_t &reg)
{
  return reg == Register::t6;
}

inline bool is_reg_allocable(const std::size_t &reg)
{
  return static_cast<std::size_t>(Register::s0) <= reg
      && static_cast<std::size_t>(Register::t5) >= reg;
}

inline std::size_t reg_allocable_start()
{
  return static_cast<std::size_t>(Register::s0);
}

inline std::size_t reg_allocable_end()
{
  return static_cast<std::size_t>(Register::t5);
}

inline std::size_t reg_writable_start()
{
  return static_cast<std::size_t>(Register::s0);
}

inline std::size_t reg_writable_end()
{
  return static_cast<std::size_t>(Register::a7);
}

} // namespace __impl

NaiveRegisterAllocator::NaiveRegisterAllocator(misc::Logger &logger)
  : env(nullptr), p_instseq(nullptr), p_it(nullptr), logger(logger)
{
  regtab.resize(REG_ALL_CNT);
  reset_regtab();
}

void NaiveRegisterAllocator::report_error(const std::string &errmsg)
{
  logger.error("[NaiveRegisterAllocator] " + errmsg);
}

void NaiveRegisterAllocator::push_scope()
{
  env = new scope_t(env);
}

void NaiveRegisterAllocator::pop_scope()
{
  assert(env != nullptr);

  for (auto &entry : env->sybtab)
  {
    auto reg = entry.second.reg_addr;

    if (reg > 0)
    {
      regtab[reg].is_dirty = false; // prevent "store" instruction generation
      reg_free(reg);
    }
  }

  auto old_env = env;
  env = env->parent;
  delete old_env;
}

void NaiveRegisterAllocator::reset_regtab()
{
  regtab_free = 0;

  for (std::size_t i = 0, n = regtab.size(); i < n; ++i)
  {
    if (env != nullptr && !regtab[i].is_number && regtab[i].sybtab_entry)
      regtab[i].sybtab_entry->reg_addr = 0;

    regtab[i].is_dirty = false;
    regtab[i].is_number = false;
    regtab[i].sybtab_entry = nullptr;

    if (__impl::is_reg_allocable(i))
      regtab_free++;
  }
}

void NaiveRegisterAllocator::reg_prealloc(std::size_t reg_cnt)
{
  if (regtab_free >= reg_cnt)
    return;

  auto st = __impl::reg_allocable_start();
  auto ed = __impl::reg_allocable_end();

  flush_regtab_number(st, ed);

  if (regtab_free >= reg_cnt)
    return;

  assert(!env->is_slfree);

  flush_regtab(st, ed);

  assert(regtab_free >= reg_cnt);
}

addr_t NaiveRegisterAllocator::reg_alloc(sysystd::int_t number)
{
  auto &list = p_instseq->list;
  auto &it = *p_it;

  if (number == 0)
    return static_cast<addr_t>(Register::x0);

  auto st = __impl::reg_allocable_start();
  auto ed = __impl::reg_allocable_end();

  for (std::size_t i = st; i <= ed; ++i)
  {
    if (regtab[i].is_used())
      continue;

    regtab[i].is_number = true;

    assert(__impl::is_reg_allocable(i));
    regtab_free--;

    list.insert(it, new Instruction(
      Opcode::SWORD,
      new Operand(Operand::Type::ADDR, i),
      new Operand(Operand::Type::NUM, number),
      nullptr
    ));

    return i;
  }

  assert(false);
  return 0;
}

addr_t NaiveRegisterAllocator::reg_alloc(
  NaiveRegisterAllocator::sybtab_entry_t *entry, bool load)
{
  assert(entry != nullptr);

  if (entry->reg_addr > 0)
  {
    auto reg = entry->reg_addr;
    assert(!regtab[reg].is_number && regtab[reg].sybtab_entry == entry);
    return reg;
  }

  auto &list = p_instseq->list;
  auto &it = *p_it;

  auto st = __impl::reg_allocable_start();
  auto ed = __impl::reg_allocable_end();

  for (std::size_t i = st; i <= ed; ++i)
  {
    if (regtab[i].is_used())
      continue;

    entry->reg_addr = i;
    regtab[i].sybtab_entry = entry;

    assert(__impl::is_reg_allocable(i));
    regtab_free--;

    if (load)
    {
      Opcode op = entry->is_array ? Opcode::LOADADDR : Opcode::LOAD;
      Operand *arg1 = nullptr;

      if (addrinfo::is_v(entry->stack_addr))
        arg1 = new Operand(Operand::Type::ADDR, entry->stack_addr);
      else
        arg1 = new Operand(Operand::Type::NUM, entry->stack_addr);

      list.insert(it, new Instruction(
        op,
        new Operand(Operand::Type::ADDR, i),
        arg1,
        nullptr
      ));
    }

    return i;
  }

  assert(false);
  return 0;
}

addr_t NaiveRegisterAllocator::opnd2reg_dest(Operand **p_opnd)
{
  auto opnd = *p_opnd;
  assert(opnd->is_addr());

  auto entry = env->get_vardef(opnd->val);
  opnd->val = reg_alloc(entry, entry->is_array);
  return opnd->val;
}

addr_t NaiveRegisterAllocator::opnd2reg_argx(Operand **p_opnd)
{
  auto opnd = *p_opnd;

  if (opnd->is_addr())
  {
    auto entry = env->get_vardef(opnd->val);
    bool load = !env->is_slfree || entry->env != env || entry->is_array;
    opnd->val = reg_alloc(entry, load);
    return opnd->val;
  }
  else
  {
    opnd->val = reg_alloc(opnd->val);
    opnd->type = Operand::Type::ADDR;
    return opnd->val;
  }
}

void NaiveRegisterAllocator::reg_free(addr_t reg)
{
  auto &list = p_instseq->list;
  auto &it = *p_it;

  if (__impl::is_reg_zero(reg) || __impl::is_reg_reserved(reg))
  {
    assert(!regtab[reg].is_used());
    return;
  }

  if (!regtab[reg].is_number)
  {
    auto entry = regtab[reg].sybtab_entry;

    if (env->is_slfree && entry != nullptr && entry->env == env)
      return;

    if (entry != nullptr && !entry->is_array && regtab[reg].is_dirty)
    {
      if (addrinfo::is_v(entry->stack_addr))
      {
        // "store" Reg VARIABLE

        // loadaddr v0 t6
        // t6[0] = Rx

        list.insert(it, new Instruction(
          Opcode::LOADADDR,
          new Operand(Register::t6),
          new Operand(Operand::Type::ADDR, entry->stack_addr),
          nullptr
        ));

        list.insert(it, new Instruction(
          Opcode::SWORD,
          new Operand(Register::t6),
          new Operand(Operand::Type::ADDR, reg),
          new Operand(Operand::Type::NUM, 0)
        ));
      }
      else
      {
        // "store" Reg NUM

        list.insert(it, new Instruction(
          Opcode::STORE,
          new Operand(Operand::Type::NUM, entry->stack_addr),
          new Operand(Operand::Type::ADDR, reg),
          nullptr
        ));
      }
    }

    if (env->is_slfree)
      return;

    if (entry != nullptr)
      entry->reg_addr = 0;
  }

  if (regtab[reg].is_used() && __impl::is_reg_allocable(reg))
    regtab_free++;

  regtab[reg].is_dirty = false;
  regtab[reg].is_number = false;
  regtab[reg].sybtab_entry = nullptr;
}

void NaiveRegisterAllocator::flush_regtab()
{
  auto st = __impl::reg_writable_start();
  auto ed = __impl::reg_writable_end();
  flush_regtab(st, ed);
}

void NaiveRegisterAllocator::flush_regtab(std::size_t st, std::size_t ed)
{
  for (std::size_t i = st; i <= ed; ++i)
    reg_free(i);
}

void NaiveRegisterAllocator::flush_regtab_nonlocal()
{
  auto st = __impl::reg_writable_start();
  auto ed = __impl::reg_writable_end();

  for (std::size_t i = st; i <= ed; ++i)
  {
    if (regtab[i].is_number)
      reg_free(i);
    else if (regtab[i].sybtab_entry && regtab[i].sybtab_entry->env != env)
      reg_free(i);
  }
}

void NaiveRegisterAllocator::flush_regtab_number(std::size_t st, std::size_t ed)
{
  for (std::size_t i = st; i <= ed; ++i)
  {
    if (regtab[i].is_number)
      reg_free(i);
  }
}

void NaiveRegisterAllocator::slfree_analyse()
{
  auto it = *p_it;
  auto eit = p_instseq->list.end();

  std::size_t cnt = 0;

  assert(env->parent != nullptr && env->parent->parent == nullptr);

  cnt += env->parent->sybtab.size();

  assert((*it)->op == Opcode::FDEF);

  for (++it; (*it)->op != Opcode::FEND; ++it)
  {
    assert(it != eit);

    if ((*it)->op == Opcode::CALL)
      return;

    if ((*it)->op == Opcode::VDEF)
      cnt++;
  }

  if (cnt > regtab_free - 2)
    return;

  env->is_slfree = true;

  it = (*p_it)++;

  for (auto &entry : env->parent->sybtab)
  {
    reg_alloc(&entry.second, true);
    it++;
  }

  *p_it = it;

  return;
}

bool NaiveRegisterAllocator::run(InstructionList &instseq)
{
  p_instseq = &instseq;

  auto &list = instseq.list;
  std::size_t param_cnt = 0;

  push_scope();

  for (auto it = list.begin(), e = list.end(); it != e; )
  {
    p_it = &it;
    auto &inst = *it;

    switch (inst->op)
    {
    case Opcode::LNOT:
    case Opcode::NEG:
    {
      assert(!inst->arg1->is_num());

      reg_prealloc(2);

      auto is_arg1_temp = __impl::is_reg_temp(inst->arg1);

      // Reg = UnaryOp Reg

      auto reg_arg1 = opnd2reg_argx(&inst->arg1);
      auto reg_dest = opnd2reg_dest(&inst->dest);

      regtab[reg_dest].is_dirty = true;

      if (is_arg1_temp)
        reg_free(reg_arg1);

      break;
    }
    case Opcode::NE:
    case Opcode::EQ:
    case Opcode::GT:
    case Opcode::LT:
    case Opcode::GE:
    case Opcode::LE:
    case Opcode::LAND:
    case Opcode::LOR:
    case Opcode::ADD:
    case Opcode::SUB:
    case Opcode::MUL:
    case Opcode::DIV:
    case Opcode::MOD:
    {
      assert(!(inst->arg1->is_num() && inst->arg2->is_num()));
      assert(!(inst->arg1->is_num() && inst->op == Opcode::LAND));

      if (inst->arg1->is_addr() && inst->arg2->is_num())
      {
        // Reg = Reg op NUM

        reg_prealloc(2);

        auto is_arg1_temp = __impl::is_reg_temp(inst->arg1);

        auto reg_arg1 = opnd2reg_argx(&inst->arg1);
        auto reg_dest = opnd2reg_dest(&inst->dest);

        regtab[reg_dest].is_dirty = true;

        if (is_arg1_temp)
          reg_free(reg_arg1);
      }
      else
      {
        // Reg = NUM op Reg
        // Reg = Reg op Reg

        reg_prealloc(3);

        auto is_arg1_temp = __impl::is_reg_temp(inst->arg1);
        auto is_arg2_temp = __impl::is_reg_temp(inst->arg2);

        auto reg_arg1 = opnd2reg_argx(&inst->arg1);
        auto reg_arg2 = opnd2reg_argx(&inst->arg2);
        auto reg_dest = opnd2reg_dest(&inst->dest);

        regtab[reg_dest].is_dirty = true;

        if (is_arg1_temp)
          reg_free(reg_arg1);
        if (is_arg2_temp)
          reg_free(reg_arg2);
      }
      break;
    }
    case Opcode::JNE:
    case Opcode::JEQ:
    case Opcode::JGT:
    case Opcode::JLT:
    case Opcode::JGE:
    case Opcode::JLE:
    {
      assert(!(inst->arg1->is_num() && inst->arg2->is_num()));

      reg_prealloc(2);

      auto is_arg1_temp = __impl::is_reg_temp(inst->arg1);
      auto is_arg2_temp = __impl::is_reg_temp(inst->arg2);

      auto reg_arg1 = opnd2reg_argx(&inst->arg1);
      auto reg_arg2 = opnd2reg_argx(&inst->arg2);

      if (is_arg1_temp)
        reg_free(reg_arg1);
      if (is_arg2_temp)
        reg_free(reg_arg2);

      flush_regtab();

      break;
    }
    case Opcode::JEQZ:
    case Opcode::JNEZ:
    {
      assert(!inst->arg1->is_num());

      reg_prealloc(1);

      auto is_arg1_temp = __impl::is_reg_temp(inst->arg1);

      auto reg_arg1 = opnd2reg_argx(&inst->arg1);

      if (is_arg1_temp)
        reg_free(reg_arg1);

      flush_regtab();

      break;
    }
    case Opcode::JMP:
    {
      flush_regtab();
      break;
    }
    case Opcode::SWORD:
    {
      if (inst->arg2 == nullptr || __impl::is_num_int10(inst->arg2))
      {
        // Reg "=" Reg
        // Reg "=" NUM
        // Reg "[" NUM "]" "=" Reg

        auto is_arg1_temp = __impl::is_reg_temp(inst->arg1);

        reg_prealloc(2);

        auto reg_arg1 = opnd2reg_argx(&inst->arg1);
        auto reg_dest = opnd2reg_dest(&inst->dest);

        regtab[reg_dest].is_dirty = true;

        if (is_arg1_temp)
          reg_free(reg_arg1);
      }
      else
      {
        // Reg [Reg] = Reg / NUM
        // dest arg2   arg1

        // loadaddr dest Rx
        // load arg2 Ry
        // load arg1 Rz
        // t6 = Rx + Ry
        // t6[0] = Rz

        reg_prealloc(3);

        auto is_dest_temp = __impl::is_reg_temp(inst->dest);
        auto is_arg2_temp = __impl::is_reg_temp(inst->arg2);
        auto is_arg1_temp = __impl::is_reg_temp(inst->arg1);

        // this 'dest' is actually an argx for 't6 = Rx + Ry'
        auto reg_dest = opnd2reg_argx(&inst->dest);

        auto reg_arg2 = opnd2reg_argx(&inst->arg2);
        auto reg_arg1 = opnd2reg_argx(&inst->arg1);

        list.insert(it, new Instruction(
          Opcode::ADD,
          new Operand(Register::t6),
          new Operand(Operand::Type::ADDR, reg_dest),
          new Operand(Operand::Type::ADDR, reg_arg2)
        ));

        delete inst;

        inst = new Instruction(
          Opcode::SWORD,
          new Operand(Register::t6),
          new Operand(Operand::Type::ADDR, reg_arg1),
          new Operand(Operand::Type::NUM, 0)
        );

        if (is_dest_temp)
          reg_free(reg_dest);
        if (is_arg2_temp)
          reg_free(reg_arg2);
        if (is_arg1_temp)
          reg_free(reg_arg1);
      }
      break;
    }
    case Opcode::LWORD:
    {
      if (inst->arg2->is_num())
      {
        // Reg "=" Reg "[" NUM "]"

        reg_prealloc(2);

        auto is_arg1_temp = __impl::is_reg_temp(inst->arg1);

        auto reg_arg1 = opnd2reg_argx(&inst->arg1);
        auto reg_dest = opnd2reg_dest(&inst->dest);

        regtab[reg_dest].is_dirty = true;

        if (is_arg1_temp)
          reg_free(reg_arg1);
      }
      else
      {
        // Reg = Reg [Reg]
        // dest  arg1 arg2

        // loadaddr arg1 Rx
        // load arg2 Ry
        // load dest Rz
        // t6 = Rx + Ry
        // Rz = t6[0]

        reg_prealloc(3);

        auto is_arg1_temp = __impl::is_reg_temp(inst->arg1);
        auto is_arg2_temp = __impl::is_reg_temp(inst->arg2);

        auto reg_arg1 = opnd2reg_argx(&inst->arg1);
        auto reg_arg2 = opnd2reg_argx(&inst->arg2);
        auto reg_dest = opnd2reg_dest(&inst->dest);

        list.insert(it, new Instruction(
          Opcode::ADD,
          new Operand(Register::t6),
          new Operand(Operand::Type::ADDR, reg_arg1),
          new Operand(Operand::Type::ADDR, reg_arg2)
        ));

        delete inst;

        inst = new Instruction(
          Opcode::LWORD,
          new Operand(Operand::Type::ADDR, reg_dest),
          new Operand(Register::t6),
          new Operand(Operand::Type::NUM, 0)
        );

        regtab[reg_dest].is_dirty = true;

        if (is_arg1_temp)
          reg_free(reg_arg1);
        if (is_arg2_temp)
          reg_free(reg_arg2);
      }
      break;
    }
    case Opcode::VDEF:
    {
      assert(inst->dest->is_addr());
      addr_t addr = inst->dest->val;

      bool is_global = (env->parent == nullptr);
      bool is_array = (inst->arg1 != nullptr);

      switch (addrinfo::get_type(addr))
      {
      case AddressType::T:
      {
        auto entry = env->add_vardef(addr);
        assert(entry != nullptr);

        entry->is_array = is_array;

        auto addr = env->stack_size;

        if (is_global)
        {
          addr = addrinfo::embed_type(addr, AddressType::v);
          inst->dest->val = addr;

          entry->stack_addr = addr;
          env->stack_size += 1;
        }
        else
        {
          entry->stack_addr = addr;

          if (is_array)
            env->stack_size += inst->arg1->val / sizeof(sysystd::int_t);
          else
            env->stack_size += 1;
        }
        break;
      }
      case AddressType::t:
      {
        // reorder_decl pass should be run first
        assert(!is_global);

        assert(!is_array);

        auto entry = env->add_vardef(addr);
        assert(entry != nullptr);

        entry->is_array = false;
        entry->stack_addr = env->stack_size++;
        break;
      }
      default:
        assert(false);
      }

      if (!(addrinfo::is_T(addr) && is_global))
      {
        delete inst;
        it = list.erase(it);
        continue;
      }

      break;
    }
    case Opcode::FDEF:
    {
      reset_regtab();
      push_scope();

      env->inst_fdef = inst;

      assert(inst->arg2 == nullptr);
      inst->arg2 = new Operand(Operand::Type::NUM, 0);

      if (static_cast<std::size_t>(inst->arg1->val) > REG_FUNC_PARAM_CNT)
      {
        report_error("In FDEF, too many func param");
        return false;
      }

      env->stack_size = inst->arg1->val;

      auto a0 = static_cast<std::size_t>(Register::a0);

      for (std::size_t i = 0, n = inst->arg1->val; i < n; ++i)
      {
        auto reg = a0 + i;
        auto addr = addrinfo::embed_type(i, AddressType::p);
        auto entry = env->add_vardef(addr);

        entry->stack_addr = i;
        entry->reg_addr = reg;

        regtab[reg].sybtab_entry = entry;
        regtab[reg].is_dirty = true;
      }

      slfree_analyse();
      break;
    }
    case Opcode::FEND:
    {
      assert(param_cnt == 0);

      // TODO: shrink stack_size if is_slfree
      env->inst_fdef->arg2->val = env->stack_size;

      // if (env->is_slfree)
      // {
      //   auto it = std::next(env->slfree_vload_it, 1);

      //   for (auto &entry : env->slfree_vload)
      //     list.splice(it, list, entry.second);
      // }

      pop_scope();
      reset_regtab();
      break;
    }
    case Opcode::LABEL:
    {
      flush_regtab();
      break;
    }
    case Opcode::PARAM:
    {
      std::size_t pn = param_cnt++;

      if (pn == REG_FUNC_PARAM_CNT)
      {
        report_error("In PARAM, too many func param");
        return false;
      }

      auto ax = Register::a0 + pn;

      reg_free(static_cast<std::size_t>(ax));

      if (inst->arg1->is_num())
      {
        auto val = inst->arg1->val;

        delete inst;

        // ax = NUM

        inst = new Instruction(
          Opcode::SWORD,
          new Operand(ax),
          new Operand(Operand::Type::NUM, val),
          nullptr
        );
      }
      else
      {
        auto is_arg1_temp = __impl::is_reg_temp(inst->arg1);

        reg_prealloc(1);

        auto reg_arg1 = opnd2reg_argx(&inst->arg1);

        delete inst;

        // ax = Reg

        inst = new Instruction(
          Opcode::SWORD,
          new Operand(ax),
          new Operand(Operand::Type::ADDR, reg_arg1),
          nullptr
        );

        if (is_arg1_temp)
          reg_free(reg_arg1);
      }
      break;
    }
    case Opcode::CALL:
    {
      param_cnt = 0;
      flush_regtab();

      if (inst->dest != nullptr)
      {
        reg_prealloc(1);

        auto reg_dest = opnd2reg_dest(&inst->dest);

        delete inst->dest;
        inst->dest = nullptr;

        it = list.insert(std::next(it, 1), new Instruction(
          Opcode::SWORD,
          new Operand(Operand::Type::ADDR, reg_dest),
          new Operand(Register::a0),
          nullptr
        ));

        regtab[reg_dest].is_dirty = true;
      }
      break;
    }
    case Opcode::RET:
    {
      flush_regtab_nonlocal();

      if (inst->arg1 != nullptr)
      {
        reg_prealloc(1);

        auto reg_arg1 = opnd2reg_argx(&inst->arg1);

        list.insert(it, new Instruction(
          Opcode::SWORD,
          new Operand(Register::a0),
          new Operand(Operand::Type::ADDR, reg_arg1),
          nullptr
        ));

        delete inst->arg1;
        inst->arg1 = nullptr;
      }
      break;
    }
    default:
      assert(false);
    }

    it++;
  }

  return true;
}

} // namespace quad

} // namespace backend

} // namespace sysyc
