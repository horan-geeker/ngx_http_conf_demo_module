#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

typedef struct
{
    ngx_str_t my_str;
    ngx_flag_t my_flag;
    ngx_array_t *my_str_array;
    ngx_array_t *my_keyval;
    ngx_int_t my_num;
    size_t my_size;
    off_t my_off;
    ngx_msec_t my_msec;
    time_t my_sec;
    ngx_bufs_t my_bufs;
    ngx_uint_t my_enum_seq;
    ngx_uint_t my_bitmask;
    ngx_uint_t my_access;
    ngx_path_t *my_path;
} ngx_http_conf_demo_conf_t;

// if use enums need define a reference
static ngx_conf_enum_t test_enums[] = {
    {ngx_string("apple"), 1},
    {ngx_string("banana"), 2},
    {ngx_string("orange"), 3},
    {ngx_null_string, 0}};

// if use bitmasks need define a reference
static ngx_conf_bitmask_t test_bitmasks[] = {
    {ngx_string("good"), 0x0002},
    {ngx_string("better"), 0x0004},
    {ngx_string("best"), 0x0008},
    {ngx_null_string, 0},
};

// define our custom function to set
static char *ngx_conf_set_my_config(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

static ngx_int_t ngx_http_test_str_handler(ngx_http_request_t *r);

static char *ngx_conf_set_test_str(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t *cscf;
    ngx_http_core_loc_conf_t *clcf;
    cscf = ngx_http_conf_get_module_srv_conf(cf, ngx_http_core_module);
    cscf->handler = ngx_http_test_str_handler;
    clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);
    clcf->handler = ngx_http_test_str_handler;
    ngx_conf_set_str_slot(cf, cmd, conf);
    return NGX_CONF_OK;
}

// Structure for all commands
static ngx_command_t ngx_http_conf_demo_commands[] = {
    {ngx_string("test_str"), // The command name
     NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
     ngx_conf_set_test_str, // The command handler
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_str),
     NULL},
    {ngx_string("test_flag"), // The command name
     NGX_HTTP_LOC_CONF | NGX_CONF_FLAG,
     ngx_conf_set_flag_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_flag),
     NULL},
    {ngx_string("test_str_array"), // The command name
     NGX_HTTP_MAIN_CONF | NGX_HTTP_SRV_CONF | NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
     ngx_conf_set_str_array_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_str_array),
     NULL},
    {ngx_string("test_keyval"), // The command name
     NGX_HTTP_LOC_CONF | NGX_CONF_TAKE2,
     ngx_conf_set_keyval_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_keyval),
     NULL},
    {ngx_string("test_num"), // The command name
     NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
     ngx_conf_set_num_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_num),
     NULL},
    {ngx_string("test_size"), // The command name
     NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
     ngx_conf_set_size_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_size),
     NULL},
    {ngx_string("test_off"), // The command name
     NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
     ngx_conf_set_off_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_off),
     NULL},
    {ngx_string("test_msec"), // The command name
     NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
     ngx_conf_set_msec_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_msec),
     NULL},
    {ngx_string("test_sec"), // The command name
     NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
     ngx_conf_set_sec_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_sec),
     NULL},
    {ngx_string("test_bufs"), // The command name
     NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
     ngx_conf_set_bufs_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_bufs),
     NULL},
    {ngx_string("test_enum"), // The command name
     NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
     ngx_conf_set_enum_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_enum_seq),
     test_enums},                // give post point
    {ngx_string("test_bitmask"), // The command name
     NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1,
     ngx_conf_set_bitmask_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_bitmask),
     test_bitmasks},
    {ngx_string("test_access"),            // The command name
     NGX_HTTP_LOC_CONF | NGX_CONF_TAKE123, // take one or two or three
     ngx_conf_set_access_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_access),
     NULL},
    {ngx_string("test_path"),               // The command name
     NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1234, // take one or two or three or four
     ngx_conf_set_path_slot,
     NGX_HTTP_LOC_CONF_OFFSET,
     offsetof(ngx_http_conf_demo_conf_t, my_path),
     NULL},
    {ngx_string("test_my_config"),
     NGX_HTTP_LOC_CONF | NGX_CONF_TAKE12, // take one or two
     ngx_conf_set_my_config,
     NGX_HTTP_LOC_CONF_OFFSET,
     0,
     NULL},
    ngx_null_command};

typedef struct
{
    ngx_str_t my_config_str;
    ngx_int_t my_config_num;
} ngx_http_conf_demo_my_config_conf_t;

static char *ngx_conf_set_my_config(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    // conf in ngx_http_conf_demo_my_config_conf_t from create_loc_conf callback
    ngx_http_conf_demo_my_config_conf_t *mycf = conf;
    // cf->args is type of ngx_array_t, all members of it are ngx_str_t, so value[1] is first paramter,value[2] is second paramter
    ngx_str_t *value = cf->args->elts;
    // nelts is nums of paramters
    if (cf->args->nelts > 1)
    {
        mycf->my_config_str = value[1];
    }
    if (cf->args->nelts > 2)
    {
        mycf->my_config_num = ngx_atoi(value[2].data, value[2].len);
        if (mycf->my_config_num == NGX_ERROR)
        {
            return "invalid number";
        }
    }
    return NGX_CONF_OK;
}

// init command config
static void *ngx_http_conf_demo_create_srv_conf(ngx_conf_t *cf)
{
    ngx_http_conf_demo_conf_t *mycf;

    mycf = (ngx_http_conf_demo_conf_t *)ngx_pcalloc(cf->pool, sizeof(ngx_http_conf_demo_conf_t));

    if (mycf == NULL)
    {
        return NULL;
    }
    mycf->my_str.len = 0;
    mycf->my_str.data = 0;
    mycf->my_flag = NGX_CONF_UNSET;
    mycf->my_num = NGX_CONF_UNSET;
    mycf->my_str_array = NGX_CONF_UNSET_PTR;
    mycf->my_keyval = NULL;
    mycf->my_off = NGX_CONF_UNSET;
    mycf->my_msec = NGX_CONF_UNSET_MSEC;
    mycf->my_sec = NGX_CONF_UNSET;
    mycf->my_size = NGX_CONF_UNSET_SIZE;

    return mycf;
}

// init command config
static void *ngx_http_conf_demo_create_loc_conf(ngx_conf_t *cf)
{
    ngx_http_conf_demo_conf_t *mycf;

    mycf = (ngx_http_conf_demo_conf_t *)ngx_pcalloc(cf->pool, sizeof(ngx_http_conf_demo_conf_t));

    if (mycf == NULL)
    {
        return NULL;
    }
    mycf->my_flag = NGX_CONF_UNSET;
    mycf->my_num = NGX_CONF_UNSET;
    mycf->my_str_array = NGX_CONF_UNSET_PTR;
    mycf->my_keyval = NULL;
    mycf->my_off = NGX_CONF_UNSET;
    mycf->my_msec = NGX_CONF_UNSET_MSEC;
    mycf->my_sec = NGX_CONF_UNSET;
    mycf->my_size = NGX_CONF_UNSET_SIZE;

    return mycf;
}

static char *ngx_http_conf_demo_merge_loc_conf(ngx_conf_t *cf, void *parent, void *child)
{
    ngx_http_conf_demo_conf_t *prev = (ngx_http_conf_demo_conf_t *)parent;
    ngx_http_conf_demo_conf_t *conf = (ngx_http_conf_demo_conf_t *)child;
    // parent config has higher priority than sub config
    ngx_conf_merge_str_value(conf->my_str, prev->my_str, "defaultstr");
    // ngx_conf_merge_str_value(prev->my_str, conf->my_str, "defaultstr");
    return NGX_CONF_OK;
}

static ngx_http_module_t ngx_http_conf_demo_module_ctx = {
    NULL,
    NULL,
    NULL, /* create main configuration */
    NULL, /* init main configuration */
    ngx_http_conf_demo_create_srv_conf, /* create server configuration */
    NULL, /* merge server configuration */
    ngx_http_conf_demo_create_loc_conf, /* create location configuration */
    ngx_http_conf_demo_merge_loc_conf}; /* merge location configuration */

ngx_module_t ngx_http_conf_demo_module = {
    NGX_MODULE_V1,
    &ngx_http_conf_demo_module_ctx,
    ngx_http_conf_demo_commands,
    NGX_HTTP_MODULE,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NGX_MODULE_V1_PADDING};

static ngx_int_t ngx_http_test_str_handler(ngx_http_request_t *r)
{
    ngx_int_t rc = 0;
    ngx_http_conf_demo_conf_t *hscf;
    ngx_http_conf_demo_conf_t *hlcf;
    ngx_str_t response_s;
    ngx_str_t response_l;
    ngx_str_t *pstr;
    ngx_chain_t out;
    ngx_buf_t *b;
    rc = ngx_http_discard_request_body(r); // drop http body
    if (NGX_OK != rc)
    {
        return rc;
    }

    r->headers_out.status = NGX_HTTP_OK;

    rc = ngx_http_send_header(r);
    if ((NGX_ERROR == rc) || (rc > NGX_OK) || (r->header_only))
    {
        return rc;
    }

    // TODO !!! here location
    hscf = ngx_http_get_module_srv_conf(r, ngx_http_conf_demo_module);
    hlcf = ngx_http_get_module_loc_conf(r, ngx_http_conf_demo_module);

    response_s = hscf->my_str;
    response_l = hlcf->my_str;

    b = ngx_create_temp_buf(r->pool, response_l.len);
    if (NULL == b)
    {
        return NGX_HTTP_INTERNAL_SERVER_ERROR;
    }

    ngx_memcpy(b->pos, response_l.data, response_l.len);
    b->last = b->pos + response_l.len;
    b->last_buf = 1;
    out.buf = b;
    out.next = NULL;
    
    //pstr = hlcf->my_str_array->elts;
    //ngx_log_error(NGX_LOG_ERR, r->connection->log, 0, "%V , %V", &pstr[0], &pstr[1]);
    
    return ngx_http_output_filter(r, &out);
}