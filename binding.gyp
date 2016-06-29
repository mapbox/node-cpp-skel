{
  'includes': [ 'common.gypi' ],
  'targets': [
    {
      'target_name': '<(module_name)',
      'product_dir': '<(module_path)',
      'sources': [ './src/hello_world.cpp' ],
      'include_dirs': [
        '<!(node -e \'require("nan")\')'
      ],
      'ldflags': [
        '-Wl,-z,now',
      ],
      'xcode_settings': {
        'OTHER_LDFLAGS':[
          '-Wl,-bind_at_load'
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