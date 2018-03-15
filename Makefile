# This Makefile serves a few purposes:
#
# 1. It provides an interface to iterate quickly while developing the C++ code in src/
# by typing `make` or `make debug`. To make iteration as fast as possible it calls out
# directly to underlying build tools and skips running steps that appear to have already
# been run (determined by the presence of a known file or directory). What `make` does is
# the same as running `npm install --build-from-source` except that it is faster when
# run a second time and may break if your build deps failed to install but their directories
# do exist. If you hit a build error do to this, you should be able to fix it by running
# `make distclean` and then `make` again.
#
# 2. It provides a few commands that call out to external scripts like `make coverage` or
# `make tidy`. These scripts can be called directly but this Makefile provides a more uniform
# interface to call them.
#
# To learn more about the build system see https://github.com/mapbox/node-cpp-skel/blob/master/docs/extended-tour.md#builds

MODULE_NAME := $(shell node -e "console.log(require('./package.json').binary.module_name)")

# Whether to turn compiler warnings into errors
export WERROR ?= true

default: release

# install deps but for now ignore our own install script
# so that we can run it directly in either debug or release
node_modules/nan:
	npm install --ignore-scripts

mason_packages/headers: node_modules/nan
	node_modules/.bin/mason-js install

mason_packages/.link/include: mason_packages/headers
	node_modules/.bin/mason-js link

build-deps: mason_packages/.link/include

release: build-deps
	V=1 ./node_modules/.bin/node-pre-gyp configure build --error_on_warnings=$(WERROR) --loglevel=error
	@echo "run 'make clean' for full rebuild"

debug: mason_packages/.link/include
	V=1 ./node_modules/.bin/node-pre-gyp configure build --error_on_warnings=$(WERROR) --loglevel=error --debug
	@echo "run 'make clean' for full rebuild"

coverage:
	./scripts/coverage.sh

tidy:
	./scripts/clang-tidy.sh

format:
	./scripts/clang-format.sh

clean:
	rm -rf lib/binding
	rm -rf build
	# remove remains from running 'make coverage'
	rm -f *.profraw
	rm -f *.profdata
	@echo "run 'make distclean' to also clear node_modules, mason_packages, and .mason directories"

distclean: clean
	rm -rf node_modules
	rm -rf mason_packages
	# remove remains from running './scripts/setup.sh'
	rm -rf .mason
	rm -rf .toolchain
	rm -f local.env

xcode: node_modules
	./node_modules/.bin/node-pre-gyp configure -- -f xcode

	@# If you need more targets, e.g. to run other npm scripts, duplicate the last line and change NPM_ARGUMENT
	SCHEME_NAME="$(MODULE_NAME)" SCHEME_TYPE=library BLUEPRINT_NAME=$(MODULE_NAME) BUILDABLE_NAME=$(MODULE_NAME).node scripts/create_scheme.sh
	SCHEME_NAME="npm test" SCHEME_TYPE=node BLUEPRINT_NAME=$(MODULE_NAME) BUILDABLE_NAME=$(MODULE_NAME).node NODE_ARGUMENT="`npm bin tape`/tape test/*.test.js" scripts/create_scheme.sh

	open build/binding.xcodeproj

docs:
	npm run docs

test:
	npm test

.PHONY: test docs
