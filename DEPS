# This file is used to manage the dependencies of the Evita src repo. It is
# used by gclient to determine what version of each dependency to check out, and
# where.

vars = {
  'chromium_git': 'https://chromium.googlesource.com',

  'base_revision': '8fe126945cb9fe557d679362b4e228324efa8a2b',
  'build_revision': '2d722d1684b43ee840d499c818f012a18942a349',
  'buildtools_revision': 'b3771b1935ea74c388b6fb1750e83f5d83b39dec',
  'ced_revision': '368a9cc09ad868a3d28f0b5ad4a733f263c46409',
  'gmock_revision': '0421b6f358139f02e102c9c332ce19a33faf75be', # from svn revision 566
  'gtest_revision': '6f8a66431cb592dad629028a50b3dd418a408c87',
  'gyp_revision': 'e7079f0e0e14108ab0dba58728ff219637458563',
  'icu_revision': '450be73c9ee8ae29d43d4fdc82febb2a5f62bfb5',
  'modp_b64_revision': '28e3fbba4cb4ec3ffd85b53d0a3904525d08f5a6',
  'testing_revision': 'a4984ec2b072cebb77259815511f85cfcfec8a4a',
  'zlib_revision': '13dc246a58e4b72104d35f9b1809af95221ebda7',
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

  'src/testing/gmock':
    Var('chromium_git') + '/external/googlemock.git' + '@' + Var('gmock_revision'),

  'src/testing/gtest':
    Var('chromium_git') + '/external/github.com/google/googletest.git' + '@' + Var('gtest_revision'),

  'src/tools/gyp':
    Var('chromium_git') + '/external/gyp.git' + '@' + Var('gyp_revision'),

  'src/third_party/ced/src':
    Var('chromium_git') + '/external/github.com/google/compact_enc_det.git' + '@' + Var('ced_revision'),

  'src/third_party/icu':
    Var('chromium_git') + '/chromium/deps/icu.git' + '@' + Var('icu_revision'),

  'src/third_party/modp_b64':
    Var('chromium_git') + '/chromium/src/third_party/modp_b64.git' + '@' + Var('modp_b64_revision'),

  'src/third_party/zlib':
    Var('chromium_git') + '/chromium/src/third_party/zlib' + '@' +  Var('zlib_revision'),
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
]

recursedeps = [
  # buildtools provides clang_format, libc++, and libc++abi
  'src/buildtools',
]
