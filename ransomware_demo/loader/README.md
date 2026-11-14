# Loader Research Sample (educational)

Benign loader demonstrating encrypted payload storage and runtime
reconstruction. Payload prints "PAYLOAD EXECUTED" and exits.

## Layout

| File             | Purpose                                            |
|------------------|----------------------------------------------------|
| payload.asm      | 52-byte shellcode (write + exit)                   |
| payload.c        | Benign ELF used by memfd loader                    |
| encrypt.c        | XOR packer (key 0x5A)                              |
| loader_memfd.c   | ⭐ primary loader — memfd_create + fexecve         |
| loader_mmap.c    | educational mmap RW→RX loader with maps diagnostic |
| build.sh         | one-shot build script                              |

## Build

    chmod +x build.sh
    ./build.sh

Paste the output of `./encrypt` into `loader_mmap.c` between the
`/* ==== ... ==== */` markers, then rebuild:

    gcc -O2 -no-pie -o loader_mmap loader_mmap.c

## Run

    ./loader_memfd        # recommended; survives execmem hardening
    ./loader_mmap         # educational; may SIGSEGV on hardened kernels

## Analysis exercises

1. `strace -f -e trace=memfd_create,write,fexecve,execve ./loader_memfd`
   Observe the fileless execution chain.

2. `strace -e trace=mmap,mprotect ./loader_mmap`
   Observe RW→RX transition; check /proc/self/maps output for the page.

3. `strings loader_memfd | grep -i payload`
   Confirm "PAYLOAD EXECUTED" does NOT appear (only in memfd ELF bytes,
   but not as a simple string in the loader binary after embedding — verify).

4. `objdump -D -b binary -m i386:x86-64 -M intel /tmp/decrypted.bin`
   Compare against payload.asm to confirm correct decryption.

5. In gdb on loader_mmap: break on mprotect, dump memory before/after,
   then examine the fault with `x/10i $rip` at the crash address.
