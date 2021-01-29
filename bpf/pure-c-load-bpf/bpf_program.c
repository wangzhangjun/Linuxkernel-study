#include <linux/bpf.h>
#define SEC(NAME) __attribute__((section(NAME), used))

static int (*bpf_trace_printk)(const char *fmt, int fmt_size,
                               ...) = (void *)BPF_FUNC_trace_printk;

#define printt(fmt, ...)                                       \
  ({                                                           \
    char ____fmt[] = fmt;                                      \
    bpf_trace_printk(____fmt, sizeof(____fmt), ##__VA_ARGS__); \
  })

struct bpf_map_def SEC("maps") my_map = {
  .type = BPF_MAP_TYPE_HASH,
  .key_size = siezof(int),
  .value_size = sizeof(int),
  .max_entries = 100
};

SEC("tracepoint/syscalls/sys_enter_execve")
int bpf_prog(void *ctx) {
  char msg[] = "Hello, BPF World!";
  bpf_trace_printk(msg, sizeof(msg));
  int key, value, result;
  key = 1, value = 1234;
  result = bpf_map_update_elem(&my_map, &key, &value, BPF_any);
  if(result == 0) {
    printt("update element success result:%d\n", result);
  }else{
    printt("failed to update element:%d (%s)\n", result, strerror(errno));
  }
  return 0;
}

SEC("socket")
int socket_prog(struct __sk_buff *skb) {
  return 0;
}

char _license[] SEC("license") = "GPL";
