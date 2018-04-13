#include <ckb-next/animation.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void ckb_info(){
    // Plugin info
    CKB_NAME("Pipe");
    CKB_VERSION("0.1");
    CKB_COPYRIGHT("2015-2018", "Tasos Sahanidis");
    CKB_LICENSE("GPLv2");
    CKB_GUID("{54AD2975-E192-457D-BCDC-D912A24F33B5}");
    CKB_DESCRIPTION("Control lights using named pipes\nplaced in /tmp/.\nData format is RRGGBBAA.\nWARNING: It doesn't clean up the nodes.");

    // Effect parameters
    CKB_PARAM_LONG("fifonum", "/tmp/ckbpipe", "", 1, 1, 200);

    // Timing/input parameters
    CKB_KPMODE(CKB_KP_POSITION);
    CKB_TIMEMODE(CKB_TIME_DURATION);
    CKB_LIVEPARAMS(FALSE);
    CKB_PREEMPT(TRUE);
    CKB_REPEAT(FALSE);

    // Presets
    CKB_PRESET_START("Pipe");
    CKB_PRESET_END;
}

#define FIFO_BUFFER 20
long fifonum = 0;
int fd = -1;
unsigned int a = 0, r = 0, g = 0, b = 0;
#define FIFO_NAME_LEN 16
char fifoname[FIFO_NAME_LEN] = {0}; // /tmp/ckbpipeNNN
char buffer[FIFO_BUFFER] = {0};

void ckb_init(ckb_runctx* context){
}

void ckb_parameter(ckb_runctx* context, const char* name, const char* value){
    CKB_PARSE_LONG("fifonum", &fifonum){}
    unsigned char fifonumchar = fifonum;
    // Hacky, but the only way without executing it on every frame
    if(fifonum)
    {
        sprintf(fifoname, "/tmp/ckbpipe%03u", fifonumchar);
        // Create named pipe in tmp
        mkfifo(fifoname, 0600);
        fd = open(fifoname, O_RDONLY | O_NONBLOCK);
    }

}

void ckb_keypress(ckb_runctx* context, ckb_key* key, int x, int y, int state){
}

void ckb_start(ckb_runctx* context, int state){
}

void ckb_time(ckb_runctx* context, double delta){
}

int ckb_frame(ckb_runctx* context){
    if(read(fd, buffer, FIFO_BUFFER) == -1)
        return 1;
    if(sscanf(buffer, "%2x%2x%2x%2x", &r, &g, &b, &a) != 4)
        return 0;
    unsigned count = context->keycount;
    for(unsigned i = 0; i < count; i++){
        ckb_key* key = context->keys + i;
        key->a = a;
        key->r = r;
        key->g = g;
        key->b = b;
    }
    return 0;
}
