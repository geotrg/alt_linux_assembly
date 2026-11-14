#!/bin/bash
# build.sh — builds all artifacts for the loader research exercise
set -euo pipefail

echo "[1/7] Assembling payload.asm ..."
nasm -f elf64 payload.asm -o payload.o
ld -nostdlib -static -e _start -o payload.elf payload.o
objcopy -O binary --only-section=.text payload.elf payload.bin

SIZE=$(stat -c%s payload.bin)
echo "      payload.bin = ${SIZE} bytes"
if [ "$SIZE" -ne 55 ]; then
    echo "ERROR: expected 52 bytes, got ${SIZE}" >&2
    exit 1
fi

echo "[2/7] Disassembling for sanity check ..."
objdump -D -b binary -m i386:x86-64 -M intel payload.bin

echo "[3/7] Building encrypt tool ..."
gcc -O2 encrypt.c -o encrypt
echo "      Running encrypt; copy its output into loader_mmap.c"
./encrypt > encrypted_block.txt
echo "      saved to encrypted_block.txt"

echo "[4/7] Building benign ELF payload for memfd loader ..."
if gcc -O2 -static -o payload_memfd.elf payload.c 2>/dev/null; then
    echo "      static build OK ($(stat -c%s payload_memfd.elf) bytes)"
else
    echo "      static libc not available — falling back to dynamic linking"
    gcc -O2 -o payload_memfd.elf payload.c
    echo "      dynamic build OK ($(stat -c%s payload_memfd.elf) bytes)"
fi
xxd -i -n payload_elf payload_memfd.elf > payload_elf.h
head -1 payload_elf.h
tail -1 payload_elf.h

echo "      payload_elf.h generated ($(stat -c%s payload_memfd.elf) bytes)"

echo "[5/7] Building memfd loader (primary) ..."
gcc -O2 -o loader_memfd loader_memfd.c

echo "[6/7] Building mmap loader (educational) ..."
# NOTE: paste encrypted_block.txt into loader_mmap.c before this step succeeds.
gcc -O2 -no-pie -o loader_mmap loader_mmap.c || \
    echo "      (loader_mmap.c not yet filled in — skipping)"

echo "[7/7] Done."
echo
echo "Primary:      ./loader_memfd    (works under execmem restrictions)"
echo "Educational:  ./loader_mmap     (needs ciphertext pasted in)"
echo
echo "Verify decryption:"
echo "  objdump -D -b binary -m i386:x86-64 -M intel /tmp/decrypted.bin"
echo "  cmp /tmp/decrypted.bin payload.bin && echo MATCH"
