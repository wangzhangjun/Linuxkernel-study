# 根据不同的内核版本进行条件编译
在内核开发中，内核之间的版本不同会使用到不同的版本，所以需要使用宏编译来在不同的版本上使用不同
的函数。
```
#include <linux/version.h>

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 0)
#define LINUX_VERSION_26
#endif
```
上面的代码表示，当内核版本为2.6.0时，可以使用LINUX_VERSION_26表示。定义好了上面的就可以像
下面这样去使用
```
#ifdef LINUX_VERSION_26(或者使用#ifndef：表示不是这个版本的时候)
    2.6上的函数
#else
    非2.6上的函数
#endif // LINUX_VERSION_26
```
