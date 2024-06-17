SUMMARY = "Radamsa black box fuzzer"

LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://LICENCE;md5=dfdd63fe51232c4632340bde657dc414"

SRC_URI = "git://gitlab.com/akihe/radamsa.git;protocol=https;branch=develop;name=radamsa; \
           git://gitlab.com/owl-lisp/hex.git;protocol=https;branch=master;name=hex;destsuffix=git/lib/hex \
           https://haltp.org/files/ol-0.2.2.c.gz;name=ol"

S = "${WORKDIR}/git"

SRCREV_radamsa = "40d5dec416fb5277dbbd72c04b82ba2ae039778a"
SRCREV_hex = "e95ebd38e4f7ef8e3d4e653f432e43ce0a804ca6"
SRC_URI[ol.sha256sum] = "fca85dae36910108598d8a4a244df7a8c2719e7803ac46d270762ece4aefc55c"

SRCREV_FORMAT = "radamsa_hex"

do_compile:prepend() {
    # Pre-compile the owl lisp dependency for the build architecture
    cd ${S}
    cp ${WORKDIR}/ol-0.2.2.c ./ol.c
    ${BUILD_CC} -o bin/ol ol.c
}

do_install() {
    install -Dm755 ${S}/bin/radamsa ${D}${bindir}/radamsa
}

inherit autotools
