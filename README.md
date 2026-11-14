# alt_linux_assembly
# how to run lcc on docker
docker run -it --rm -v /path/to/your/project:/work -v /opt/mcst:/opt/mcst registry.altlinux.org/p11/devel:latest /bin/bash
lcc -static rsa_compare_test.c -o rsa_compare_test
