	.file	"worm.s"
	.ignore	ld_st_style
	.ignore	strict_delay
	
	.text
	.global	make_executable_and_run
	.type	make_executable_and_run, #function
	.align	8
make_executable_and_run:
	{
	  setwd	wsz = 0x8, nfx = 0x1, dbl = 0x0
	  setbn	rsz = 0x3, rbs = 0x4, rcur = 0x0
	  getsp,0	_f16s,_lts1hi 0xfff0, %r2
	}
	{
	  addd,0,sm	0x0, %r0, %b[0]
	  addd,1,sm	0x0, [ _f64,_lts0 .LC.1 ], %b[1]
	}
.LCS.1:
	{
	  disp	%ctpr1, fopen
	}
	{
	  nop 4
	}
	{
	  call	%ctpr1, wbs = 0x4
	}
.LCS.2:
	{
	  addd,0,sm	0x0, %b[0], %r5
	  disp	%ctpr1, .L15; ipd 2
	}
	{
	  addd,0,sm	0x0, %r5, %r3
	}
	{
	  nop 2
	  cmpedb,0	%r3, 0x0, %pred0
	}
	{
	  ct	%ctpr1 ? ~%pred0
	}
.L18:
	{
	  addd,0,sm	0x0, [ _f64,_lts0 .LC.2 ], %b[0]
	}
.LCS.3:
	{
	  disp	%ctpr1, perror
	}
	{
	  nop 4
	}
	{
	  call	%ctpr1, wbs = 0x4
	}
.LCS.4:
	{
	  return	%ctpr3; ipd 2
	}
	{
	  nop 5
	}
	{
	  ct	%ctpr3
	}
.L15:
	{
	  addd,0,sm	0x0, %r3, %b[0]
	  addd,1,sm	0x0, [ _f64,_lts0 .LC.3 ], %b[1]
	  addd,2,sm	0x0, [ _f64,_lts2 .LC.3 ], %r5
	}
	{
	  std,2,sm	%r2, 0x8, %r5
	}
.LCS.5:
	{
	  disp	%ctpr1, fprintf
	}
	{
	  nop 4
	}
	{
	  call	%ctpr1, wbs = 0x4
	}
.LCS.6:
	{
	  addd,0,sm	0x0, %r3, %b[0]
	}
.LCS.7:
	{
	  disp	%ctpr1, fclose
	}
	{
	  nop 4
	}
	{
	  call	%ctpr1, wbs = 0x4
	}
.LCS.8:
	{
	  addd,0,sm	0x0, [ _f64,_lts0 .LC.4 ], %b[0]
	  addd,1,sm	0x0, [ _f64,_lts2 .LC.4 ], %r5
	  addd,2,sm	0x0, %r0, %b[1]
	}
	{
	  std,2,sm	%r2, 0x0, %r5
	  std,5,sm	%r2, 0x8, %r0
	}
.LCS.9:
	{
	  disp	%ctpr1, printf
	}
	{
	  nop 4
	}
	{
	  call	%ctpr1, wbs = 0x4
	}
.LCS.10:
	{
	  addd,0,sm	0x0, %r0, %b[0]
	  addd,1,sm	0x0, _f16s,_lts0hi 0x1ed, %b[1]
	  disp	%ctpr1, chmod
	}
	{
	  nop 4
	}
	{
	  call	%ctpr1, wbs = 0x4
	}
	{
	  adds,0,sm	0x0, %b[0], %r5
	  disp	%ctpr1, .L64; ipd 2
	}
	{
	  nop 3
	  cmpesb,0	%r5, 0x0, %pred1
	}
	{
	  ct	%ctpr1 ? %pred1
	}
.L67:
	{
	  addd,0,sm	0x0, [ _f64,_lts0 .LC.5 ], %b[0]
	}
.LCS.11:
	{
	  disp	%ctpr1, perror
	}
	{
	  nop 4
	}
	{
	  call	%ctpr1, wbs = 0x4
	}
.LCS.12:
	{
	  return	%ctpr3; ipd 2
	}
	{
	  nop 5
	}
	{
	  ct	%ctpr3
	}
.L64:
	{
	  addd,0,sm	0x0, [ _f64,_lts0 .LC.6 ], %b[0]
	  addd,1,sm	0x0, [ _f64,_lts2 .LC.6 ], %r5
	  addd,2,sm	0x0, %r0, %b[1]
	}
	{
	  std,2,sm	%r2, 0x0, %r5
	  std,5,sm	%r2, 0x8, %r0
	}
.LCS.13:
	{
	  disp	%ctpr1, printf
	}
	{
	  nop 4
	}
	{
	  call	%ctpr1, wbs = 0x4
	}
.LCS.14:
	{
	  addd,0,sm	0x0, [ _f64,_lts0 .LC.7 ], %b[0]
	  addd,1,sm	0x0, [ _f64,_lts2 .LC.7 ], %r5
	  addd,2,sm	0x0, %r0, %b[1]
	}
	{
	  std,2,sm	%r2, 0x0, %r5
	  std,5,sm	%r2, 0x8, %r0
	}
.LCS.15:
	{
	  disp	%ctpr1, printf
	}
	{
	  nop 4
	}
	{
	  call	%ctpr1, wbs = 0x4
	}
.LCS.16:
	{
	  addd,0,sm	0x0, %r0, %b[0]
	}
.LCS.17:
	{
	  disp	%ctpr1, system
	}
	{
	  nop 4
	}
	{
	  call	%ctpr1, wbs = 0x4
	}
.LCS.18:
	{
	  adds,0,sm	0x0, %b[0], %r5
	  disp	%ctpr1, .L109; ipd 2
	}
	{
	  adds,0,sm	0x0, %r5, %r4
	}
	{
	  nop 2
	  cmpesb,0	%r4, _f16s,_lts0hi 0xffff, %pred2
	}
	{
	  ct	%ctpr1 ? ~%pred2
	}
.L112:
	{
	  addd,0,sm	0x0, [ _f64,_lts0 .LC.8 ], %b[0]
	}
.LCS.19:
	{
	  disp	%ctpr1, perror
	}
	{
	  nop 4
	}
	{
	  call	%ctpr1, wbs = 0x4
	}
.LCS.20:
.L109:
	{
	  addd,0,sm	0x0, [ _f64,_lts0 .LC.9 ], %b[0]
	  addd,1,sm	0x0, [ _f64,_lts2 .LC.9 ], %r5
	}
	{
	  std,2,sm	%r2, 0x0, %r5
	}
.LCS.21:
	{
	  disp	%ctpr1, printf
	}
	{
	  nop 4
	}
	{
	  call	%ctpr1, wbs = 0x4
	}
.LCS.22:
	{
	  return	%ctpr3; ipd 2
	}
	{
	  nop 5
	}
	{
	  ct	%ctpr3
	}
	.size	make_executable_and_run, .- make_executable_and_run

	.section .rodata
	.align	2
.LC.1:
	.ascii	"w\000"
	.align	16
.LC.2:
	.ascii	"Error creating file\000"
	.align	16
.LC.3:
	.ascii	"#!/bin/bash\nwhoami\necho 'Hello from the"
	.ascii	" executable script!'\npwd\000"
	.align	16
.LC.4:
	.ascii	"Created script: %s\n\000"
	.align	16
.LC.5:
	.ascii	"Error changing permissions\000"
	.align	16
.LC.6:
	.ascii	"Successfully made %s executable."
	.ascii	"\n\000"
	.align	16
.LC.7:
	.ascii	"Executing %s...\n---\n\000"
	.align	16
.LC.8:
	.ascii	"Error executing file\000"
	.align	16
.LC.9:
	.ascii	"---\nExecution finished.\n\000"
