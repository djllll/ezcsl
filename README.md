# EzCsl
Ezcsl (Easy Console) is a console programming by C ,that can be used for MCU terminal simulation

## Migration
1. copy `ezcsl.c/h  ezcsl_port.c/h` to your project;
2. modify the file `ezcsl_port.c`;
    1. `ezport_receive_a_char` use to receive a character,call it in your receiver;
    2. write your own sending code in `ezport_send_str`
3. ok.

## Usage
1. call `ezcsl_init` for initialization
2. call `ezcsl_cmd_unit_create` to create a cmd unit (first command)
3. call `ezcsl_cmd_register`to create your cmd (second command)
4. call `ezcsl_printf` to print everywhere you want (recommend call it in callback)
5. put `ezcsl_tick` in a loop

## Macro Configuration
|Macro|Meaning|
|:--:|:--:|
|CSL_BUF_LEN|Terminal buffer size, which is the maximum number of characters that the terminal can input|
|HISTORY_BUF_LEN     |History buffer size|        
|PRINT_BUF_LEN   |Printf buffer size|
|PARA_LEN_MAX    |Maximum number of parameters|
|SPLIT_CHAR    |Split Character|
|LOG_DEFINE    |Enable/Disable log level by macro|
|USE_EZ_MODEM    |choose MODEM (EZ_NO_MODEM or EZ_YMODEM_1K)|

# Tutorial
## Command Unit

## Parameters Description

## Ymodem

## 


# Example 
The example can work on Win

## How to run example(tcp server mode)
1. clone this repo
2. cd this repo
3. `gcc ./example/*.c ./src/*.c -Iexample -Isrc -lws2_32 -DTCP_MODE -o example` compile TCP_MODE
4. run `example(.exe)`
5. terminal 

## How to run example(tcp server mode)
1. clone this repo
2. cd this repo
3. `gcc ./example/*.c ./src/*.c -Iexample -Isrc -lws2_32 -DTCP_MODE -o example` compile TCP_MODE
4. run `example(.exe)

## screenshot
welcome  
![](./docs/screenshot/welcome.png)

autocomplete & prompt  
![](./docs/screenshot/autocomplete.gif)

## How to make a welcome-message?  
[Text to ASCII Art Generator](https://patorjk.com/software/taag/)  

## TODO

