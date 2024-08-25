# libfunmath: a pointless contrived libaven example

Testing out file watching and hot reloading with [libaven][1].

To build and watch:

```shell
cc -o build build.c
./build watch
```

To run:

```
./build_out/bin/print_funmath
```

Now try updating the message in `src/libhot/hot.c` and watch it recompile and
print automatically!

[1]: https://github.com/permutationlock/libaven
