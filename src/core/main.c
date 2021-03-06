#include "config.h"
#include "lb.h"

static int args_parse(int argc ,char **argv ,config_t *gconfig);
static void usage(void);

/**
 * @brief main 
 *   程序入口函数
 */
int main(int argc ,char **argv)
{

    int ret = 0;
    config_t gconfig = {NULL};

    //
    //解析参数
    ret = args_parse(argc ,argv ,&gconfig);
    assert_error(!ret ,ret);
    //
    //h表示查看程序支持的参数
    assert_void(!gconfig.h ,usage(););
    //
    //获取配置文件路径 解析配置文件
    ret = config_loading(&gconfig);
    assert_error(!ret ,ret);
    //
    //开启守护进程
#ifdef __USE_UNIX__
    assert_void(!gconfig.d ,use_daemon());
#endif
    //
    //加载模块
    ret = module_init(&gconfig);
    assert_error(!ret ,ret);

    //负载均衡模块
    ret = lb_process(&gconfig);
    assert_error(!ret ,ret);
    //
    return 0;
}

/**
 * @brief usage 
 *  使用参数说明
 */
static void usage(void)
{
    printf("Usage:.....\n");
    exit(0);
}

/**
 * @brief args_parse 
 *   解析参数获取配置文件路径等
 */
static int args_parse(int argc ,char **argv ,config_t *gconfig)
{
    int i = 0;
    unsigned char *p = NULL;

    for(i = 1; i < argc; i++){
        p = (unsigned char *)argv[i];

        if(*p++ != '-'){
            printf("error args\n");
            return -EINVAL;
        }

        do{
            switch(*p++){
                case 'h':
                    {
                        gconfig->h = 1;
                        break;
                    }
                case 'd':
                    {
                        gconfig->d = 1;
                        break;
                    }
                case 'c':
                    {
                        gconfig->path = argv[++i];
                        break;
                    }
                default:
                    {
                        printf("not support this args [%s]\n" ,argv[i]);
                        return -EINVAL;
                    }
            }
        }while(*p);
    }

    return 0;
}

