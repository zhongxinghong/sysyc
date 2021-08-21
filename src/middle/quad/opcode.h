//------------------------------------------------------------------------------
//  A SysY Compiler for PKU Compiling Principles Practice Course (2021 Spring)
//------------------------------------------------------------------------------
//  Project: sysyc
//  File: opcode.h
//  Created Date: 2021-05-17
//  Author: Zhong Xinghong (1700012608)
//  Email: zxh2017@pku.edu.cn
//------------------------------------------------------------------------------
//  Copyright (c) 2021 Zhong Xinghong
//------------------------------------------------------------------------------

#ifndef __SYSYC_MIDDLE_QUAD_OPCODE_H__
#define __SYSYC_MIDDLE_QUAD_OPCODE_H__

namespace sysyc
{
namespace middle
{
namespace quad
{
enum class Opcode
{
  /**
   * Expression      ::= SYMBOL "=" RightValue BinOp RightValue
   *                   | SYMBOL "=" OP RightValue
   */
  LNOT, NEG,
  NE, EQ, GT, LT, GE, LE,
  LAND, LOR,
  ADD, SUB, MUL, DIV, MOD,

  /**
   * Expression      ::= "if" RightValue LOGICOP RightValue "goto" LABEL
   *                   | "goto" LABEL | ......
   */
  JNE, JEQ, JGT, JLT, JGE, JLE,
  JEQZ, JNEZ,
  JMP,

  /**
   * Initialization  ::= SYMBOL "=" NUM
   *                   | SYMBOL "[" NUM "]" "=" NUM;
   * Expression      ::= SYMBOL "=" RightValue
   *                   | SYMBOL "[" RightValue "]" "=" RightValue | ......
   *
   * Expression      ::= Reg "=" Reg
   *                   | Reg "=" NUM
   *                   | Reg "[" NUM "]" "=" Reg
   */
  SWORD,    // dest = SYMBOL, arg1 = RVAL / SYMBOL / REG, [arg2 = RVAL / NUM]

  /**
   * Expression      ::= SYMBOL "=" SYMBOL "[" RightValue "]" | ......
   * Expression      ::= Reg "=" Reg "[" NUM "]" | ......
   */
  LWORD,    // dest = SYMBOL / REG, arg1 = SYMBOL / REG, arg2 = RVAL / NUM

  /**
   * Declaration     ::= "var" [NUM] SYMBOL;
   */
  VDEF,     // dest = SYMBOL / VARIABLE, [arg1 = NUM_SIZE]

  /**
   * FunctionHeader  ::= FUNCTION "[" NUM "]";              // Eeyore
   * FunctionHeader  ::= FUNCTION "[" NUM "]" "[" NUM "]";  // Tigger
   *
   * FunctionEnd     ::= "end" FUNCTION;
   */
  FDEF,     // dest = FUNCTION, arg1 = NUM_ARGS, [arg2 = NUM_STACK]
  FEND,     // dest = FUNCTION

  /* Expression      ::= LABEL ":" | ...... */
  LABEL,    // arg1 = LABEL

  /* Expression      ::= "param" RightValue */
  PARAM,    // arg1 = RVAL

  /**
   * Expression      ::= "call" FUNCTION
   *                   | SYMBOL "=" "call" FUNCTION | ......
   */
  CALL,     // arg1 = FUNCTION, [dest = SYMBOL]

  /**
   * Expression      ::= "return" RightValue
   *                   | "return" | ......
   */
  RET,      // [arg1 = RVAL]

  /**
   * Expression      ::= "store" Reg NUM
   *                   | "load" NUM Reg
   *                   | "load" VARIABLE Reg
   *                   | "loadaddr" NUM Reg
   *                   | "loadaddr" VARIABLE Reg | ......
   */
  STORE,    // dest = NUM, arg1 = REG
  LOAD,     // dest = REG, arg1 = NUM / VARIABLE
  LOADADDR, // dest = REG, arg1 = NUM / VARIABLE

  /* Additional Opcode */
  SLL,
  SRL,
  SRA,
  AND,
};

} // namespace quad

} // namespace middle

} // namespace sysyc

#endif // __SYSYC_MIDDLE_QUAD_OPCODE_H__
