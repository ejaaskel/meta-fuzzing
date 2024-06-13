DESCRIPTION = "Simple sysfs string echo driver"
SECTION = "examples"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

inherit module

SRC_URI = " \
    file://sysfs_attribute_echo.c \
    file://Makefile \
"

S = "${WORKDIR}"
