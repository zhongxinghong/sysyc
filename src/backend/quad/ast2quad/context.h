//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: context.h
//  Created Date: 2021-05-22
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_BACKEND_QUAD_AST2QUAD_CONTEXT_H__
#define __SYSYC_BACKEND_QUAD_AST2QUAD_CONTEXT_H__

#include <stack>
#include <vector>
#include <unordered_map>

#include "middle/quad/instseq.h"
#include "middle/quad/address.h"

namespace sysyc
{
namespace backend
{
namespace quad
{
namespace __impl
{
struct vardef_s
{
  std::string id;
  std::vector<std::size_t> dim_list;
  std::size_t type_size;
  middle::quad::addr_t addr;
};

struct funcdef_s
{
  std::string sysy_id;
  std::string link_id;
  bool has_retval;
  middle::quad::addr_t addr;
};

struct quadgen_scope_s
{
  quadgen_scope_s(quadgen_scope_s *parent) : parent(parent) {}

  quadgen_scope_s *parent;
  std::unordered_map<std::string, vardef_s> vardef_map;
  std::unordered_map<std::string, funcdef_s> funcdef_map;
};

struct loop_ctx_s
{
  std::size_t L_begin;
  std::size_t L_end;
};

struct cond_ctx_s
{
  std::size_t L_true;
  std::size_t L_false;
};

} // namespace __impl

} // namespace quad

} // namespace backend

} // namespace sysyc

#endif // __SYSYC_BACKEND_QUAD_AST2QUAD_CONTEXT_H__
