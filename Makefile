PACKAGE_NAME := $(shell node -e "console.log(require('./package.json').name)")
MODULE_NAME := $(shell node -e "console.log(require('./package.json').binary.module_name)")

default: node_modules
	./node_modules/.bin/node-pre-gyp configure build --loglevel=error

debug:
	npm install --build-from-source=$(PACKAGE_NAME) --debug

coverage:
	./scripts/coverage.sh

clean:
	rm -rf lib/binding
	rm -rf build

distclean: clean
	rm -rf node_modules
	rm -rf .mason

node_modules:
	npm install --build-from-source=$(PACKAGE_NAME)

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