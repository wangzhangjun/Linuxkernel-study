#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x41086e, "module_layout" },
	{ 0xe5942277, "kmem_cache_free" },
	{ 0xd8b08694, "kmem_cache_alloc" },
	{ 0x175ffd9e, "kmem_cache_size" },
	{ 0x85b2e26b, "kmem_cache_name" },
	{ 0xd4c1dda0, "kmem_cache_create" },
	{ 0x2dc45af9, "kmem_cache_destroy" },
	{ 0xb72397d5, "printk" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "5A2DDABA10C2DC968DEA088");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 3,
};
