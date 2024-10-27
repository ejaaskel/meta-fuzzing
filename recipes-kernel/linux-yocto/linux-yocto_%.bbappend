FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

SRC_URI:append = " file://syzkaller-minimal.cfg"
