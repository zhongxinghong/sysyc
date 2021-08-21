//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: generator.cc
//  Created Date: 2021-05-02
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include "middle/ast/node.h"
#include "middle/ast/generator.h"

namespace sysyc
{
namespace middle
{
namespace ast
{

bool ProgramASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool VarDeclASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool VarDefASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool InitValListASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool FuncDefASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool FuncFParamListASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool FuncFParamASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool BlockASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool IfElseStmtASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool WhileStmtASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool ControlStmtASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool BinaryExpASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool UnaryExpASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool FuncCallASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool FuncRParamListASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool BareStmtASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool LValASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool NumberASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

bool PrimTypeASTNode::generate_on(IASTGenerator &generator) const
{
  return generator.generate(this);
}

} // namespace ast

} // namespace middle

} // namespace sysyc
