#ifndef FUNMATH_BUILD_H
#define FUNMATH_BUILD_H

#include "libaven/build.h"

#include "libaven/include/aven.h"
#include "libaven/include/aven/arena.h"
#include "libaven/include/aven/arg.h"
#include "libaven/include/aven/build.h"
#include "libaven/include/aven/build/common.h"
#include "libaven/include/aven/path.h"
#include "libaven/include/aven/str.h"

static AvenBuildStep funmath_build_lib_step(
    AvenBuildCommonOpts *opts,
    AvenStr root_path,
    AvenBuildStep *work_dir_step,
    AvenBuildStep *out_dir_step,
    AvenArena *arena
) {
    AvenBuildStep *collatz_step = aven_arena_create(AvenBuildStep, arena);
    *collatz_step = aven_build_common_step_cc(
        opts,
        aven_path(arena, root_path.ptr, "src", "collatz.c", NULL),
        work_dir_step,
        arena
    );

    AvenBuildStep *fibonacci_step = aven_arena_create(AvenBuildStep, arena);
    *fibonacci_step = aven_build_common_step_cc(
        opts,
        aven_path(arena, root_path.ptr, "src", "fibonacci.c", NULL),
        work_dir_step,
        arena
    );

    AvenBuildStep *dep_obj_data[] = { collatz_step, fibonacci_step };
    AvenBuildStepPtrSlice dep_objs = {
        .ptr = dep_obj_data,
        .len = countof(dep_obj_data),
    };
    return aven_build_common_step_ar(
        opts,
        dep_objs,
        out_dir_step,
        aven_str("libfunmath"),
        arena
    );
}

static AvenBuildStep funmath_build_libhot_step(
    AvenBuildCommonOpts *opts,
    AvenStr root_path,
    AvenBuildStep *work_dir_step,
    AvenBuildStep *out_dir_step,
    AvenArena *arena
) {
    AvenBuildStep *res_dir_step = aven_arena_create(AvenBuildStep, arena);
    *res_dir_step = aven_build_common_step_subdir(
        out_dir_step,
        aven_str("res"),
        arena
    );

    AvenBuildStep *lock_dir_step = aven_arena_create(AvenBuildStep, arena);
    *lock_dir_step = aven_build_common_step_subdir(
        out_dir_step,
        aven_str("reslock"),
        arena
    );

    AvenBuildStep *hot_step = aven_arena_create(AvenBuildStep, arena);
    *hot_step = aven_build_common_step_cc(
        opts,
        aven_path(arena, root_path.ptr, "src", "libhot", "hot.c", NULL),
        work_dir_step,
        arena
    );

    AvenBuildStep *libhot_obj_data[] = { hot_step };
    AvenBuildStepPtrSlice libhot_objs = {
        .ptr = libhot_obj_data,
        .len = countof(libhot_obj_data),
    };
    AvenBuildStep *libhot_step = aven_arena_create(AvenBuildStep, arena);
    *libhot_step = aven_build_common_step_ld_so(
        opts,
        libhot_objs,
        res_dir_step,
        aven_str("libhot"),
        arena
    );

    AvenBuildStep touch_lock_step = aven_build_step_touch(
        aven_path(arena, lock_dir_step->out_path.value.ptr, "lock", NULL)
    );
    aven_build_step_add_dep(&touch_lock_step, lock_dir_step, arena);
    aven_build_step_add_dep(&touch_lock_step, libhot_step, arena);

    return touch_lock_step;
}

static AvenBuildStep funmath_build_exe_step(
    AvenBuildCommonOpts *opts,
    AvenStr root_path,
    AvenBuildStep *work_dir_step,
    AvenBuildStep *out_dir_step,
    AvenArena *arena
) { 
    AvenBuildStep *libfunmath_step = aven_arena_create(AvenBuildStep, arena);
    *libfunmath_step = funmath_build_lib_step(
        opts,
        root_path,
        work_dir_step,
        work_dir_step,
        arena
    );

    AvenStr include_paths[] = {
        libaven_build_include_path(
            aven_path(arena, root_path.ptr, "libaven", NULL),
            arena
        ),
    };
    AvenStrSlice includes = {
        .ptr = include_paths,
        .len = countof(include_paths),
    };
    AvenStrSlice macros = { 0 };

    AvenBuildStep *main_step = aven_arena_create(AvenBuildStep, arena);
    *main_step = aven_build_common_step_cc_ex(
        opts,
        includes,
        macros,
        aven_path(arena, root_path.ptr, "src", "main.c", NULL),
        work_dir_step,
        arena
    );

    AvenBuildStep *libaven_work_dir_step = aven_arena_create(
        AvenBuildStep,
        arena
    );
    *libaven_work_dir_step = aven_build_common_step_subdir(
        work_dir_step,
        aven_str("libaven_work"),
        arena
    );

    AvenBuildStep *libaven_step = aven_arena_create(AvenBuildStep, arena);
    *libaven_step = libaven_build_step(
        opts,
        aven_str("libaven"),
        libaven_work_dir_step,
        work_dir_step,
        arena
    );

    AvenBuildStep *libfunmath_work_dir_step = aven_arena_create(
        AvenBuildStep,
        arena
    );
    *libfunmath_work_dir_step = aven_build_common_step_subdir(
        work_dir_step,
        aven_str("libfunmath_work"),
        arena
    );
    AvenBuildStep *libhot_step = aven_arena_create(AvenBuildStep, arena);
    *libhot_step = funmath_build_libhot_step(
        opts,
        root_path,
        work_dir_step,
        out_dir_step,
        arena
    );

    AvenBuildStep *exe_obj_data[] = {
        main_step,
        libfunmath_step,
        libaven_step,
    };
    AvenBuildStepPtrSlice exe_objs = {
        .ptr = exe_obj_data,
        .len = countof(exe_obj_data),
    };
    AvenBuildStep exe_step = aven_build_common_step_ld_exe(
        opts,
        exe_objs,
        out_dir_step,
        aven_str("print_funmath"),
        arena
    );
    aven_build_step_add_dep(&exe_step, libhot_step, arena);

    return exe_step;
}

#endif // FUNMATH_BUILD_H
