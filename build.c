#define _POSIX_C_SOURCE 200809L

#include <stdlib.h>

#define AVEN_IMPLEMENTATION

#include "build.h"

#include "libaven/include/aven.h"
#include "libaven/include/aven/arena.h"
#include "libaven/include/aven/build.h"
#include "libaven/include/aven/build/common.h"
#include "libaven/include/aven/str.h"
#include "libaven/include/aven/watch.h"

AvenArg custom_args[] = {
    {
        .name = "watch",
        .description = "Automatically re-build on changes to files in src/",
        .type = AVEN_ARG_TYPE_BOOL,
    },
};
 
#define ARENA_SIZE (4096 * 2000)

int main(int argc, char **argv) {
    void *mem = malloc(ARENA_SIZE);
    if (mem == NULL) {
        fprintf(stderr, "malloc failure\n");
    }

    AvenArena arena = aven_arena_init(mem, ARENA_SIZE);

    AvenArgSlice args = {
        .len = countof(custom_args) + aven_build_common_args.len,
    };
    args.ptr = aven_arena_create_array(
        AvenArg,
        &arena,
        countof(custom_args) + aven_build_common_args.len
    );
    memcpy(args.ptr, custom_args, sizeof(custom_args));
    memcpy(
        args.ptr + countof(custom_args),
        aven_build_common_args.ptr,
        aven_build_common_args.len * sizeof(AvenArg)
    );

    int error = aven_arg_parse(
        args,
        argv,
        argc,
        aven_build_common_overview.ptr,
        aven_build_common_usage.ptr
    );
    if (error != 0) {
        return error;
    }

    AvenBuildCommonOpts opts = aven_build_common_opts(args, &arena);
    bool watch = aven_arg_get_bool(args, "watch");

    AvenBuildStep work_dir_step = aven_build_step_mkdir(aven_str("build_work"));
    AvenBuildStep out_dir_step = aven_build_step_mkdir(aven_str("build_out"));
    AvenBuildStep bin_dir_step = aven_build_common_step_subdir(
        &out_dir_step,
        aven_str("bin"),
        &arena
    );

    AvenBuildStep root_step = funmath_build_exe_step(
        &opts,
        aven_str("."),
        &work_dir_step,
        &bin_dir_step,
        &arena
    );

    if (opts.clean) {
        aven_build_step_clean(&root_step);
    } else if (watch) {
        error = aven_build_step_run(&root_step, arena);
        if (error != 0) {
            fprintf(stderr, "BUILD ERROR: %d\n", error);
            return 1;
        }

        AvenStr src_dir = aven_path(&arena, "src", "libhot", NULL);
        AvenWatchHandle src_handle = aven_watch_init(src_dir);
        if (src_handle == AVEN_WATCH_HANDLE_INVALID) {
            fprintf(stderr, "couldn't watch %s\n", src_dir.ptr);
            return 1;
        }

        AvenBuildStep libhot_step = funmath_build_libhot_step(
            &opts,
            aven_str("."),
            &work_dir_step,
            &bin_dir_step,
            &arena
        );
        for (;;) {
            aven_build_step_reset(&libhot_step);
            AvenWatchResult result = aven_watch_check(src_handle, -1);
            if (result.error != 0) {
                fprintf(stderr, "directory watching failed\n");
                return 1;
            }
            while (result.payload) {
                result = aven_watch_check(src_handle, 100);
                if (result.error != 0) {
                    fprintf(stderr, "directory watching failed\n");
                    return 1;
                }
            };
            printf("\nRE-BUILDING: libhot\n");
            error = aven_build_step_run(&libhot_step, arena);
            if (error != 0) {
                fprintf(stderr, "BUILD ERROR: %d\n", error);
            }
        }
        aven_watch_deinit(src_handle);
    } else {
        error = aven_build_step_run(&root_step, arena);
        if (error != 0) {
            fprintf(stderr, "BUILD ERROR: %d\n", error);
        }
    }

    free(mem);

    return error;
}

