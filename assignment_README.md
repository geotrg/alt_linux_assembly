# Assignment deliverables

## 1️⃣ Python package for ALT Linux — twitchio

Files (in `alt-package/`):
- `python3-module-twitchio.spec` — the spec file
- `python3-module-twitchio-3.3.2-alt1.noarch.rpm` — the built binary RPM
- `python3-module-twitchio-3.3.2-alt1.src.rpm` — the built source RPM
- `twitchio-3.3.2.tar.gz` — the original upstream source tarball (as published on PyPI), which is what `Source:` in the spec refers to

**What the package does:** twitchio 3.3.2 is a pure-Python library (no compiled
extensions), so the package is `BuildArch: noarch`. It follows ALT's Python
packaging convention: binary package named `python3-module-twitchio`, built
with the `rpm-build-python3` macros (`%py3_build`, `%py3_install`,
`%python3_sitelibdir`), and declares `Requires: python3-module-aiohttp` since
that's twitchio's one runtime dependency (`aiohttp>=3.9.1,<4`, straight from
upstream's `requirements.txt`).

**How I actually verified it, not just wrote it:**
- `rpmbuild -bb` / `-bs` were run for real and produced the `.rpm` / `.src.rpm`
  in this folder — not hand-assembled.
- I extracted the built RPM's payload and ran `import twitchio` /
  `from twitchio.ext import commands` against it with `aiohttp` installed
  alongside — both imports succeeded and `twitchio.__version__` reported
  `3.3.2`, confirming the packaged files are a genuinely working module, not
  just correctly-named empty files.

**Important caveat:** this was built and tested on Ubuntu 24.04's generic
`rpm`/`rpmbuild`, *not* on a real ALT Linux system — ALT's actual
`rpm-build-python3` package (which provides the real `%py3_build` /
`%py3_install` / `%python3_sitelibdir` macro definitions) isn't installable
in this environment. I supplied local approximations of those macros,
matching the standard, widely-documented behavior those macros have across
RPM-based distros, to get a real, working build as a correctness check. The
`.spec` file itself is written the way it should look on actual ALT Linux
(package naming, `BuildRequires: rpm-build-python3`, etc.) — but before
treating the `.rpm` in this folder as final, rebuild it in a real ALT
environment (e.g. `hasher` or a Sisyphus chroot) to pick up ALT's actual
macro definitions and dependency resolution against the real ALT repository.
I also couldn't verify the `BuildRequires`/`Requires` package names
(`python3-module-setuptools`, `python3-module-aiohttp`, etc.) resolve to real
packages in ALT's repo, since I have no network access to it from here — I'm
fairly confident in these names from ALT's documented conventions, but
they're worth a quick double-check against the actual Sisyphus package list.

## 2️⃣ RSA in C — dumb vs. optimized

Files (in `rsa_c/`):
- `rsa_dumb.c` — naive version
- `rsa_optimized.c` — optimized version
- `rsa_compare.c` — runs both on the same input back-to-back and prints a
  side-by-side timing table (this is what I'd actually demo with)
- `Makefile` — `make` builds all three

**The one thing that differs between dumb and optimized** is how
`base^exp mod n` (modular exponentiation — the core operation of both RSA
encryption and decryption) gets computed:

- **Dumb** (`modexp_naive`): a straight loop, multiplying by `base` and
  reducing mod `n`, `exp` times. O(exp) multiplications.
- **Optimized** (`modexp_fast`): square-and-multiply / binary
  exponentiation — square `base` and conditionally fold it into the result
  while walking the bits of `exp`. O(log2 exp) multiplications.

Both files expose the required `encrypt_*` / `decrypt_*` function pairs built
on top of their respective `modexp_*`. Each program reads a line of text from
the console, encrypts it character-by-character with the public key,
prints the resulting block values, decrypts them back with the private key,
prints the recovered text, and reports timing via `clock()`.

**Demo key** (fixed, defined in each file): `p=809, q=2411`, giving
`n=1950499`, `e=65537`, `d=235473`. `d` was deliberately chosen in the
hundreds-of-thousands range — large enough that the dumb version's O(d) loop
is clearly, measurably slow, while still finishing a demo in well under a
second, rather than either being too fast to show a difference or taking
minutes to run.

**Actual measured results** (not estimated — `./rsa_compare` really run
against a 41-character sentence):

| Version   | Encrypt (s) | Decrypt (s) | Total (s) |
|-----------|-------------|-------------|-----------|
| Dumb      | 0.019119    | 0.065108    | 0.084227  |
| Optimized | 0.000003    | 0.000004    | 0.000007  |

Optimized came out about **12,000× faster** overall in that run (decrypt
dominates both totals since `d` is much larger than `e`). I also re-ran it
through the `Makefile`-built binaries and with a 200-character input to
confirm the dumb version's time scales linearly with message length, as
expected for an O(d) loop run once per character — both checks were
consistent with the table above.

Build and run:
```sh
cd rsa_c
make
echo "Hello, RSA!" | ./rsa_compare
# or run them separately:
echo "Hello, RSA!" | ./rsa_dumb
echo "Hello, RSA!" | ./rsa_optimized
```
