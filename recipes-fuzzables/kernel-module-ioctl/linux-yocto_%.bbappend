FILESEXTRAPATHS:prepend := "${THISDIR}/files:"

# The source is added by copying because I want to support both in-tree and
# out-of-tree module builds, and the source is expected to change multiple
# times and I do not want to re-generate a patch constantly. Constant Kconfig
# and Makefile changes can be applied as patch.
SRC_URI:append = "${@ ' file://ioctl_string_parse.c \
                        file://ioctl_string_parse.h \
                        file://0001-misc-Add-Kconfig-and-Makefile-for-ioctl-string-parse.patch \
		        file://ioctl_string_parse.cfg \
                      ' if d.getVar('IOCTL_STRING_PARSE_INTREE') else ''}"

do_unpack_ioctl_string_parse_source () {
    if [ "${IOCTL_STRING_PARSE_INTREE}" -eq "1" ]; then
        cp ${WORKDIR}/ioctl_string_parse.c ${S}/drivers/misc
        cp ${WORKDIR}/ioctl_string_parse.h ${S}/include/linux
    fi
}

addtask unpack_ioctl_string_parse_source after do_kernel_checkout before do_patch
