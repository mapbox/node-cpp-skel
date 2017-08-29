# Developer

We use [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) version [4.0.0](https://github.com/mapbox/mason/tree/master/scripts/clang-format) to consistently format the code base. The format is automatically checked via a Travis CI build as well. Run the following locally to ensure formatting is ready to merge:
```
make format
```

This `format` command is set from within [the Makefile](./Makefile).