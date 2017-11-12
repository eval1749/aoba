# This file is used to manage the dependencies of the Evita src repo. It is
# used by gclient to determine what version of each dependency to check out, and
# where.

vars = {
  'chromium_git': 'https://chromium.googlesource.com',

  'base_revision': 'ce09bb159440ea591335ce0a9477c1b5de840b02',
  'build_revision': '98bbbff4731a0b844cb0e77a7afdb955329826b3',
  'buildtools_revision': '93a751e41bd93e373548759c6c5453bd95b6f35e',
  'ced_revision': '94c367a1fe3a13207f4b22604fcfd1d9f9ddf6d9',
  'depot_tools_revision': '1ea21322e369836d6989d54ed645bb69cdc3acef',
  'googletest_revision': '7f8fefabedf2965980585be8c2bff97458f28e0b',
  'gyp_revision': 'd61a9397e668fa9843c4aa7da9e79460fe590bfb',
  'icu_revision': '5ed26985c0171755d7542e7ad6bce12cde4dccfb',
  'modp_b64_revision': '28e3fbba4cb4ec3ffd85b53d0a3904525d08f5a6',
  'testing_revision': '7220d9f76ff592ca7c738b50d7214e339bd4e094',
  'tools_clang_revision': '4b585122606d6daecc97b89b69f0f3454afb170a',
  'tools_win_revision': '66f69221de06b65a6ef3e9d6f01a2dc3bfb8bf03',
  'zlib_revision': '69ad9a4ce2f1cc917e51fa5db740f305bfd3d394',
}

# base requires
#  //third_party/ced
#  //third_party/icu
#  //third_party/libxml
#  //third_party/modp_b64
#  //third_party/zlib

deps = {
  # From chromium_git
  'src/base':
    Var('chromium_git') + '/chromium/src/base' + '@' +  Var('base_revision'),

  'src/build':
    Var('chromium_git') + '/chromium/src/build' + '@' +  Var('build_revision'),

  'src/buildtools':
    Var('chromium_git') + '/chromium/buildtools.git' + '@' +  Var('buildtools_revision'),

  'src/testing':
    Var('chromium_git') + '/chromium/src/testing' + '@' + Var('testing_revision'),

  'src/third_party/ced/src':
    Var('chromium_git') + '/external/github.com/google/compact_enc_det.git' + '@' + Var('ced_revision'),

  'src/third_party/depot_tools':
    Var('chromium_git') + '/chromium/tools/depot_tools.git' + '@' + Var('depot_tools_revision'),

 'src/third_party/googletest/src':
    Var('chromium_git') + '/external/github.com/google/googletest.git' + '@' + Var('googletest_revision'),

  'src/third_party/icu':
    Var('chromium_git') + '/chromium/deps/icu.git' + '@' + Var('icu_revision'),

  'src/third_party/modp_b64':
    Var('chromium_git') + '/chromium/src/third_party/modp_b64.git' + '@' + Var('modp_b64_revision'),

  'src/third_party/zlib':
    Var('chromium_git') + '/chromium/src/third_party/zlib' + '@' +  Var('zlib_revision'),

  'src/tools/clang':
    Var('chromium_git') + '/chromium/src/tools/clang' + '@' +  Var('tools_clang_revision'),

  'src/tools/gyp':
    Var('chromium_git') + '/external/gyp.git' + '@' + Var('gyp_revision'),

  'src/tools/win':
    Var('chromium_git') + '/chromium/src/tools/win' + '@' +  Var('tools_win_revision'),
}

hooks = [
  {
    # Update LASTCHANGE.
    'name': 'lastchange',
    'pattern': '.',
    'action': ['python', 'src/build/util/lastchange.py',
               '-o', 'src/build/util/LASTCHANGE'],
  },

  # Pull GN binaries. This needs to be before running GYP below.
  {
    'name': 'gn_win',
    'pattern': '.',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=win32',
                '--no_auth',
                '--bucket', 'chromium-gn',
                '-s', 'src/buildtools/win/gn.exe.sha1',
    ],
  },
  # Pull clang-format binaries using checked-in hashes.
  {
    'name': 'clang_format_win',
    'pattern': '.',
    'action': [ 'download_from_google_storage',
                '--no_resume',
                '--platform=win32',
                '--no_auth',
                '--bucket', 'chromium-clang-format',
                '-s', 'src/buildtools/win/clang-format.exe.sha1',
    ],
  },
  {
    # Pull clang if needed or requested via GYP_DEFINES.
    # Note: On Win, this should run after win_toolchain, as it may use it.
    'name': 'clang',
    'pattern': '.',
    'action': ['python', 'src/tools/clang/scripts/update.py', '--if-needed'],
  },
  # Pull rc binaries using checked-in hashes.
  {
    'name': 'rc_win',
    'pattern': '.',
    'condition': 'checkout_win and host_os == "win"',
    'action': [ 'python',
                'src/third_party/depot_tools/download_from_google_storage.py',
                '--no_resume',
                '--no_auth',
                '--bucket', 'chromium-browser-clang/rc',
                '-s', 'src/build/toolchain/win/rc/win/rc.exe.sha1',
    ],
  },
]

recursedeps = [
  # buildtools provides clang_format, libc++, and libc++abi
  'src/buildtools',
]
