project('libcie-pkcs11', 'cpp',
  version : '1.4.1',
  license : 'MIT')
add_project_arguments('-UWIN32 -U_WIN64 -fPIC', language : 'cpp')
crypto_dep = dependency('libcrypto')
cryptop_dep = dependency('libcrypto++', required : false)
cryptopp_dep = dependency('libcryptopp', required : false)
pcsclite_dep = dependency('libpcsclite')
thread_dep = dependency('threads')

inc = include_directories('/usr/include/PCSC', '/usr/include', '.', 'Crypto', 'CSP', 'PCSC', 'PKCS11', 'UI', 'Util')

cie_pkcs11_sources = ['Util/UUCByteArray.cpp', 'Util/UtilException.cpp', 'Util/SyncroEvent.cpp', 'Util/Array.cpp', 'Util/TLV.cpp', 'Util/UUCProperties.cpp', 'Util/CacheLib.cpp', 'Util/UUCStringTable.cpp', 'Util/funccallinfo.cpp', 'Util/ModuleInfo.cpp', 'Util/UUCTextFileReader.cpp', 'Util/SyncroMutex.cpp', 'Util/log.cpp', 'Util/CryptoppUtils.cpp', 'Util/util.cpp', 'Util/IniSettings.cpp', 'CSP/ExtAuthKey.cpp', 'CSP/PINManager.cpp', 'CSP/AbilitaCIE.cpp', 'CSP/IAS.cpp', 'PCSC/Token.cpp', 'PCSC/PCSC.cpp', 'PCSC/APDU.cpp', 'PCSC/CardLocker.cpp', 'PKCS11/session.cpp', 'PKCS11/P11Object.cpp', 'PKCS11/CardTemplate.cpp', 'PKCS11/CIEP11Template.cpp', 'PKCS11/CardContext.cpp', 'PKCS11/Mechanism.cpp', 'PKCS11/PKCS11Functions.cpp', 'PKCS11/initP11.cpp', 'PKCS11/Slot.cpp', 'Crypto/SHA512.cpp', 'Crypto/DES3.cpp', 'Crypto/SHA1.cpp', 'Crypto/ASNParser.cpp', 'Crypto/Base64.cpp', 'Crypto/SHA256.cpp', 'Crypto/MAC.cpp', 'Crypto/MD5.cpp', 'Crypto/RSA.cpp', 'Crypto/AES.cpp']

libcie_pkcs11 = shared_library(
  'cie-pkcs11',
  cie_pkcs11_sources,
  include_directories : inc,
  dependencies : [ crypto_dep, cryptop_dep, cryptopp_dep, pcsclite_dep, thread_dep ],
  install : true
)


pkg_mod = import('pkgconfig')
pkg_mod.generate(
  libraries : libcie_pkcs11,
  version : '1.0',
  name : 'libcie-pkcs11',
  filebase : 'cie-pkcs11',
  description : 'A library to enable CIE authentication.'
)
