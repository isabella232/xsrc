#
# Copyright (c) 2010, Oracle and/or its affiliates. All rights reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice (including the next
# paragraph) shall be included in all copies or substantial portions of the
# Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
#

adminmandir = $(ADMIN_MAN_DIR)
adminman_PRE = xdm.man
adminman_DATA = $(adminman_PRE:man=$(ADMIN_MAN_SUFFIX))

xdmlmandir = $(ADMIN_MAN_DIR)
xdmlman_PRE = xdmshell.man

if INSTALL_XDMSHELL
xdmlman_DATA = $(xdmlman_PRE:man=$(ADMIN_MAN_SUFFIX))
else
noinst_DATA = $(xdmlman_PRE:man=$(ADMIN_MAN_SUFFIX))
endif

EXTRA_DIST = $(adminman_PRE) $(xdmlman_PRE)
CLEANFILES = $(adminman_DATA) $(xdmlman_DATA) $(noinst_DATA)
SUFFIXES = .$(ADMIN_MAN_SUFFIX) .man

# String replacements in MAN_SUBSTS now come from xorg-macros.m4 via configure
MAN_SUBSTS +=	-e 's|CHOOSERPATH|$(XDMLIBDIR)/chooser|g' \
		-e 's|XDMLOGDIR|$(XDMLOGDIR)|g' \
		-e 's|XDMDIR|$(XDMCONFIGDIR)|g' \
		-e 's|BINDIR|$(bindir)|g' \
		-e 's|XDMPIDDIR|$(XDMPIDDIR)|g' \
		-e 's|XDMXAUTHDIR|$(XDMXAUTHDIR)|g' \
		-e 's|DEF_USER_PATH|$(DEF_USER_PATH)|g' \
		-e 's|DEF_SYSTEM_PATH|$(DEF_SYSTEM_PATH)|g' \
		-e 's|DEF_GREETER_LIB|$(DEF_GREETER_LIB)|g' \
		-e 's|DEV_RANDOM|$(DEV_RANDOM)|g' \
		-e 's|ARC4_RANDOM|$(HAVE_ARC4RANDOM)|g'

.man.$(ADMIN_MAN_SUFFIX):
	$(AM_V_GEN)$(SED) $(MAN_SUBSTS) < $< > $@
