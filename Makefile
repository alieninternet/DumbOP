# Generated automatically from Makefile.in by configure.
#
#
# ./Makefile
# Make entire DumbOP package
# Author: Simon Butcher
# Copyright (c) 2001 Alien Internet Services
#

RM=rm -vf

SUB_DIRS=src

.PHONY: all clean dclean autoconf dready dsums distro

# Make the package
all:
	@echo "**********************************************************"
	@echo "**           Making the DumbOP IRC bot package          **"
	@echo "**********************************************************"
	for d in $(SUB_DIRS); do $(MAKE) -C $$d; done

# Clean the package for a fresh start
clean:
	@echo "**********************************************************"
	@echo "**              Cleaning out created files              **"
	@echo "**********************************************************"
	for d in $(SUB_DIRS); do $(MAKE) -C $$d clean; done
	$(RM) *~

# Clean the package for a possible new distribution
dclean:
	@echo "**********************************************************"
	@echo "**    Performing a full clean ready for a new version   **"
	@echo "**********************************************************"
	for d in $(SUB_DIRS); do cd $$d; $(MAKE) dclean; cd ..; done
	$(RM) *~ Makefile config.log config.cache config.status configure.scan $(CRC_FILE)
	
# Create AutoConf files from templates
autoconf:
	@echo "**********************************************************"
	@echo "**     Creating new configure script and autoconf.h     **"
	@echo "**********************************************************"
	autoconf configure.in >configure
	-chmod 755 configure
	autoheader --localdir=src/include

# Create files, like the mini-makefile and crc file, for a new distribution
dready:
	@echo "**********************************************************"
	@echo "** Creating self-destructing configure-compile Makefile **"
	@echo "**********************************************************"
	@echo "# Automatic configure and compile Makefile" > Makefile
	@echo "# Copyright (c) 2001 Alien Internet Services" >> Makefile
	@echo "#" >> Makefile
	@echo "# Generated automatically from Makefile.in by configure." >> Makefile
	@echo "# This makefile was pre-generated and will be destroyed" >> Makefile
	@echo "" >> Makefile
	@echo "all:" >> Makefile
	@echo "	@echo \"**********************************************************\"" >> Makefile
	@echo "	@echo \"**    Configuring the DumbOP package to your system     **\"" >> Makefile
	@echo "	@echo \"**********************************************************\"" >> Makefile
	@echo "	@./configure" >> Makefile
	@echo "	@echo \"**********************************************************\"" >> Makefile
	@echo "	@echo \"**                Compiling DumbOP package              **\"" >> Makefile
	@echo "	@echo \"**********************************************************\"" >> Makefile
	@echo "	make" >> Makefile
	@echo "" >> Makefile
	@echo "install: all" >> Makefile
	@echo "	make install" >> Makefile
	@echo "**********************************************************"
	@echo "**         Creating CRC and file-size index file        **"
	@echo "**********************************************************"

dsums:
	-sum `find` >BSD.SUM
	-md5sum `find` >MD5.SUM

# Setup package for new distribution or archive
distro: autoconf dclean dready dsums
	@echo "Done."
