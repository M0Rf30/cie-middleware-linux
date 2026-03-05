project('cie-middleware', 'cpp', 'c', version: '2.0.0', license: 'GPL-2.0-or-later')

# This project now depends on the modern opencie-pkcs11 library
# instead of building an embedded copy of the old libcie-pkcs11

# OpenCIE PKCS#11 dependency
# This can be:
# 1. A system-installed package (pkg-config)
# 2. A local build path specified via PKG_CONFIG_PATH
# 3. Users can manually install the library system-wide

opencie_dep = dependency('opencie-pkcs11', required: false)

if not opencie_dep.found()
  # Try alternative names
  opencie_dep = dependency('libopencie-pkcs11', required: false)
endif

if not opencie_dep.found()
  # Don't fail the build - the Java UI can still work if the library is installed
  message('opencie-pkcs11 not found as pkg-config dependency')
  message('')
  message('To use this middleware:')
  message('1. Install opencie-pkcs11 system-wide:')
  message('   git clone https://github.com/M0Rf30/opencie-pkcs11.git')
  message('   cd opencie-pkcs11 && meson setup builddir && meson install -C builddir')
  message('')
  message('2. Or set PKG_CONFIG_PATH to point to a built opencie-pkcs11:')
  message('   PKG_CONFIG_PATH=/path/to/opencie-pkcs11/builddir meson setup builddir libs')
  message('')
  message('3. The Java UI can load libopencie-pkcs11.so directly if installed system-wide')
  
  warning('opencie-pkcs11 not found as build dependency, but Java UI may still work if libopencie-pkcs11.so is available')
endif

# Just export the dependency for now
# The Java UI will load libopencie-pkcs11.so directly via JNA
summary('opencie-pkcs11', opencie_dep.found() ? 'found' : 'not found (Java UI may still work)')