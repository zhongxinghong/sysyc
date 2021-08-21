//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: context.cc
//  Created Date: 2021-05-22
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "backend/quad/ast2quad/ast2quad.h"
#include "backend/quad/ast2quad/context.h"
#include "backend/quad/ast2quad/ivlist.h"

namespace sysyc
{
namespace backend
{
namespace quad
{
using namespace middle::quad;

using vardef_t = __impl::vardef_s;
using funcdef_t = __impl::funcdef_s;

namespace __impl
{
inline bool has_def(const quadgen_scope_s *env, const std::string &id)
{
  if (env->vardef_map.find(id) != env->vardef_map.end())
    return true;

  if (env->funcdef_map.find(id) != env->funcdef_map.end())
    return true;

  return false;
}

} // namespace __impl

void QuadrupleGenerator::push_scope()
{
  env = new __impl::quadgen_scope_s(env);
}

void QuadrupleGenerator::pop_scope()
{
  assert(env != nullptr);

  auto old_env = env;
  env = env->parent;
  delete old_env;
}

const vardef_t *QuadrupleGenerator::add_vardef(
  const std::string &id, const std::vector<std::size_t> &dim_list)
{
  if (has_def(env, id))
    return nullptr;

  auto vardef = &env->vardef_map[id];

  vardef->id = id;
  vardef->dim_list = dim_list;
  vardef->type_size = sizeof(sysystd::int_t);
  vardef->addr = addrinfo::embed_type(T_addr_id++, AddressType::T);

  assert(vardef->type_size == 4);

  return vardef;
}

const vardef_t *QuadrupleGenerator::add_paramdef(
  const std::string &id, const std::vector<std::size_t> &dim_list)
{
  if (has_def(env, id))
    return nullptr;

  auto vardef = &env->vardef_map[id];

  vardef->id = id;
  vardef->dim_list = dim_list;
  vardef->type_size = sizeof(sysystd::int_t);
  vardef->addr = addrinfo::embed_type(p_addr_id++, AddressType::p);

  assert(vardef->type_size == 4);

  return vardef;
}

const funcdef_t *QuadrupleGenerator::add_funcdef(
  const std::string &id, const std::string &link_id, bool has_retval)
{
  if (has_def(env, id))
    return nullptr;

  auto funcdef = &env->funcdef_map[id];

  funcdef->sysy_id = id;
  funcdef->link_id = link_id;
  funcdef->has_retval = has_retval;
  funcdef->addr = addrinfo::embed_type(f_addr_id++, AddressType::f);

  instseq.funcaddr_map[funcdef->addr] = link_id;

  return funcdef;
}

const vardef_t *QuadrupleGenerator::get_vardef(
  const std::string &id)
{
  for (auto cenv = env; cenv != nullptr; cenv = cenv->parent)
  {
    auto it = cenv->vardef_map.find(id);

    if (it != cenv->vardef_map.end())
      return &it->second;
  }

  return nullptr;
}

const funcdef_t *QuadrupleGenerator::get_funcdef(
  const std::string &id)
{
  for (auto cenv = env; cenv != nullptr; cenv = cenv->parent)
  {
    auto it = cenv->funcdef_map.find(id);

    if (it != cenv->funcdef_map.end())
      return &it->second;
  }

  return nullptr;
}

addr_t QuadrupleGenerator::new_temp()
{
  auto &list = instseq.list;

  if (!t_addr_unused.empty())
  {
    auto addr = t_addr_unused.back();
    t_addr_unused.pop_back();
    return addr;
  }

  auto addr = addrinfo::embed_type(t_addr_id++, AddressType::t);
  list.push_back(Instruction::new_vdef(addr, nullptr));
  return addr;
}

void QuadrupleGenerator::free_temp(const addr_t &addr)
{
  assert(addrinfo::is_t(addr));
  t_addr_unused.push_back(addr);
}

void QuadrupleGenerator::free_temp(const Operand *opnd)
{
  if (!opnd || opnd->is_num() || !addrinfo::is_t(opnd->val))
    return;

  // TODO: delete this assert in the future

  for (std::size_t i = 0, n = t_addr_unused.size(); i < n; ++i)
    assert(t_addr_unused[i] != static_cast<std::size_t>(opnd->val));

  t_addr_unused.push_back(opnd->val);
}

} // namespace quad

} // namespace backend

} // namespace sysyc
