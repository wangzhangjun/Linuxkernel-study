#include <linux/bpf.h>
#include "bpf_helpers.h"
#include <string.h>
#include <errno.h>
#define SEC(NAME) __attribute__((section(NAME), used))

struct bpf_map_def SEC("maps") my_map = {
    .type = BPF_MAP_TYPE_HASH,
    .key_size = sizeof(int),
    .value_size = sizeof(int),
    .max_entries = 100};

SEC("tracepoint/syscalls/sys_enter_execve")
int bpf_prog(void *ctx) {
  char msg[] = "Hello, BPF World!";
  bpf_trace_printk(msg, sizeof(msg));
  int key, value, result;
  key = 1, value = 1234;
  result = bpf_map_update_elem(&my_map, &key, &value, BPF_ANY); //BPF_ANY元素存在，内核更新元素，不存在，则在映射中创建该元素，同理可以尝试BPF_NOEXIST和BPF_EXIST
  if(result == 0) {
    printt("update element success result:%d\n", result);
  }else {
    printt("failed to update element:%d\n", result);
  }
  return 0;
}

SEC("socket")
int socket_prog(struct __sk_buff *skb) {
  return 0;
}

char _license[] SEC("license") = "GPL";
