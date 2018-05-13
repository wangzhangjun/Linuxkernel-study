# linux内核hashtable
参考链接：
```
https://kernelnewbies.org/FAQ/Hashtables
http://yupeng0921.blogspot.hk/2010/10/linux-hash-table.html?showComment=1350877062473#c3513363701255844366
```
最新版本的内核hashtable在#include <linux/hashtable.h>这个函数中，哈希表的实现如下图：
![](assets/markdown-img-paste-20180512235306423.png)

### 1.如何定义一个hashtable
```
15 #define DEFINE_HASHTABLE(name, bits)                                            \
16         struct hlist_head name[1 << (bits)] =                                   \
17                         { [0 ... ((1 << (bits)) - 1)] = HLIST_HEAD_INIT }
```
Notice that we use the number of bits to define a hashtable and not the size, so the following declaration will have 8 buckets.
```
DEFINE_HASHTABLE(a, 3);
```
### 2.添加一个node，使用的API是：
```
hash_add(a, &first.next, first.data);
```
### 3.遍历一个hashtable的API:
```
hash_for_each_safe
```

### 4.完整的demo
```
#include <linux/init.h>
#include <linux/module.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/hashtable.h>
#include <linux/jhash.h>
#include <linux/types.h>
typedef struct student
{
    int type;
	int number;
    char buf[30];
}student_t;

typedef struct students_list
{
	int count;                       
	student_t rules[];
}students_list_t;

typedef struct student_rule {
    struct hlist_node node;
    uint32_t index;
    student_t rule;
}student_rule_t;

#define for_each_rule(rules_list, rule) \
    for ((rule) = (rules_list)->rules; (rule) - (rules_list)->rules < (rules_list)->count; (rule)++)

#define current_rule_index(rules_list, rule)    ((rule) - (rules_list)->rules)

#define DEFAULT_RULE_HASH_BUCKET_BITS   17

//初始化hash_head
static DEFINE_HASHTABLE(student_active_rules, DEFAULT_RULE_HASH_BUCKET_BITS);
static DEFINE_HASHTABLE(student_active_rules_copy, DEFAULT_RULE_HASH_BUCKET_BITS);

#define jhash_string(str) jhash((str), strlen(str), 0x95279527)

void PrintfArray(student_t *p, int num)
{
    int i = 0;
    for(i=0; i<num; i++)
    {
        printk("%d========%d\n",i, p[i].number);
    }
}

static void add_hash_table(students_list_t *rules)
{
    const student_t *rule;
    for_each_rule(rules, rule){
        student_rule_t *entry = (student_rule_t*)kmalloc(sizeof(student_rule_t), GFP_ATOMIC);
        if(!entry){
            printk("kmalloc error in build_hash_table\n");
        }
        memcpy(&entry->rule, rule, sizeof(*rule));
        entry->index = current_rule_index(rules, rule);
        hash_add(student_active_rules_copy, &entry->node, jhash_string(rule->buf));
    }
}

//遍历hashtable
static void test_for_each_hash_table(students_list_t *rules_add)
{
    int bkt;
    student_rule_t *rule;
    struct hlist_node *tmp;

    printk("get this hashtable size:\n");
    printk("get this hashtable size:%lu\n", HASH_SIZE(student_active_rules));

    if(hash_empty(student_active_rules)){
        printk("hash table is empty========================\n");
    }
    printk("before delete,hava type0========================\n");
    //原来的hashtable
    hash_for_each_safe(student_active_rules, bkt, tmp, rule, node) {
        int idx = rule->index;
        printk("hashtable===index:%d\n", idx);
        printk("hashtable===buf:%s\n", rule->rule.buf);
        printk("hashtable===number:%d\n", rule->rule.number);
        printk("hashtable===type:%d\n", rule->rule.type);
    }

    //删除掉type为0的node
    hash_for_each_safe(student_active_rules, bkt, tmp, rule, node) {
        if(rule->rule.type == 0){
            hash_del(&rule->node);
            kfree(rule);
        }
    }

    //copy
    hash_for_each_safe(student_active_rules, bkt, tmp, rule, node) {
        hash_add(student_active_rules_copy, &rule->node, jhash_string(rule->rule.buf));
    }

    //再add进去新的
    add_hash_table(rules_add);

    printk("after delete, No type0========================\n");
    //原来的hashtable
    hash_for_each_safe(student_active_rules, bkt, tmp, rule, node) {
        int idx = rule->index;
        printk("hashtable===index:%d\n", idx);
        printk("hashtable===buf:%s\n", rule->rule.buf);
        printk("hashtable===number:%d\n", rule->rule.number);
        printk("hashtable===type:%d\n", rule->rule.type);
    }
    printk("after copy, have type0========================\n");
    //测试拷贝后的hashtable
    hash_for_each_safe(student_active_rules_copy, bkt, tmp, rule, node) {
        int idx = rule->index;
        printk("hashtable===index:%d\n", idx);
        printk("hashtable===buf:%s\n", rule->rule.buf);
        printk("hashtable===type:%d\n", rule->rule.type);
    }
}

static void build_hash_table(students_list_t *rules, students_list_t *rules_add)
{
    const student_t *rule;
    for_each_rule(rules, rule){
        student_rule_t *entry = (student_rule_t*)kmalloc(sizeof(student_rule_t), GFP_ATOMIC);
        if(!entry){
            printk("kmalloc error in build_hash_table\n");
        }
        memcpy(&entry->rule, rule, sizeof(*rule));
        entry->index = current_rule_index(rules, rule);
        hash_add(student_active_rules, &entry->node, jhash_string(rule->buf));
    }
    test_for_each_hash_table(rules_add);
}



static int htest_init (void)
{
    int i = 0;
    student_t tArray[10];
    student_t tArrayAdd[3];
    //student_t *temp;
    students_list_t *fs_list = (students_list_t *)kmalloc(sizeof(students_list_t)+10*sizeof(student_t), GFP_ATOMIC);
    students_list_t *fs_list_add = (students_list_t *)kmalloc(sizeof(students_list_t)+3*sizeof(student_t), GFP_ATOMIC);

    for(i=0; i < 10; i++){
        tArray[i].number = i;
    }

    for(i=0; i < 3; i++){
        tArrayAdd[i].number = i;
    }

    strcpy(tArrayAdd[0].buf, "add123");
    tArrayAdd[0].type = 0;
    strcpy(tArrayAdd[1].buf, "add1234");
    tArrayAdd[1].type = 0;
    strcpy(tArrayAdd[2].buf, "add123456");
    tArrayAdd[2].type = 0;

    strcpy(tArray[0].buf, "123");
    tArray[0].type = 0;
    strcpy(tArray[1].buf, "1234");
    tArray[1].type = 0;
    strcpy(tArray[2].buf, "1235");
    tArray[2].type = 0;
    strcpy(tArray[3].buf, "1236");
    tArray[3].type = 1;
    strcpy(tArray[4].buf, "1237");
    tArray[4].type = 1;
    strcpy(tArray[5].buf, "1238");
    tArray[5].type = 1;
    strcpy(tArray[6].buf, "1239");
    tArray[6].type = 1;
    strcpy(tArray[7].buf, "12310");
    tArray[7].type = 1;
    strcpy(tArray[8].buf, "1212310");
    tArray[8].type = 1;
    strcpy(tArray[9].buf, "3412310");
    tArray[9].type = 1;

    if(fs_list_add != NULL)
    {
        fs_list_add->count = 3;
        //结构体数组拷贝
        memcpy(fs_list_add->rules, tArrayAdd, 3*sizeof(student_t));    
    }

    if(fs_list != NULL)
    {
        fs_list->count = 10;
        //结构体数组拷贝
        memcpy(fs_list->rules, tArray, 10*sizeof(student_t));    
    }

    //测试是否正确
    // for(i=0; i < 10; i++)
    // {
    //     temp = ((fs_list->rules)+i);
    //     printk("%s\n",temp->buf);
    // }

    build_hash_table(fs_list, fs_list_add);
    return 0;
}

static void htest_exit (void)
{
}

module_init(htest_init);
module_exit(htest_exit);

MODULE_LICENSE("GPL");


```
