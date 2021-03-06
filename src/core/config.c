#include "config.h"

static cJSON *__config_load(const char *path);
static int __config_check(const char *path);

/**
 * @brief config_loading 
 *   检查配置文件合法性
 *   解析配置文件
 */
int config_loading(config_t *gconfig)
{
    int ret = 0;

    ret = __config_check(gconfig->path);
    assert_error(!ret ,ret);

    gconfig->json_ptr = __config_load(gconfig->path);
    assert_error(gconfig->json_ptr ,-EACCES);

    return 0;
}

/**
 * @brief __config_load 
 *   读取到内存，解析成json
 */
static cJSON *__config_load(const char *path)
{
    int fd = 0;
    cJSON *root = NULL;
    char *buffer = NULL;
    struct stat st = {0};

    fd = open(path ,O_RDONLY);
    assert_error(fd ,NULL);

    assert_goto(!fstat(fd ,&st) ,out ,);

    buffer = malloc(st.st_size);
    assert_goto(buffer ,out ,);

    memset(buffer ,0x00 ,st.st_size);
    read(fd ,buffer ,st.st_size);

    root = cJSON_Parse(buffer);

    free(buffer);

out:
    close(fd);

    return root;
}

/**
 * @brief config_check 
 *   校验配置文件
 */
static int __config_check(const char *path)
{
    return 0;
}


