# Developer

We use [this](https://github.com/mapbox/node-cpp-skel/blob/master/scripts/setup.sh#L7) current [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) version to consistently format the code base. The format is automatically checked via a Travis CI build as well. Run the following script locally to ensure formatting is ready to merge:
```
make format
```

This `format` command is set from within [the Makefile](./Makefile).