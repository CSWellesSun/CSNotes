# 记录

首先需要安装依赖：`sudo apt install tcl tcl-dev tk tk-dev; sudo apt install flex; sudo apt install bison`。 

## PartA

解题答案放在：`sim/misc/sum.ys`, `sim/misc/rsum/ys`, `sim/misc/copy.ys`

ISA参考：

- https://csit.kutztown.edu/~schwesin/fall21/csc235/lectures/Instruction_Set_Architecture.html（主要）

- https://www.cs.cmu.edu/~fp/courses/15213-s07/misc/asm64-handout.pdf

主要是要了解y86-64的ISA，将`examples.c`翻译成汇编，注意在该指令集下没有`testq/jz`。

## PartB

关于HCL(Hardware Control Language)需要参考CSAPP原书。

解题答案放在`sim/seq/seq-full.hcl`。

`IADDQ V, rB`和`IRMOVQ`以及`OPQ`类似，不要漏掉`set_cc`。

## PartC

解题答案放在`sim/pipe/pipe-full.hcl`。

暂时放置，因为计组已经实现过流水线。

# 原版README

#####################################################################
# CS:APP Architecture Lab
# Handout files for students
#
# Copyright (c) 2002, 2010, 2015, R. Bryant and D. O'Hallaron,
# All rights reserved. May not be used, modified, or copied
# without permission.
#
######################################################################

This directory contains the files that you need for the CS:APP
architecture lab.

******
Files:
******

Makefile		Use this to handin your solutions
README			This file
archlab.{ps,pdf}	Lab writeup
sim.tar			Archive of the Y86-64 tools in tar format
simguide.{ps,pdf}	CS:APP Guide to Simulators document	
