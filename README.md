# EzCsl
> C语言控制台模拟程序，可以用于MCU与上位机的交互

## 代码移植
1. 将`ezcsl.c/h,ezrb.c/h`四个文件放入工程中
2. 将`ezport_receive_a_char`函数放入串口接收函数（或其他终端输入函数）中，参数`char c`为接收到的字符
3. 在函数`ezport_send_str`中，写入自己的串口发送函数
4. 移植完成

## EzCsl使用
1. 调用`ezcsl_init`初始化
2. 使用`ezcsl_cmd_unit_create`创建命令单元，即一级命令（回调函数中可以使用`ezcsl_send_printf`输出内容）
3. 使用`ezcsl_cmd_register`将命令注册到单元中，即二级命令
4. 循环调用`ezcsl_tick`函数
5. 此时即可使用`一级命令,二级命令,参数`与控制台交互

## 宏配置
|宏|含义|
|:--:|:--:|
|CSL_BUF_LEN|终端缓冲区大小，即终端最多可以输入的字符数|
|HISTORY_LEN     |最大历史记录存储条数|        
|PRINT_BUF_LEN   |终端每次输出缓冲区大小|
|PARA_LEN_MAX    |命令支持的最大参数数量|
|ez_param_t      |参数变量的类型|


# Example 
在PC上模拟终端，在根目录使用`gcc *.c -o example`即可编译
运行example进入终端模拟，按Esc退出