FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " file://syzkaller-minimal.cfg"
SRC_URI:append:aarch64 = " file://0001-Add-Kasan-definitions-to-own-header.patch"
