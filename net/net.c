// +build ignore

#include "vpi_user.h"
#include "libnet.h"
#include <stdlib.h>
#include <string.h>

static vpiHandle input_signal_handle = NULL;
static vpiHandle output_signal_handle = NULL;
static int server_status = 0;
static int input_width = 0;
static int input_size = 0;
static int output_width = 0;
static int output_size = 0;
static struct t_vpi_value input_value;
static struct t_vpi_value output_value;
static unsigned char* output_bytes = NULL;
static char input_sig_name[256];
static char output_sig_name[256];

void init_server() {
    StartServer("","8009");
    input_signal_handle = vpi_handle_by_name(input_sig_name, NULL);
    output_signal_handle = vpi_handle_by_name(output_sig_name, NULL);
    input_width = vpi_get(vpiSize, input_signal_handle);
    input_size = (input_width + 31) / 32;
    input_value.format = vpiVectorVal;
    input_value.value.vector = malloc(sizeof(s_vpi_value) * input_size);
    vpi_printf("input width: %d\n", input_width);

    output_width = vpi_get(vpiSize, output_signal_handle);
    output_size = (output_width + 31) / 32;
    output_bytes = malloc(sizeof(unsigned char) * (output_width * 2));
    vpi_printf("output width: %d\n", output_width);

    server_status = 1;
}

void input_signal() {
    if (input_signal_handle) {
        PLI_INT32* input_bytes = (PLI_INT32*)GetInput();
        // int pos = 0;
        for (int i = 0; i < input_size; i++) {
            // vpi_printf("input_bytes[%d]: %08x\n", i, input_bytes[i]);
            // vpi_printf("input_bytes1[%d]: %08x\n", i, input_bytes[i+input_size]);
            input_value.value.vector[i].aval = input_bytes[i];
            input_value.value.vector[i].bval = input_bytes[i+input_size];
        }
        // for (int i = 0; i < input_width; i++) {
        //     char cur_byte = input_bytes[i/8];
        //     if ((((PLI_INT32)cur_byte) << ((i/8)*8)) & (1 << pos)) {
        //         input_value.value.vector[i/32].aval |= (1 << (i % 32));
        //     }
        //     if ((((PLI_INT32)cur_byte) << ((i/8)*8)) & (1 << (pos + 1))) {
        //         input_value.value.vector[i/32].bval |= (1 << (i % 32));
        //     }
        //     pos += 2;
        // }
        vpi_put_value(input_signal_handle, &input_value, NULL, vpiNoDelay);
    } else {
        vpi_printf("input signal handle not found\n");
    }
}

void output_signal() {
    if (output_signal_handle) {
        output_value.format = vpiVectorVal;
        vpi_get_value(output_signal_handle, &output_value);
        struct t_vpi_vecval * vec = output_value.value.vector;
        // int pos = 0;
        // for (int i = 0; i < output_size; i++) {
        //     vpi_printf("vec[%d].aval: %02x\n", i, vec[i].aval);
        //     vpi_printf("vec[%d].bval: %02x\n", i, vec[i].bval);
        // }
        memset(output_bytes, 0, output_width * 2);
        PLI_INT32* outBufs = (PLI_INT32*) output_bytes; 
        for(int i = 0; i < output_size; i++) {
            outBufs[i] = vec[i].aval;
            outBufs[i+output_size] = vec[i].bval;
        }
        SetOutput(output_bytes, output_width*2);
    } else {
        vpi_printf("output signal handle not found\n");
    }
}

static int refresh_compiletf(char*user_data) {
    return 0;
}

static int refresh_calltf(char*user_data) {
    // vpi_printf("refresh called\n");
    if(!server_status) {
        init_server();
    }
    input_signal();
    output_signal();
    return 0;
}

void refresh_register() {
    s_vpi_systf_data tf_data;

    tf_data.type      = vpiSysTask;
    tf_data.tfname    = "$refresh";
    tf_data.calltf    = refresh_calltf;
    tf_data.compiletf = refresh_compiletf;
    tf_data.sizetf    = 0;
    tf_data.user_data = 0;
    vpi_register_systf(&tf_data);
}

PLI_INT32 set_sig_names_calltf(PLI_BYTE8 *user_data) {
    vpiHandle systf_handle, args_iter, arg_handle;
    systf_handle = vpi_handle(vpiSysTfCall, NULL);

    args_iter = vpi_iterate(vpiArgument, systf_handle);

    // first parameter
    arg_handle = vpi_scan(args_iter);
    s_vpi_value value;
    value.format = vpiStringVal;
    vpi_get_value(arg_handle, &value);
    strcpy(input_sig_name, value.value.str);
    vpi_printf("First parameter: %s\n", value.value.str);

    // second parameter
    arg_handle = vpi_scan(args_iter);
    value.format = vpiStringVal;
    vpi_get_value(arg_handle, &value);
    strcpy(output_sig_name, value.value.str);
    vpi_printf("Second parameter: %s\n", value.value.str);

    return 0;
}


void register_set_sig_names(void) {
    s_vpi_systf_data tf_data;
    tf_data.type = vpiSysTask;           
    tf_data.tfname = "$setSignalNames";     
    tf_data.calltf = set_sig_names_calltf;
    tf_data.compiletf = NULL;      
    tf_data.sizetf = NULL; 
    tf_data.user_data = NULL;
    
    vpi_register_systf(&tf_data);
}



void (*vlog_startup_routines[])() = {
    refresh_register,
    register_set_sig_names,
    0
};