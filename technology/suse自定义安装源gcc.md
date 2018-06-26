# suse自定义安装源gcc


http://blog.csdn.net/u012359618/article/details/53318613

列出所有的源：
```
zypper lr
```
禁用掉不用的源(禁用掉 1 2 3 4 5的源)
```
zypper mr -d 1 2 3 4 5
```
卸载
```
zypper rm gcc
```

当出现No repositories defined. Use the 'zypper addrepo' command to add one or more repositories.
```
 zypper addrepo /mnt/suse suse
```
