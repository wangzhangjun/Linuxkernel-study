CLANG = clang
EXECABLE = bpfload

BPFLOADER_C = /usr/src/linux-source-4.15.0/linux-source-4.15.0/samples/bpf/bpf_load.c  #link load_bpf_file symbol
CCINCLUDE += -I/usr/src/linux-source-4.15.0/linux-source-4.15.0/samples/bpf # bpf_load.h
CCINCLUDE += -I/usr/src/linux-source-4.15.0/linux-source-4.15.0/tools/lib # bpf/bpf.h
CCINCLUDE += -I/usr/src/linux-source-4.15.0/linux-source-4.15.0/tools/perf # perf-sys.h
CCINCLUDE += -I/usr/src/linux-source-4.15.0/linux-source-4.15.0/tools/include # <linux/compiler.h>

# -lelf 会默认去找 ./usr/lib/x86_64-linux-gnu/libelf.so 
# -lbpf 会默认去找 ./lib/x86_64-linux-gnu/libbpf.so

all:
	# -I 指定的路径是因为bpf程序需要asm/types.h
	$(CLANG) -O2 -target bpf -c bpf_program.c  -I /usr/include/x86_64-linux-gnu/  -o bpf_program.o 
	$(CLANG) -o $(EXECABLE)  \
		$(CCINCLUDE)\
		-lelf \
		-lbpf \
		loader.c \
		$(BPFLOADER_C)

clean:
	rm -f *.o *.so $(EXECABLE)
