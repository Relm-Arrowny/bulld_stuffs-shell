#include "helper.h"

int main(int argc, char *argv[]) {
    const char *path_env = "/usr/local/sbin:/usr/local/bin:/usr/sbin:/usr/bin:/sbin:/bin";
    const char *com = "n";
    
    check_typeDefaultPath(com);

    //free_string_list(path_list);
    return 0;
}