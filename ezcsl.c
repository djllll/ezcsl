#include "ezcsl.h"
#include "stdlib.h" // malloc
#include "ezrb.h"
#include "ezstring.h"
#include "stdio.h"  // vsprintf


#define IS_VISIBLE(c)       ((c) >= 0x20 && (c) <= 0x7e)
#define IS_BACKSPACE(c)     ((c)==0x08 || (c)==0x7f)
#define IS_TAB(c)           ((c)==0x09)
#define IS_ENTER(c)         ((c)==0x0d)
#define IS_CTRL_C(c)        ((c)==0x03)
#define IS_CTRL_D(c)        ((c)==0x04)


#define EXPAND_DESC(c) ((c)=='s'?"string":((c)=='i'?"integer":((c)=='f'?"float":"unkown")))

const char* strNULL="";
#define CHECK_NULL_STR(c) ((c)==NULL?strNULL:(c))




static struct EzCslHandleStruct {
    /* prefix */
    const char *prefix;
    uint8_t prefix_len;

    /* buffer */
    char buf[CSL_BUF_LEN];
    uint16_t bufp;
    uint16_t bufl;

    /* history */
    char hist_buf[HISTORY_BUF_LEN];
    uint8_t cur_hist_idx;
    uint16_t hist_buf_rest;

    /* ringbuffer */
    ezrb_t *rb;

    /* lock */
    volatile uint8_t lock;

    /* log */
    ez_log_level_mask_t log_level_mask;

    /* xmodem */
#ifdef USE_EZ_XMODEM
    const char *modem_prefix;
    xmodem_cfg_t *modem_cfg;
#endif
    /* sudo */
    const char *sudo_psw;
    uint8_t sudo_checked;
    uint8_t psw_inputing;
} ezhdl;


#define LOCK() do{while(ezhdl.lock!=0){LOCK_WAIT_DELAY();};ezhdl.lock=1;}while(0)
#define UNLOCK() do{ezhdl.lock=0;}while(0)


/* ez console port function */
void ezport_receive_a_char(char c);

void ezcsl_init(const char *prefix ,const char *welcome,const char *sudo_psw);
void ezcsl_log_level_set(ez_log_level_mask_t mask);
uint8_t ezcsl_log_level_allowed(ez_log_level_mask_t mask);
#ifdef USE_EZ_XMODEM
void ezcsl_xmodem_set(const char *modem_prefix,xmodem_cfg_t *cfg);
#endif
void ezcsl_deinit(void);
uint8_t ezcsl_tick(void);
void ezcsl_reset_prefix(void);
void ezcsl_printf(const char *fmt, ...);

static void ezcsl_reset_empty(void);
static void ezcsl_tabcomplete(void);
static void ezcsl_submit(void);
static void buf_to_history(void);
static uint8_t load_history(void);
static void last_history_to_buf(void);
static void next_history_to_buf(void);

static ez_cmd_t *cmd_head=NULL;
static ez_cmd_unit_t *cmd_unit_head=NULL;
ez_cmd_unit_t *ezcsl_cmd_unit_create(const char *title_main,const char *describe ,uint8_t need_sudo, void (*callback)(uint16_t,ez_param_t*));
ez_sta_t ezcsl_cmd_register(ez_cmd_unit_t *unit,uint16_t id,const char *title_sub,const char *describe,const char* para_desc);

/* ez inner cmd */
static void ezcsl_cmd_help_callback(uint16_t id,ez_param_t* para);



/**
 * @brief reset with prefix
 * 
 */
void ezcsl_reset_prefix(void)
{
    ezcsl_printf(MOVE_CURSOR_ABS(0)"%s"ERASE_TO_END(), ezhdl.prefix);
    ezhdl.buf[0] = 0;
    ezhdl.bufl = 0;
    ezhdl.bufp = 0;
}

/**
 * @brief reset with empty prefix
 * 
 */
static void ezcsl_reset_empty(void)
{
    ezcsl_printf(MOVE_CURSOR_ABS(0)ERASE_TO_END());
    ezhdl.buf[0] = 0;
    ezhdl.bufl = 0;
    ezhdl.bufp = 0;
}


/**
 * @param  c the char from input
 * @author Jinlin Deng
 */
void ezport_receive_a_char(char c)
{
    ezrb_push(ezhdl.rb,(uint8_t)c);
}



/**
 * @brief init
 * 
 * @param prefix prefix of shell
 * @param welcome 
 * @param sudo_psw password of sudo, NULL = no sudo  
 */
void ezcsl_init(const char *prefix,const char *welcome,const char *sudo_psw)
{
#ifdef USE_EZ_XMODEM
    ezhdl.modem_prefix = NULL;
#endif
    ezhdl.prefix_len = estrlen_s(prefix,CSL_BUF_LEN);
    ezhdl.prefix = prefix;
    ezhdl.bufp = 0;
    ezhdl.bufl = 0;

    ezhdl.cur_hist_idx = 0;
    ezhdl.hist_buf_rest = HISTORY_BUF_LEN;
    for (uint16_t i = 0; i < HISTORY_BUF_LEN; i++) {
        ezhdl.hist_buf[i] = 0;
    }

    ezhdl.sudo_psw = sudo_psw;
    ezhdl.psw_inputing = 0;
    ezhdl.sudo_checked = 0;

    ezhdl.rb = ezrb_create(CSL_BUF_LEN/2);

    ezcsl_log_level_set(LOG_LEVEL_ALL);
    ez_cmd_unit_t *unit = ezcsl_cmd_unit_create("?","help",0,ezcsl_cmd_help_callback);
    ezcsl_cmd_register(unit,0,NULL,NULL,"");
    ezport_send_str((char*)welcome,estrlen(welcome)); 
    ezcsl_printf("you can input '?' for help\r\n");
    ezcsl_reset_prefix();
}

#ifdef USE_EZ_XMODEM
/**
 * @brief modem init (optional),if you need xmodem,call this after `ezcsl_init`
 *
 * @param modem_prefix
 */
void ezcsl_xmodem_set(const char *modem_prefix,xmodem_cfg_t *cfg)
{
    ezhdl.modem_prefix = modem_prefix;
    ezhdl.modem_cfg = cfg;
}
#endif

/**
 * @brief set loglevel
 * 
 * @param mask 
 */
void ezcsl_log_level_set(ez_log_level_mask_t mask){
    ezhdl.log_level_mask = mask;
}

/**
 * @brief log level get
 * 
 * @return ez_log_level_mask_t 
 */
uint8_t ezcsl_log_level_allowed(ez_log_level_mask_t mask){
    return (ezhdl.log_level_mask & mask);
}


/**
 * @brief deinit
 * 
 */
void ezcsl_deinit(void){
    ez_cmd_t *p1=cmd_head;
    while(p1!=NULL){
        ez_cmd_t *p_del=p1;
        p1=p1->next;
        free(p_del);
    }
    ez_cmd_unit_t *p2=cmd_unit_head;
    while(p2!=NULL){
        ez_cmd_unit_t *p_del=p2;
        p2=p2->next;
        free(p_del);
    }
    ezrb_destroy(ezhdl.rb);
}


#define DIR_KEY_DETECT(c, up, down, left, right) \
    do {                                         \
        if (c == left) {                         \
            if (ezhdl.bufp > 0) {                \
                ezcsl_printf(CURSOR_BACK(1));    \
                ezhdl.bufp--;                    \
            };                                   \
        } else if (c == right) {                 \
            if (ezhdl.bufp < ezhdl.bufl) {       \
                ezcsl_printf(CURSOR_FORWARD(1)); \
                ezhdl.bufp++;                    \
            }                                    \
        } else if (c == up) {                    \
            last_history_to_buf();               \
        } else if (c == down) {                  \
            next_history_to_buf();               \
        }                                        \
    } while (0)

#define DELETE_KEY_DETECT(c, delete)                               \
    do {                                                           \
        if (c == delete &&ezhdl.bufp < ezhdl.bufl) {               \
            ezcsl_printf(SAVE_CURSOR_POS());                       \
            for (uint16_t i = ezhdl.bufp; i < ezhdl.bufl; i++) { \
                ezhdl.buf[i] = ezhdl.buf[i + 1];                   \
                ezport_send_str(ezhdl.buf + i, 1);                 \
            }                                                      \
            ezhdl.bufl--;                                          \
            ezcsl_printf(ERASE_TO_END() RESTORE_CURSOR_POS());     \
        }                                                          \
    } while (0)


#define IS_POWERSHELL_PREFIX(c) (c==0x00)
#define IS_BASH_PREFIX(c)       (c==0x1b)
#define IS_BASH_1_PREFIX(c)     (c=='[')
#define IS_BASH_2_PREFIX(c)     (c=='3')

#define MATCH_MODE_DEFAULT      0
#define MATCH_MODE_POWERSHELL   1
#define MATCH_MODE_BASH         2
#define MATCH_MODE_BASH_1       3
#define MATCH_MODE_BASH_2       4

/**
 * @brief call it in a loop
 * 
 */
uint8_t ezcsl_tick(void) {
    static uint8_t match_mode = MATCH_MODE_DEFAULT;
    uint8_t c;
    while (ezrb_pop(ezhdl.rb, &c) == RB_OK) {
        switch (match_mode) {
        case MATCH_MODE_POWERSHELL:
            DIR_KEY_DETECT(c, 'H', 'P', 'K', 'M');
            DELETE_KEY_DETECT(c, 'S');
            match_mode = MATCH_MODE_DEFAULT;
            break;
        case MATCH_MODE_BASH:
            if (IS_BASH_1_PREFIX(c)) {
                match_mode = MATCH_MODE_BASH_1;
                break;
            }
            match_mode = MATCH_MODE_DEFAULT;
            break;
        case MATCH_MODE_BASH_1:
            if (IS_BASH_2_PREFIX(c)) {
                match_mode = MATCH_MODE_BASH_2;
                break;
            }
            DIR_KEY_DETECT(c, 'A', 'B', 'D', 'C');
            match_mode = MATCH_MODE_DEFAULT;
            break;
        case MATCH_MODE_BASH_2:
            DELETE_KEY_DETECT(c, '~');
            match_mode = MATCH_MODE_DEFAULT;
            break;
            break;
        case MATCH_MODE_DEFAULT:
        default:
            if (IS_VISIBLE(c) && ezhdl.bufl < CSL_BUF_LEN - 1) {
                /* visible char */
                for (uint16_t i = ezhdl.bufl; i >= ezhdl.bufp + 1; i--) {
                    ezhdl.buf[i] = ezhdl.buf[i - 1];
                }
                ezhdl.buf[ezhdl.bufp] = c;
                if (!ezhdl.psw_inputing) {
                    ezcsl_printf(SAVE_CURSOR_POS());
                    ezport_send_str(ezhdl.buf + ezhdl.bufp, ezhdl.bufl - ezhdl.bufp + 1);
                    ezcsl_printf(RESTORE_CURSOR_POS() CURSOR_FORWARD(1));
                }
                ezhdl.bufp++;
                ezhdl.bufl++;
            } else if (IS_BACKSPACE(c) && ezhdl.bufp > 0) {
                /* backspace */
                if (!ezhdl.psw_inputing) {
                    ezcsl_printf(CURSOR_BACK(1) SAVE_CURSOR_POS());
                }
                for (uint16_t i = ezhdl.bufp - 1; i < ezhdl.bufl-1; i++) {
                    ezhdl.buf[i] = ezhdl.buf[i + 1];
                    if (!ezhdl.psw_inputing) {
                        ezport_send_str(ezhdl.buf + i, 1);
                    }
                }
                ezhdl.bufp--;
                ezhdl.bufl--;
                if (!ezhdl.psw_inputing) {
                    ezcsl_printf(ERASE_TO_END() RESTORE_CURSOR_POS());
                }
            } else if (IS_ENTER(c)) {
                /* enter */
                ezhdl.buf[ezhdl.bufl] = 0; // cmd end
                if (!ezhdl.psw_inputing) {
                    ezcsl_submit();
                } else {
                    if(estrcmp(ezhdl.sudo_psw,ezhdl.buf)==0){
                        /* password success */
                        ezcsl_printf(COLOR_GREEN("\r\nPassword Checked!\r\n"));
                        ezcsl_reset_prefix();
                        ezhdl.sudo_checked = 1;
                        ezhdl.psw_inputing = 0;
                    }else{
                        ezcsl_printf(COLOR_RED("\r\nWrong Password! Try again.\r\n"));
                        ezcsl_reset_empty();
                    }
                }
            } else if (IS_CTRL_C(c)) {
                /* ctrl+c */
                ezcsl_printf("^C\r\n");
                ezcsl_reset_prefix();
                if(ezhdl.psw_inputing){
                    ezhdl.psw_inputing = 0;
                }
            } else if (IS_CTRL_D(c)) {
                /* ctrl+D */
                ezcsl_printf("^D\r\n");
                ezcsl_reset_prefix();
                if(ezhdl.psw_inputing){
                    ezhdl.psw_inputing = 0;
                }
                return 1;
            } else if (IS_TAB(c) && !ezhdl.psw_inputing) {
                /* tab */
                ezhdl.buf[ezhdl.bufp] = 0; // cmd end
                ezcsl_tabcomplete();
            } else if (IS_POWERSHELL_PREFIX(c)) {
                match_mode = MATCH_MODE_POWERSHELL;
            } else if (IS_BASH_PREFIX(c)) {
                match_mode = MATCH_MODE_BASH;
            }
            break;
        }
    }
    return 0;
}


/**
 * @brief printf
 * 
 * @param fmt 
 * @param ... 
 */
void ezcsl_printf(const char *fmt, ...){
    LOCK();
    uint16_t printed;
    va_list args;
    char dat_buf[PRINT_BUF_LEN];
    va_start(args, fmt);
    printed = vsnprintf(dat_buf,PRINT_BUF_LEN, fmt, args); 
    ezport_send_str(dat_buf, printed);  
    va_end(args);
    UNLOCK();
}



/**
 * @brief submit input
 * 
 */
static void ezcsl_submit(void)
{
#ifdef USE_EZ_XMODEM
    // if(ezhdl.modem_prefix!=NULL && ezhdl.modem_cfg!=NULL){
    //     if(estrncmp(ezhdl.modem_prefix,ezhdl.buf,estrlen(ezhdl.modem_prefix))==0){
    //         if(xmodem_start(ezhdl.rb,ezhdl.modem_cfg) == X_TRANS_TIMEOUT){
    //             ezcsl_printf(COLOR_RED("Xmodem Timeout!"));
    //         }else{
    //             ezcsl_printf(COLOR_RED("Xmodem OK!"));
    //         }
    //         return;
    //     }
    // }
#endif
    uint8_t paranum=0;
    float paraF[PARA_LEN_MAX];
    int paraI[PARA_LEN_MAX];
    ez_param_t para[PARA_LEN_MAX];
    char *cmd=ezhdl.buf;
    const char *subtitle=strNULL;
    const char *maintitle=strNULL;
    
    char *a_split;
    uint8_t split_cnt=0;

    buf_to_history();

    if (ezhdl.bufl > CSL_BUF_LEN - 1) {
        ezhdl.bufl = CSL_BUF_LEN - 1;
    }
    ezhdl.buf[ezhdl.bufl]=SPLIT_CHAR; // add a SPLIT_CHR to the end for estrtokc 
    // ezhdl.buf[ezhdl.bufl+1]=0; // add a SPLIT_CHR to the end for estrtokc 
    while (1) {
        a_split = estrtokc((char *)cmd, SPLIT_CHAR);
        if (a_split != NULL) {
            switch (split_cnt) {
            case 0:
                maintitle=a_split;
                break;
            case 1:
                subtitle=a_split;
                break;
            default:
                if(paranum<PARA_LEN_MAX && estrlen(a_split)>0){
                    para[paranum]=(ez_param_t*)a_split;
                    paranum++;
                }
                break;
            }
        } else {
            break;
        }
        split_cnt++;
        cmd=NULL; //for estrtokc continue
    };
    

    ezcsl_printf("\r\n");
    // Cmd Match 
    ez_cmd_t *cmd_p = cmd_head;
    uint8_t match_ok_flag = 0; // 0 match fail ,1 main match ok ,2 main and sub  match  ok
    while (cmd_p != NULL) {
        if (estrcmp(cmd_p->unit->title_main, maintitle) == 0) {
            match_ok_flag = 1;
            if (cmd_p->unit->need_sudo && !ezhdl.sudo_checked && ezhdl.sudo_psw!=NULL) {
                /* query sudo password */
                ezcsl_printf("Please Input Sudo Password :\r\n");
                ezcsl_reset_empty();
                ezhdl.psw_inputing = 1;
                return;
            }
            if (estrcmp(cmd_p->title_sub, subtitle) == 0) {
                match_ok_flag = 2;
                if (cmd_p->para_num == paranum) {
                    /* match ok ,ready to exec cmd */
                    for (uint8_t i = 0; i < paranum; i++) {
                        switch (cmd_p->para_desc[i]) {
                        case 's': {
                            para[i] = (void *)para[i];
                        } break;
                        case 'i': {
                            paraI[i] = (int)atoi((const char *)para[i]);
                            para[i] = (void*)&paraI[i];
                        } break;
                        case 'f': {
                            paraF[i] = (float)atof((const char *)para[i]);
                            para[i] = (void*)&paraF[i];
                        } break;
                        default:
                            break;
                        }
                    }
                    cmd_p->unit->callback(cmd_p->id,para); // user can use `ezcsl_printf` in callback
                } else {
                    ezcsl_printf("\033[31mCmd Error!\033[m %s,%s", maintitle, subtitle);
                    for(uint8_t i=0;i<cmd_p->para_num;i++){
                    ezcsl_printf(",<%s>",EXPAND_DESC(cmd_p->para_desc[i]));
                    }
                    ezcsl_printf(" : %s\r\n",cmd_p->describe);
                }
                break;
            }
        }
        cmd_p = cmd_p->next;
    }

    switch (match_ok_flag)
    {
    case 0:
        ezcsl_printf(COLOR_RED("Unknown Command")" %s\r\n",maintitle);
        break;
    case 1:
        cmd_p = cmd_head;
        ezcsl_printf(COLOR_GREEN("Sub Command & Description List ")"\r\n");
        ezcsl_printf(COLOR_GREEN("========================= ")"\r\n");
        while (cmd_p != NULL) {
            if (estrcmp(cmd_p->unit->title_main, maintitle) == 0) {
                ezcsl_printf("%s,%s:  %s\r\n", cmd_p->unit->title_main, cmd_p->title_sub, cmd_p->describe);
            }
            cmd_p = cmd_p->next;
        }
        ezcsl_printf(COLOR_GREEN("========================= ")"\r\n");
        break;
    default:
        break;
    }
        
    
    ezcsl_reset_prefix();
}


/**
 * @brief auto complete
 * 
 */
static void ezcsl_tabcomplete(void)
{
    char existed_cmdbuf[CSL_BUF_LEN] = {0};
    uint8_t match_ok_cnt = 0;
    if(estrlen_s(ezhdl.buf,CSL_BUF_LEN)==0){
        return;
    }
    ez_cmd_t *p;
    p = cmd_head;
    while (p != NULL) {
        existed_cmdbuf[0] = 0;
        estrcat_s(existed_cmdbuf,CSL_BUF_LEN,p->unit->title_main);
        estrcatc_s(existed_cmdbuf,CSL_BUF_LEN, SPLIT_CHAR);
        estrcat_s(existed_cmdbuf,CSL_BUF_LEN, p->title_sub);
        if (estrncmp(ezhdl.buf, existed_cmdbuf, estrlen_s(ezhdl.buf,CSL_BUF_LEN)) == 0) {
            match_ok_cnt++;
        }
        p = p->next;
    }
    if (match_ok_cnt == 1) {
        p = cmd_head;
        while (p != NULL) {
            existed_cmdbuf[0] = 0;
            estrcat_s(existed_cmdbuf,CSL_BUF_LEN, p->unit->title_main);
            estrcatc_s(existed_cmdbuf,CSL_BUF_LEN, SPLIT_CHAR);
            estrcat_s(existed_cmdbuf,CSL_BUF_LEN, p->title_sub);
            if (estrncmp(ezhdl.buf, existed_cmdbuf, estrlen_s(ezhdl.buf,CSL_BUF_LEN)) == 0) {
                estrcpy_s(ezhdl.buf,CSL_BUF_LEN, existed_cmdbuf);
                ezhdl.bufp = ezhdl.bufl = estrlen_s(ezhdl.buf,CSL_BUF_LEN);
                ezcsl_printf(MOVE_CURSOR_ABS(0)"%s%s"ERASE_TO_END(),ezhdl.prefix,ezhdl.buf);
                break;
            }
            p = p->next;
        }
    } else if (match_ok_cnt > 1) {
        char autocomplete[CSL_BUF_LEN]={0};
        ezport_send_str((char*)"\r\n",2);
        p = cmd_head;
        while (p != NULL) {
            existed_cmdbuf[0] = 0;
            estrcat_s(existed_cmdbuf,CSL_BUF_LEN, p->unit->title_main);
            estrcatc_s(existed_cmdbuf,CSL_BUF_LEN, SPLIT_CHAR);
            estrcat_s(existed_cmdbuf,CSL_BUF_LEN, p->title_sub);
            if (estrncmp(ezhdl.buf, existed_cmdbuf, estrlen_s(ezhdl.buf,CSL_BUF_LEN)) == 0) {
                ezcsl_printf("%s\t", existed_cmdbuf);
                if (autocomplete[0] == 0) {
                    estrcpy_s(autocomplete,CSL_BUF_LEN,existed_cmdbuf);
                }else{
                    for(uint16_t i=0;i<estrlen_s(autocomplete,CSL_BUF_LEN);i++){
                        if(autocomplete[i]!=existed_cmdbuf[i]){
                            autocomplete[i]=0;
                            break;
                        }
                    }
                }
            }
            p = p->next;
        }
        estrcpy_s(ezhdl.buf,CSL_BUF_LEN,autocomplete);
        ezhdl.bufp=ezhdl.bufl=estrlen_s(ezhdl.buf,CSL_BUF_LEN);
        ezport_send_str((char*)"\r\n",2);
        ezport_send_str((char*)ezhdl.prefix, ezhdl.prefix_len);
        ezport_send_str(ezhdl.buf, ezhdl.bufl);
    }
}


/**
 * create a cmd unit
 * @param title_main main title ,cannot null or '' ,length < 10
 * @author Jinlin Deng
 */
ez_cmd_unit_t *ezcsl_cmd_unit_create(const char *title_main,const char *describe ,uint8_t need_sudo,void (*callback)(uint16_t,ez_param_t* )){
    LOCK();
    if (estrlen(title_main)==0 || estrlen(title_main)>=10 || callback==NULL){
        return NULL;
    }
    
        ez_cmd_unit_t *p = cmd_unit_head;
        while (p != NULL) { //duplicate
            if(estrcmp(p->title_main,title_main)==0){
                UNLOCK();
                return NULL;
            }
            p = p->next;
        }
        
        ez_cmd_unit_t *p_add = (ez_cmd_unit_t *)malloc(sizeof(ez_cmd_unit_t));
        p_add->describe=CHECK_NULL_STR(describe);
        p_add->next = NULL;
        p_add->title_main = title_main;
        p_add->callback=callback;
        p_add->need_sudo=need_sudo;
        
        if(cmd_unit_head==NULL){
            cmd_unit_head=p_add;
        }else{
            p = cmd_unit_head;
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = p_add;
        }
        
        UNLOCK();
        return p_add;
}


/**
 * create a cmd unit
 * @param title_sub sub title ,can set null 
 * @param describe describe your cmd
 * @param para_desc the description of parameters your cmd need,s->string,i->int,f->float
 * @author Jinlin Deng
 * @return register result
 */
ez_sta_t ezcsl_cmd_register(ez_cmd_unit_t *unit, uint16_t id, const char *title_sub, const char *describe, const char *para_desc)
{
    if (estrlen(para_desc) > PARA_LEN_MAX) {
        return EZ_ERR;
    }
    ez_cmd_t *p = cmd_head;
    while (p != NULL) { // duplicate
        if (estrcmp(p->unit->title_main, unit->title_main) == 0 && (estrcmp(p->title_sub, title_sub) == 0 || p->id == id)) {
            return EZ_ERR;
        }
        p = p->next;
    }

    ez_cmd_t *p_add = (ez_cmd_t *)malloc(sizeof(ez_cmd_t));
    p_add->describe = CHECK_NULL_STR(describe);
    p_add->next = NULL;
    p_add->title_sub = CHECK_NULL_STR(title_sub);
    p_add->para_num = estrlen(para_desc);
    p_add->para_desc = para_desc;
    p_add->unit = unit;
    p_add->id = id;

    if (cmd_head == NULL) {
        cmd_head = p_add;
    } else {
        p = cmd_head;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = p_add;
    }

    return EZ_OK;
}



/**
 * @brief help cmd callback
 * 
 * @param id cmd id
 * @param para param
 */
static void ezcsl_cmd_help_callback(uint16_t id,ez_param_t* para)
{
    ez_cmd_unit_t *p = cmd_unit_head;
    ezcsl_printf(COLOR_GREEN("Main Command & Description List")" \r\n");
    ezcsl_printf(COLOR_GREEN("=========================")" \r\n");
    while (p!= NULL) {
        ezcsl_printf("%-10s %s\r\n", p->title_main,  p->describe);
        p = p->next;
    }
    ezcsl_printf(COLOR_GREEN("=========================")" \r\n");
}


static uint8_t last_load_hist = 0; // ugly flag...
/**
 * move the buf to history
 * @param
 * @author Jinlin Deng
 */
static void buf_to_history(void)
{
    for (uint16_t i = 0; i < HISTORY_BUF_LEN; i++) {
        uint16_t idx = HISTORY_BUF_LEN - 1 - i;
        if (idx > ezhdl.bufl) {
            ezhdl.hist_buf[idx] = ezhdl.hist_buf[idx - ezhdl.bufl - 1];
        } else if (idx < ezhdl.bufl) {
            ezhdl.hist_buf[idx] = ezhdl.buf[idx];
        } else {
            ezhdl.hist_buf[idx] = 0;
        }
    }
    ezhdl.cur_hist_idx = 0;
    last_load_hist = 0;
}


/**
 * move history to buf
 * @author Jinlin Deng
 */
static uint8_t load_history(void)
{
    uint8_t hist_num = 0;
    char *hist_start = ezhdl.hist_buf;
    for (uint16_t i = 0; i < HISTORY_BUF_LEN; i++) {
        if (ezhdl.hist_buf[i] == 0) {
            if (hist_num == ezhdl.cur_hist_idx) {
                estrcpy_s(ezhdl.buf, CSL_BUF_LEN, hist_start);
                ezhdl.bufl = ezhdl.bufp = estrlen_s(ezhdl.buf, CSL_BUF_LEN);
                ezcsl_printf(MOVE_CURSOR_ABS(0) "%s%s" ERASE_TO_END(), ezhdl.prefix, ezhdl.buf);
                return 1;
            } else {
                if (ezhdl.hist_buf[i + 1] == 0) {
                    return 0;
                }
                hist_start = &ezhdl.hist_buf[i + 1];
                hist_num++;
            }
        }
    }
}


/**
 * @brief load last record
 * 
 */
static void last_history_to_buf(void)
{
    if (last_load_hist == 2) {
        ezhdl.cur_hist_idx++;
    }
    if (load_history()) {
        last_load_hist = 1;
        ezhdl.cur_hist_idx++;
    }
}

/**
 * @brief load next record
 * 
 */
static void next_history_to_buf(void)
{

    if (ezhdl.cur_hist_idx > 0) {
        if (last_load_hist == 1) {
            ezhdl.cur_hist_idx--;
        }
        ezhdl.cur_hist_idx--;
    }
    load_history();
    last_load_hist = 2;
}