//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: analyzer.cc
//  Created Date: 2021-05-04
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#include "middle/ast/node.h"
#include "middle/ast/analyzer.h"

namespace sysyc
{
namespace middle
{
namespace ast
{

bool ProgramASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool VarDeclASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool VarDefASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool InitValListASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool FuncDefASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool FuncFParamListASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool FuncFParamASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool BlockASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool IfElseStmtASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool WhileStmtASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool ControlStmtASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool BinaryExpASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool UnaryExpASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool FuncCallASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool FuncRParamListASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool BareStmtASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool LValASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool NumberASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

bool PrimTypeASTNode::analyze_on(IASTAnalyzer &analyzer)
{
  return analyzer.analyze(this);
}

} // namespace ast

} // namespace middle

} // namespace sysyc
