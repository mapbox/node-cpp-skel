# This file inherits default targets for Node addons, see https://github.com/nodejs/node-gyp/blob/master/addon.gypi
{
  'includes': [ 'common.gypi' ], # brings in a default set of options that are inherited from gyp
  'variables': { # custom variables we use specific to this file
      'error_on_warnings%':'true', # can be overriden by a command line variable because of the % sign using "WERROR" (defined in Makefile)
      # Use this variable to silence warnings from mason dependencies and from NAN
      # It's a variable to make easy to pass to
      # cflags (linux) and xcode (mac)
      'system_includes': [
        "-isystem <(module_root_dir)/<!(node -e \"require('nan')\")",
        "-isystem <(module_root_dir)/mason_packages/.link/include/"
      ]
  },
  # `targets` is a list of targets for gyp to run.
  # Different types of targets:
  # - [executable](https://github.com/mapbox/cpp/blob/master/glossary.md#executable)
  # - [loadable_module](https://github.com/mapbox/cpp/blob/master/glossary.md#loadable-module)
  # - [static library](https://github.com/mapbox/cpp/blob/master/glossary.md#static-library)
  # - [shared library](https://github.com/mapbox/cpp/blob/master/glossary.md#shared-library)
  # - none: a trick to tell gyp not to run the compiler for a given target.
  'targets': [
    {
      # This target: 
      # - doesnt build any code (why it's type "none", to tell gyp not to run the compiler)
      # - runs a script to install mason packages
      'target_name': 'action_before_build',
      'type': 'none',
      'hard_dependency': 1,
      'actions': [
        {
          'action_name': 'install_deps',
          'inputs': ['./scripts/install_deps.sh'],
          'outputs': ['./mason_packages'],
          'action': ['./scripts/install_deps.sh']
        }
      ]
    },
    {
      # module_name and module_path are both variables passed by node-pre-gyp from package.json
      'target_name': '<(module_name)', # sets the name of the binary file
      'product_dir': '<(module_path)', # controls where the node binary file gets copied to (./lib/binding/module.node)
      'type': 'loadable_module',
      'dependencies': [ 'action_before_build' ],
      # "make" only watches files specified here, and will sometimes cache these files after the first compile.
      # This cache can sometimes cause confusing errors when removing/renaming/adding new files. 
      # Running "make clean" helps to prevent this "mysterious error by cache" scenario
      # This also is where the benefits of using a "glob" come into play...
      # See: https://github.com/mapbox/node-cpp-skel/pull/44#discussion_r122050205
      'sources': [ 
        './src/module.cpp',
        './src/standalone/hello.cpp',
        './src/standalone_async/hello_async.cpp',
        './src/object_sync/hello.cpp',
        './src/object_async/hello_async.cpp'
      ],
      'include_dirs': [
        '<!(node -e \'require("nan")\')'
      ],
      'ldflags': [
        '-Wl,-z,now',
      ],
      'conditions': [
        ['error_on_warnings == "true"', {
            'cflags_cc' : [ '-Werror' ],
            'xcode_settings': {
              'OTHER_CPLUSPLUSFLAGS': [ '-Werror' ]
            }
        }]
      ],
      'cflags': [
          '<@(system_includes)'
      ],
      'xcode_settings': {
        'OTHER_LDFLAGS':[
          '-Wl,-bind_at_load'
        ],
        'OTHER_CPLUSPLUSFLAGS': [
            '<@(system_includes)'
        ],
        'GCC_ENABLE_CPP_RTTI': 'YES',
        'GCC_ENABLE_CPP_EXCEPTIONS': 'YES',
        'MACOSX_DEPLOYMENT_TARGET':'10.8',
        'CLANG_CXX_LIBRARY': 'libc++',
        'CLANG_CXX_LANGUAGE_STANDARD':'c++11',
        'GCC_VERSION': 'com.apple.compilers.llvm.clang.1_0'
      }
    }
  ]
}