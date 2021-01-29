#include "bpf_load.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>

int main(int argc, char **argv)
{
  if (load_bpf_file("bpf_program.o") != 0)
  {
    printf("The kernel didn't load the BPF program\n");
    return -1;
  }

  int key,value,result;
  key = 1, value = 1234;
  result = bpf_map_update_elem(map_data[0].fd, &key, &value, BPF_ANY);//用户空间始终使用文件描述符来访问Map,放在全局的map_data数组中
  if (result == 0){
    printf("userspace update element success result:%d\n", result);
  }
  else{
    printf("userspace failed to update element:%d (%s)\n", result, strerror(errno));
  }

  read_trace_pipe();

  return 0;
}
