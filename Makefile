MODULE_NAME := $(shell node -e "console.log(require('./package.json').name)") 

default: node_modules
	./node_modules/.bin/node-pre-gyp build --loglevel=error

debug:
	npm install --build-from-source=$(MODULE_NAME) --verbose

clean:
	rm -rf node_modules
	rm -rf lib/binding

node_modules:
	npm install --build-from-source=$(MODULE_NAME)

docs:
	npm run docs

test:
	npm test

.PHONY: test docs