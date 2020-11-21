#include <linux/kobject.h>
#include <linux/module.h>
#include <linux/kmod.h>
#include <linux/init.h>

struct kset kset_p;
struct kset kset_c;

int kset_filter(struct kset *kset,struct kobject *kobj)
{
	printk("Filter: kobj is %s.\n",kobj->name);
	return 1;
}


int kset_name(struct kset *kset,struct kobject *kobj)
{
	static char buf[20];
	printk("Name: kobj is %s \n",kobj->name);
	sprintf(buf,"%s","kset_name");
	return buf;
}

int kset_uevent(struct kset *kset,struct kobject *kobj,struct kobj_uevent_env *env)
{
	int i = 0;
	printk("Uevent: kobj is %s \n",kobj->name);
	while(i < env->envp_idx)
	{
		printk("%s\n",env->envp[i]);
		i++;
	}
	return 0;
	

}

struct kset_uevent_ops uevent_ops =
{
	.filter = kset_filter,
	.name = kset_name,
	.uevent = kset_uevent,
};


static int __init  kset_test_init(void)
{
	int ret =0;	
	printk("kset_test_init \n");
	kobject_set_name(&kset_p.kobj,"kset_p");
	kset_p.uevent_ops = &uevent_ops;
	ret = kset_register(&kset_p);

	printk("kset_p ret = %d\n ",ret);
	kobject_set_name(&kset_c.kobj,"kset_c");
	//create kobject to call uevent
	kset_c.kobj.kset = &kset_p;
	//kset_c.uevent_ops = &uevent_ops;
	ret = kset_register(&kset_c);
	//printk("kset_c ret = %d\n ",ret);
	
	return 0;
}

static int __exit kset_test_exit(void)
{
	printk("kset_test_exit \n");
	kset_unregister(&kset_p);
	kset_unregister(&kset_c);
	return 0;
}

module_init(kset_test_init);
module_exit(kset_test_exit);
MODULE_AUTHOR ("support@ingben.com");
MODULE_DESCRIPTION ("uevent_ops test");
MODULE_LICENSE ("GPL v2");
