# kenel中进程绝对路径的获取

```
char* get_process_absolute_path(struct task_struct * task)
{
    char * ret_ptr = NULL;
    char * tpath   = NULL ;
    struct vm_area_struct * vma = NULL;
    struct path base_path;

    tpath = (char*)kmalloc(512, 0);
    if(NULL == tpath || NULL == task)
    {
        return NULL;
    }
    memset(tpath,'\0',512);

    task_lock(task);

    if(task->mm && task->mm->mmap)
    {
        vma = task->mm->mmap;
    }
    else
    {
        task_unlock(task);
        kfree(tpath);
        return NULL;
    }

    while(vma)
    {
        if ((vma->vm_flags) && vma->vm_file)
        {    
            base_path = vma->vm_file->f_path;
            break;
        }
        vma = vma->vm_next;
    }
    task_unlock(task);

    ret_ptr = d_path(&base_path, tpath, 512);

    return ret_ptr;
}
```
