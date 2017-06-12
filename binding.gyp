{
  'includes': [ 'common.gypi' ],
  'variables': {
      'error_on_warnings%':'true',
      # includes we don't want warnings for.
      # As a variable to make easy to pass to
      # cflags (linux) and xcode (mac)
      'system_includes': [
        "-isystem <(module_root_dir)/<!(node -e \"require('nan')\")",
        "-isystem <(module_root_dir)/mason_packages/.link/include/"
      ]
  },
  'targets': [
    {
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
      'target_name': '<(module_name)',
      'product_dir': '<(module_path)',
      'dependencies': [ 'action_before_build' ],
      'sources': [ './src/hello_world.cpp' ],
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