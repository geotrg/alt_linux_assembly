.file "write_to_file_asm.c"
.ignore ld_st_style
.ignore strict_delay

# ---------------------------------------------------------------
# write_to_file_asm.s — E2K (Elbrus) assembly for writing a string
# to a file. Instruction forms (setwd/setbn/getsp sizes, %b[] call
# args, "sm" modifiers) are copied from real lcc -S output for the
# equivalent C function:
#
#   void open_w(const char *path, const char *data) {
#       FILE *f = fopen(path, "w");
#       if (f) { fputs(data, f); fclose(f); }
#   }
#
# Function signature: void write_to_file_asm(const char *filename,
#                                             const char *data)
# Incoming: %r0 = filename, %r1 = data
# ---------------------------------------------------------------

.text
.global write_to_file_asm
.type write_to_file_asm, #function
.align 8

write_to_file_asm:
{
    setwd  wsz = 0x8, nfx = 0x1, dbl = 0x0
    setbn  rsz = 0x3, rbs = 0x4, rcur = 0x0
    getsp,0 _f16s,_lts1hi 0xfff0, %r3
}

# FILE *f = fopen(filename, "w");
{
    addd,0,sm 0x0, %r0, %b[0]
    addd,1,sm 0x0, [ _f64,_lts0 STR_MODE_W ], %b[1]
}
{
    disp %ctpr1, fopen
}
{
    nop 4
}
{
    call %ctpr1, wbs = 0x4
}

# save result; r4 = f
{
    addd,0,sm 0x0, %b[0], %r5
}
{
    addd,0,sm 0x0, %r5, %r4
}

# if (f == NULL) return;
{
    nop 2
    cmpedb,0 %r4, 0x0, %pred0
}
{
    disp %ctpr1, WTF_DONE; ipd 2
}
{
    ct %ctpr1 ? %pred0
}

# fputs(data, f);
{
    addd,0,sm 0x0, %r1, %b[0]
    addd,1,sm 0x0, %r4, %b[1]
}
{
    disp %ctpr1, fputs
}
{
    nop 4
}
{
    call %ctpr1, wbs = 0x4
}

# fclose(f);
{
    addd,0,sm 0x0, %r4, %b[0]
}
{
    disp %ctpr1, fclose
}
{
    nop 4
}
{
    call %ctpr1, wbs = 0x4
}

WTF_DONE:
{
    return %ctpr3; ipd 2
}
{
    nop 5
}
{
    ct %ctpr3
}
.size write_to_file_asm, .-write_to_file_asm

.section .rodata
.align 2
STR_MODE_W:
    .ascii "w\000"
