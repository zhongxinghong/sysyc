//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: ivlist.cc
//  Created Date: 2021-05-22
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include <cassert>

#include "backend/quad/ast2quad/ivlist.h"

namespace sysyc
{
namespace backend
{
namespace quad
{
namespace __impl
{
using namespace middle::ast;

quadgen_ivlist_iterator::quadgen_ivlist_iterator(
  const ASTNode *ast,
  const std::vector<std::size_t> &dim_list,
  IASTGenerator *generator)
  : generator(generator), dim_list(dim_list), pos(0)
{
  std::size_t i, n = dim_list.size();

  dim_size.resize(n + 1);
  dim_size[n] = 1;

  for (i = n - 1; i < n; --i)
    dim_size[i] = dim_size[i + 1] * dim_list[i];

  assert(ast->node_type() == ASTNodeType::InitValList);
  const auto list_node = dynamic_cast<const InitValListASTNode *>(ast);

  const_ = list_node->is_const();
  st.push({list_node->get_init_val_list(), 0, 0, 0});
}

bool quadgen_ivlist_iterator::next()
{
  if (st.empty())
  {
    assert(pos == dim_size[0]);
    return false;
  }

  auto &cur = st.top();

  if (cur.pos == cur.list.size())
  {
    std::size_t size = dim_size[cur.dim];
    std::size_t rem = pos % size;

    if (rem > 0 || cur.pos == 0)
      pos += size - rem;

    st.pop();
    return next();
  }

  if (pos + 1 - cur.start_pos > dim_size[cur.dim])
  {
    throw exception("too many expression of InitValList at pos " +
                    std::to_string(pos) + ", expect " +
                    std::to_string(dim_size[cur.dim]) + " elements");
  }

  const auto &ast = cur.ast();

  if (ast->node_type() == ASTNodeType::InitValList)
  {
    std::size_t dim = cur.dim;

    while (pos % dim_size[++dim] != 0)
      ;

    if (dim == dim_list.size())
      throw exception("misaligned InitValList at pos " + std::to_string(pos));

    const auto list_node = dynamic_cast<const InitValListASTNode *>(ast);

    cur.pos++;
    st.push({list_node->get_init_val_list(), dim, 0, pos});
    return next();
  }
  else if (ast->node_type() == ASTNodeType::Number)
  {
    assert(ast->generate_on(*generator));

    pos++;
    cur.pos++;
  }
  else
  {
    if (const_)
    {
      // evaluator need to be run first
      throw exception("ConstInitVal at pos " + std::to_string(pos) +
                      " can't be determined at compile time");
    }

    if (!ast->generate_on(*generator))
    {
      throw exception("failed to generate InitVal at pos " + std::to_string(pos));
    }

    pos++;
    cur.pos++;
  }

  return true;
}

} // namespace __impl

} // namespace quad

} // namespace backend

} // namespace sysyc
