//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: ivlist.h
//  Created Date: 2021-05-22
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_BACKEND_QUAD_AST2QUAD_IVLIST_H__
#define __SYSYC_BACKEND_QUAD_AST2QUAD_IVLIST_H__

#include <stack>
#include <exception>

#include "middle/ast/node.h"
#include "middle/ast/generator.h"

namespace sysyc
{
namespace backend
{
namespace quad
{
namespace __impl
{
class quadgen_ivlist_iterator
{
public:
  class exception : public std::exception
  {
  public:
    exception(const std::string &errmsg) : errmsg(errmsg) {}
    virtual const char *what() const noexcept override { return errmsg.c_str(); }

  private:
    std::string errmsg;
  };

public:
  quadgen_ivlist_iterator(const middle::ast::ASTNode *ast,
                          const std::vector<std::size_t> &dim_list,
                          middle::ast::IASTGenerator *generator);

  bool next();

  std::size_t get_ix() const { return pos - 1; }

private:
  struct cursor_s
  {
    const std::vector<middle::ast::ASTNode *> &list;
    std::size_t dim;
    std::size_t pos;
    std::size_t start_pos;

    const middle::ast::ASTNode *ast() const { return list[pos]; }
  };

  middle::ast::IASTGenerator *generator;
  bool const_;
  std::vector<std::size_t> dim_list;
  std::vector<std::size_t> dim_size;
  std::stack<cursor_s> st;
  std::size_t pos;
};

} // namespace __impl

} // namespace quad

} // namespace backend

} // namespace sysyc

#endif // __SYSYC_BACKEND_QUAD_AST2QUAD_IVLIST_H__
