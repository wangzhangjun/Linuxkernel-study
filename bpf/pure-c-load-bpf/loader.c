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

  //插入元素--用户态
  int key,value,result;
  key = 1, value = 1234;
  result = bpf_map_update_elem(map_data[0].fd, &key, &value, BPF_ANY);//用户空间始终使用文件描述符来访问Map,放在全局的map_data数组中
  if (result == 0){
    printf("userspace update element success result:%d\n", result);
  }
  else{
    printf("userspace failed to update element:%d (%s)\n", result, strerror(errno));
  }

  //查找元素--用户态
  int key1 = 1;
  int value1;
  result = bpf_map_lookup_elem(map_data[0].fd, &key1,&value1); //查找元素
  if (result == 0){
    printf("userspace lookup element success result:%d\n", value1);
  }
  else{
    printf("userspace lookup element failed (%s)\n",strerror(errno));
  }

  read_trace_pipe();

  return 0;
}
