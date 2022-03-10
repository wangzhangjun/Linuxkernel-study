#include <uapi/linux/openat2.h>
#include <linux/sched.h>

struct data_t { 
    u32 pid; 
    u64 ts; 
    char comm[TASK_COMM_LEN]; 
    char fname[NAME_MAX];
};

// 定义perf性能事件映射
// 用户态的Py就可以不断的从这个映射中读取收集到的数据
BPF_PERF_OUTPUT(eventstest);

// 定义kprobe处理函数
int hello_world(struct pt_regs *ctx, int dfd, const char __user * filename, struct open_how *how)
{
  struct data_t data = { };

  // 获取PID和时间
  data.pid = bpf_get_current_pid_tgid();
  data.ts = bpf_ktime_get_ns();

  // 获取进程名
  if (bpf_get_current_comm(&data.comm, sizeof(data.comm)) == 0)
  {
    bpf_probe_read(&data.fname, sizeof(data.fname), (void *)filename); // 读取filename的值到data.fname中
  }

  // 提交性能事件
  eventstest.perf_submit(ctx, &data, sizeof(data));
  return 0;
}