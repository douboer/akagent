#ifndef __MANGOSTEEN_MODULE_H__
#define __MANGOSTEEN_MODULE_H__

#include <unistd.h>
#include <stdio.h>
#include <linux/ioctl.h>
#include "config.h"
#include "list.h"

#include "assertion.h"

/**
 * @brief 模块元素定义
 */
typedef struct module_s{
    struct list_head list;
    const char *name;
    int (*init)(config_t *gconfig);
    int (*check)(config_t *gconfig);
}module_t;

int module_init(config_t *gconfig);

void module_register(module_t *m);

#endif
