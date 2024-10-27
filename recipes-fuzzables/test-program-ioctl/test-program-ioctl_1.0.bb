SUMMARY = "Simple test application for ioctl kernel module"
SECTION = "examples"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"

SRC_URI = "file://source.c"

S = "${WORKDIR}"

do_compile() {
    ${CC} ${LDFLAGS} source.c -o test-program-ioctl
}

do_install() {
    install -d ${D}${bindir}
    install -m 0755 test-program-ioctl ${D}${bindir}
}
