#ifndef AVEN_BUILD_COMMON_H
#define AVEN_BUILD_COMMON_H

#include "../../aven.h"
#include "../arena.h"
#include "../arg.h"
#include "../build.h"
#include "../path.h"
#include "../str.h"

typedef struct {
    AvenStr compiler;
    AvenStr objflag;
    AvenStr outflag;
    AvenStr incflag;
    AvenStr defflag;
    AvenStrSlice flags;
} AvenBuildCommonCOpts;

typedef struct {
    AvenStr linker;
    AvenStr outflag;
    AvenStr libflag;
    AvenStr shrflag;
    AvenStrSlice flags;
} AvenBuildCommonLDOpts;

typedef struct {
    AvenStr archiver;
    AvenStr outflag;
    AvenStrSlice flags;
} AvenBuildCommonAROpts;

typedef struct {
    AvenBuildCommonCOpts cc;
    AvenBuildCommonLDOpts ld;
    AvenBuildCommonAROpts ar;
    AvenStr obext;
    AvenStr exext;
    AvenStr soext;
    AvenStr arext;
    bool clean;
} AvenBuildCommonOpts;

AvenStr aven_build_common_overview;
AvenStr aven_build_common_usage;
AvenArgSlice aven_build_common_args;

AvenBuildCommonOpts aven_build_common_opts(
    AvenArgSlice arg_slice,
    AvenArena *arena
);

AvenBuildStep aven_build_common_step_subdir(
    AvenBuildStep *dir_step,
    AvenStr subdir_name,
    AvenArena *arena
);

AvenBuildStep aven_build_common_step_cc(
    AvenBuildCommonOpts *opts,
    AvenStr src_path,
    AvenBuildStep *out_dir_step,
    AvenArena *arena
);
AvenBuildStep aven_build_common_step_cc_ex(
    AvenBuildCommonOpts *opts,
    AvenStrSlice includes,
    AvenStrSlice macros,
    AvenStr src_path,
    AvenBuildStep *out_dir_step,
    AvenArena *arena
);

AvenBuildStep aven_build_common_step_ld_exe(
    AvenBuildCommonOpts *opts,
    AvenBuildStepPtrSlice obj_steps,
    AvenBuildStep *out_dir_step,
    AvenStr out_fname,
    AvenArena *arena
);
AvenBuildStep aven_build_common_step_ld_exe_ex(
    AvenBuildCommonOpts *opts,
    AvenStrSlice linked_libs,
    AvenBuildStepPtrSlice obj_steps,
    AvenBuildStep *out_dir_step,
    AvenStr out_fname,
    AvenArena *arena
);

AvenBuildStep aven_build_common_step_ld_so(
    AvenBuildCommonOpts *opts,
    AvenBuildStepPtrSlice obj_steps,
    AvenBuildStep *out_dir_step,
    AvenStr out_fname,
    AvenArena *arena
);
AvenBuildStep aven_build_common_step_ld_so_ex(
    AvenBuildCommonOpts *opts,
    AvenStrSlice linked_libs,
    AvenBuildStepPtrSlice obj_steps,
    AvenBuildStep *out_dir_step,
    AvenStr out_fname,
    AvenArena *arena
);

AvenBuildStep aven_build_common_step_ar(
    AvenBuildCommonOpts *opts,
    AvenBuildStepPtrSlice obj_steps,
    AvenBuildStep *out_dir_step,
    AvenStr out_fname,
    AvenArena *arena
);

#if defined(AVEN_IMPLEMENTATION) or defined(AVEN_BUILD_COMMON_IMPLEMENTATION)

char aven_build_common_overview_cstr[] = "Aven C build system";
AvenStr aven_build_common_overview = {
    .ptr = aven_build_common_overview_cstr,
    .len = countof(aven_build_common_overview_cstr),
};

char aven_build_common_usage_cstr[] = "./build [options]";
AvenStr aven_build_common_usage = {
    .ptr = aven_build_common_usage_cstr,
    .len = countof(aven_build_common_usage_cstr),
};

AvenArg aven_build_common_args_data[] = {
    {
        .name = "clean",
        .description = "Remove all build artifacts",
        .type = AVEN_ARG_TYPE_BOOL,
    },
    {
        .name = "-cc",
        .description = "C compiler exe",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_CC)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_CC },
#elif defined(_WIN32)
            .data = { .arg_str = "gcc.exe" },
#else
            .data = { .arg_str = "gcc" },
#endif
        },
    },
    {
        .name = "-ld",
        .description = "Linker exe to use instead of C compiler",
        .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_LD)
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_LD },
        },
#else
        .optional = true,
#endif
    },
    {
        .name = "-ar",
        .description = "Archiver exe to create static libraries",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_AR)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_AR },
#elif defined(_WIN32)
            .data = { .arg_str = "ar.exe" },
#else
            .data = { .arg_str = "ar" },
#endif
        },
    },
    {
        .name = "-ccflags",
        .description = "C compiler common flags",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_CCFLAGS)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_CCFLAGS },
#else
            .data = { .arg_str = "" },
#endif
        },
    },
    {
        .name = "-ldflags",
        .description = "Linker common flags",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_LDFLAGS)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_LDFLAGS },
#else
            .data = { .arg_str = "" },
#endif
        },
    },
    {
        .name = "-arflags",
        .description = "Archiver common flags",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_ARFLAGS)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_ARFLAGS },
#else
            .data = { .arg_str = "-rcs" },
#endif
        },
    },
    {
        .name = "-obext",
        .description = "File extension for object files",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_OBEXT)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_OBEXT },
#else
            .data = { .arg_str = ".o" },
#endif
        },
    },
    {
        .name = "-exext",
        .description = "File extension for executables",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_EXEXT)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_EXEXT },
#elif defined(_WIN32)
            .data = { .arg_str = ".exe" },
#else
            .data = { .arg_str = "" },
#endif
        },
    },
    {
        .name = "-soext",
        .description = "File extension for shared libraries",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_SOEXT)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_SOEXT },
#elif defined(_WIN32)
            .data = { .arg_str = ".dll" },
#else
            .data = { .arg_str = ".so" },
#endif
        },
    },
    {
        .name = "-arext",
        .description = "File extension for static libraries",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_AREXT)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_AREXT },
#else
            .data = { .arg_str = ".a" },
#endif
        },
    },
    {
        .name = "-ccincflag",
        .description = "C compiler flag to add include path",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_CCINCFLAG)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_CCINCFLAGEXT },
#else
            .data = { .arg_str = "-I" },
#endif
        },
    },
    {
        .name = "-ccdefflag",
        .description = "C compiler flag to define macro",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_CCDEFFLAG)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_CCDEFFLAGEXT },
#else
            .data = { .arg_str = "-D" },
#endif
        },
    },
    {
        .name = "-ccobjflag",
        .description = "C compiler flag to compile object",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_CCOBJFLAG)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_COBJFLAGEXT },
#else
            .data = { .arg_str = "-c" },
#endif
        },
    },
    {
        .name = "-ccoutflag",
        .description = "C compiler flag to specify output file",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_CCOUTFLAG)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_COUTFLAGEXT },
#else
            .data = { .arg_str = "-o" },
#endif
        },
    },
    {
        .name = "-ldlibflag",
        .description = "Linker flag to link library",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_LDLIBFLAG)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_LDLIBFLAG },
#else
            .data = { .arg_str = "-l" },
#endif
        },
    },
    {
        .name = "-ldshrflag",
        .description = "Linker flag to emit shared library",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_LDSHRFLAG)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_LDSHRFLAG },
#else
            .data = { .arg_str = "-shared" },
#endif
        },
    },
    {
        .name = "-ldoutflag",
        .description = "Linker flag to specify output file",
        .type = AVEN_ARG_TYPE_STRING,
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_LDOUTFLAG)
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_LDOUTFLAG },
#else
            .data = { .arg_str = "-o" },
#endif
        },
    },
    {
        .name = "-aroutflag",
        .description = "Archiver flag to specify output file",
        .type = AVEN_ARG_TYPE_STRING,
#if defined(AVEN_BUILD_COMMON_DEFAULT_AROUTFLAG)
        .value = {
            .type = AVEN_ARG_TYPE_STRING,
            .data = { .arg_str = AVEN_BUILD_COMMON_DEFAULT_AROUTFLAG },
        },
#else
        .optional = true,
#endif
    },
};
AvenArgSlice aven_build_common_args = {
    .ptr = aven_build_common_args_data,
    .len = countof(aven_build_common_args_data),
};

AvenBuildCommonOpts aven_build_common_opts(
    AvenArgSlice arg_slice,
    AvenArena *arena
) {
    AvenBuildCommonOpts opts = { 0 };

    opts.clean = aven_arg_get_bool(arg_slice, "clean");
     
    opts.cc.compiler = aven_str_cstr(aven_arg_get_str(arg_slice, "-cc"));
    opts.cc.incflag = aven_str_cstr(aven_arg_get_str(arg_slice, "-ccincflag"));
    opts.cc.defflag = aven_str_cstr(aven_arg_get_str(arg_slice, "-ccdefflag"));
    opts.cc.objflag = aven_str_cstr(aven_arg_get_str(arg_slice, "-ccobjflag"));
    opts.cc.outflag = aven_str_cstr(aven_arg_get_str(arg_slice, "-ccoutflag"));
    opts.cc.flags = aven_str_split(
        aven_str_cstr(aven_arg_get_str(arg_slice, "-ccflags")),
        ' ',
        arena
    );

    if (aven_arg_has_arg(arg_slice, "-ld")) {
        opts.ld.linker = aven_str_cstr(aven_arg_get_str(arg_slice, "-ld"));
        opts.ld.outflag = aven_str_cstr(
            aven_arg_get_str(arg_slice, "-ldoutflag")
        );
    } else {
        opts.ld.linker = opts.cc.compiler;
        opts.ld.outflag = opts.cc.outflag;
    }
    opts.ld.libflag = aven_str_cstr(aven_arg_get_str(arg_slice, "-ldlibflag"));
    opts.ld.shrflag = aven_str_cstr(aven_arg_get_str(arg_slice, "-ldshrflag"));
    opts.ld.flags = aven_str_split(
        aven_str_cstr(aven_arg_get_str(arg_slice, "-ldflags")),
        ' ',
        arena
    );

    opts.ar.archiver = aven_str_cstr(aven_arg_get_str(arg_slice, "-ar"));
    if (aven_arg_has_arg(arg_slice, "-aroutflag")) {
        opts.ar.outflag = aven_str_cstr(
            aven_arg_get_str(arg_slice, "-aroutflag")
        );
    } else {
        opts.ar.outflag = aven_str("");
    }
    opts.ar.flags = aven_str_split(
        aven_str_cstr(aven_arg_get_str(arg_slice, "-arflags")),
        ' ',
        arena
    );

    opts.obext = aven_str_cstr(aven_arg_get_str(arg_slice, "-obext"));
    opts.exext = aven_str_cstr(aven_arg_get_str(arg_slice, "-exext"));
    opts.soext = aven_str_cstr(aven_arg_get_str(arg_slice, "-soext"));
    opts.arext = aven_str_cstr(aven_arg_get_str(arg_slice, "-arext"));

    return opts;
}

AvenBuildStep aven_build_common_step_subdir(
    AvenBuildStep *dir_step,
    AvenStr subdir_name,
    AvenArena *arena
) {
    assert(dir_step->out_path.valid);
    AvenStr dir_path = dir_step->out_path.value;

    AvenBuildStep subdir_step = aven_build_step_mkdir(
        aven_path(arena, dir_path.ptr, subdir_name.ptr, NULL)
    );
    aven_build_step_add_dep(&subdir_step, dir_step, arena);
    return subdir_step;
}

AvenBuildStep aven_build_common_step_cc_ex(
    AvenBuildCommonOpts *opts,
    AvenStrSlice includes,
    AvenStrSlice macros,
    AvenStr src_path,
    AvenBuildStep *out_dir_step,
    AvenArena *arena
) {
    assert(out_dir_step->out_path.valid);
    AvenStr out_dir_path = out_dir_step->out_path.value;

    AvenStr src_fname = aven_path_fname(src_path, arena);
    AvenStrSlice ext_split = aven_str_split(
        src_fname,
        '.',
        arena
    );
    AvenStr out_fname = slice_get(ext_split, 0);

    AvenStr ext = opts->obext;
    if (ext.len > 0) {
        out_fname = aven_str_concat(out_fname, ext, arena);
    }
    AvenStr target_path = aven_path(
        arena,
        out_dir_path.ptr,
        out_fname.ptr,
        NULL
    );

    AvenStrSlice cmd_slice = {
        .len = 5 + opts->cc.flags.len + 2 * includes.len + 2 * macros.len
    };
    cmd_slice.ptr = aven_arena_create_array(AvenStr, arena, cmd_slice.len);

    size_t i = 0;
    slice_get(cmd_slice, i) = opts->cc.compiler;
    i += 1;

    for (size_t j = 0; j < opts->cc.flags.len; j += 1) {
        slice_get(cmd_slice, i) = slice_get(opts->cc.flags, j);
        i += 1;
    }

    for (size_t j = 0; j < includes.len; j += 1) {
        slice_get(cmd_slice, i) = opts->cc.incflag;
        i += 1;
        slice_get(cmd_slice, i) = slice_get(includes, j);
        i += 1;
    }

    for (size_t j = 0; j < macros.len; j += 1) {
        slice_get(cmd_slice, i) = opts->cc.defflag;
        i += 1;
        slice_get(cmd_slice, i) = slice_get(macros, j);
        i += 1;
    }

    slice_get(cmd_slice, i) = opts->cc.objflag;
    i += 1;
    slice_get(cmd_slice, i) = opts->cc.outflag;
    i += 1;
    slice_get(cmd_slice, i) = target_path;
    i += 1;
    slice_get(cmd_slice, i) = src_path;
    i += 1;

    AvenBuildOptionalPath out_path = { .value = target_path, .valid = true };
    AvenBuildStep cc_step = aven_build_step_cmd(out_path, cmd_slice);
    aven_build_step_add_dep(&cc_step, out_dir_step, arena);

    return cc_step;
}

AvenBuildStep aven_build_common_step_cc(
    AvenBuildCommonOpts *opts,
    AvenStr src_path,
    AvenBuildStep *out_dir_step,
    AvenArena *arena
) {
    return aven_build_common_step_cc_ex(
        opts,
        (AvenStrSlice){ 0 },
        (AvenStrSlice){ 0 },
        src_path,
        out_dir_step,
        arena
    );
}

static AvenBuildStep aven_build_common_step_ld(
    AvenBuildCommonOpts *opts,
    AvenStrSlice linked_libs,
    AvenBuildStepPtrSlice obj_steps,
    AvenBuildStep *out_dir_step,
    AvenStr out_fname,
    bool shared_lib,
    AvenArena *arena
) {
    assert(out_dir_step->out_path.valid);
    AvenStr out_dir_path = out_dir_step->out_path.value;

    AvenStr ext = { 0 };
    if (shared_lib) {
        ext = opts->soext;
    } else {
        ext = opts->exext;
    }
    if (ext.len > 0) {
        out_fname = aven_str_concat(out_fname, ext, arena);
    }
    AvenStr target_path = aven_path(
        arena,
        out_dir_path.ptr,
        out_fname.ptr,
        NULL
    );

    AvenStrSlice cmd_slice = { 0 };
    cmd_slice.len = 3 +
        opts->ld.flags.len +
        obj_steps.len +
        2 * linked_libs.len;
    if (shared_lib) {
        cmd_slice.len += 1;
    }
    cmd_slice.ptr = aven_arena_create_array(AvenStr, arena, cmd_slice.len);

    size_t i = 0;
    slice_get(cmd_slice, i) = opts->ld.linker;
    i += 1;

    for (size_t j = 0; j < opts->ld.flags.len; j += 1) {
        slice_get(cmd_slice, i) = slice_get(opts->ld.flags, j);
        i += 1;
    }

    if (shared_lib) {
        slice_get(cmd_slice, i) = opts->ld.shrflag;
        i += 1;
    }

    for (size_t j = 0; j < linked_libs.len; j += 1) {
        slice_get(cmd_slice, i) = opts->ld.libflag;
        i += 1;
        slice_get(cmd_slice, i) = slice_get(linked_libs, j);
        i += 1;
    }

    slice_get(cmd_slice, i) = opts->ld.outflag;
    i += 1;
    slice_get(cmd_slice, i) = target_path;
    i += 1;

    for (size_t j = 0; j < obj_steps.len; j += 1) {
        AvenBuildStep *obj_step = slice_get(obj_steps, j);
        assert(obj_step->out_path.valid);
        slice_get(cmd_slice, i) = obj_step->out_path.value;
        i += 1;
    }

    AvenBuildOptionalPath out_path = { .value = target_path, .valid = true };
    AvenBuildStep link_step = aven_build_step_cmd(
        out_path,
        cmd_slice
    );

    for (size_t j = 0; j < obj_steps.len; j += 1) {
        aven_build_step_add_dep(&link_step, slice_get(obj_steps, j), arena);
    }
    aven_build_step_add_dep(&link_step, out_dir_step, arena);

    return link_step;
}

AvenBuildStep aven_build_common_step_ld_exe_ex(
    AvenBuildCommonOpts *opts,
    AvenStrSlice linked_libs,
    AvenBuildStepPtrSlice obj_steps,
    AvenBuildStep *out_dir_step,
    AvenStr out_fname,
    AvenArena *arena
) {
    return aven_build_common_step_ld(
        opts,
        linked_libs,
        obj_steps,
        out_dir_step,
        out_fname,
        false,
        arena
    );
}

AvenBuildStep aven_build_common_step_ld_so_ex(
    AvenBuildCommonOpts *opts,
    AvenStrSlice linked_libs,
    AvenBuildStepPtrSlice obj_steps,
    AvenBuildStep *out_dir_step,
    AvenStr out_fname,
    AvenArena *arena
) {
    return aven_build_common_step_ld(
        opts,
        linked_libs,
        obj_steps,
        out_dir_step,
        out_fname,
        true,
        arena
    );
}

AvenBuildStep aven_build_common_step_ld_exe(
    AvenBuildCommonOpts *opts,
    AvenBuildStepPtrSlice obj_steps,
    AvenBuildStep *out_dir_step,
    AvenStr out_fname,
    AvenArena *arena
) {
    return aven_build_common_step_ld_exe_ex(
        opts,
        (AvenStrSlice){ 0 },
        obj_steps,
        out_dir_step,
        out_fname,
        arena
    );
}

AvenBuildStep aven_build_common_step_ld_so(
    AvenBuildCommonOpts *opts,
    AvenBuildStepPtrSlice obj_steps,
    AvenBuildStep *out_dir_step,
    AvenStr out_fname,
    AvenArena *arena
) {
    return aven_build_common_step_ld_so_ex(
        opts,
        (AvenStrSlice){ 0 },
        obj_steps,
        out_dir_step,
        out_fname,
        arena
    );
}

AvenBuildStep aven_build_common_step_ar(
    AvenBuildCommonOpts *opts,
    AvenBuildStepPtrSlice obj_steps,
    AvenBuildStep *out_dir_step,
    AvenStr out_fname,
    AvenArena *arena
) {
    assert(out_dir_step->out_path.valid);
    AvenStr out_dir_path = out_dir_step->out_path.value;

    AvenStr ext = opts->arext;
    if (ext.len > 0) {
        out_fname = aven_str_concat(out_fname, ext, arena);
    }
    AvenStr target_path = aven_path(
        arena,
        out_dir_path.ptr,
        out_fname.ptr,
        NULL
    );

    AvenStrSlice cmd_slice = { 0 };
    cmd_slice.len = 2 + opts->ar.flags.len + obj_steps.len;
    if (opts->ar.outflag.len != 0) {
        cmd_slice.len += 1;
    }
    cmd_slice.ptr = aven_arena_create_array(AvenStr, arena, cmd_slice.len);

    size_t i = 0;
    slice_get(cmd_slice, i) = opts->ar.archiver;
    i += 1;

    for (size_t j = 0; j < opts->ar.flags.len; j += 1) {
        slice_get(cmd_slice, i) = slice_get(opts->ar.flags, j);
        i += 1;
    }

    if (opts->ar.outflag.len != 0) {
        slice_get(cmd_slice, i) = opts->ar.outflag;
        i += 1;
    }

    slice_get(cmd_slice, i) = target_path;
    i += 1;

    for (size_t j = 0; j < obj_steps.len; j += 1) {
        AvenBuildStep *obj_step = slice_get(obj_steps, j);
        assert(obj_step->out_path.valid);
        slice_get(cmd_slice, i) = obj_step->out_path.value;
        i += 1;
    }

    AvenBuildOptionalPath out_path = { .value = target_path, .valid = true };
    AvenBuildStep ar_step = aven_build_step_cmd(
        out_path,
        cmd_slice
    );

    for (size_t j = 0; j < obj_steps.len; j += 1) {
        aven_build_step_add_dep(&ar_step, slice_get(obj_steps, j), arena);
    }
    aven_build_step_add_dep(&ar_step, out_dir_step, arena);

    return ar_step;
}

#endif // AVEN_BUILD_COMMON_IMPLEMENTATION

#endif // AVEN_BUILD_COMMON_H
