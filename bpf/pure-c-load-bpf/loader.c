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

  //删除元素--用户态
  int key_del = 1;
  result = bpf_map_delete_elem(map_data[0].fd, &key_del);
  if (result == 0){
    printf("userspace delete element success\n");
  }
  else{
    printf("userspace delete element failed (%s)\n", strerror(errno));
  }

  //遍历元素
  int new_key,new_value, it;
  for(it=1; it < 6; it++) {
    new_key = it;
    new_value = 1234 + it;
    bpf_map_update_elem(map_data[0].fd, &new_key, &new_value, BPF_ANY);//先添加几个
  }
  int next_key, lookup_key;
  lookup_key = -1;
  //每循环一个，会把下一个的key设置到next_key中。这里从-1开始就可以遍历所有的元素，如果是从中间的某一个，就是从中间的那一个到最后。
  while (bpf_map_get_next_key(map_data[0].fd, &lookup_key, &next_key) == 0) {
    printf("The next key in the map is: %d\n", next_key);
    //如果正在遍历的时候，内核突然删除了一个，是什么效果,注意bpf_map_get_next_key将会重新开始查找
    /*
      效果如下：
The next key in the map is: 1
The next key in the map is: 4
The next key in the map is: 5
The next key in the map is: 2
delteing 2
The next key in the map is: 1
The next key in the map is: 4
The next key in the map is: 5
The next key in the map is: 3
    */
    if(next_key == 2) {
      printf("delteing 2\n");
      bpf_map_delete_elem(map_data[0].fd, &next_key);
    }
    lookup_key = next_key;
  }
  read_trace_pipe();

  return 0;
}
