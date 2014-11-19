---
title: IRATI project
layout: default
---

What is this?
=============

IRATI is a prototype of the Recursive InterNetwork Architecture
([RINA](http://rina.tssg.org/)), targeted for GNU/Linux OS platforms
with key components implemented directly in the kernel. For more
information we redirect you to this
[paper](http://dx.doi.org/10.1109/MNET.2014.6786609).

The source code is available on
 [GitHub](https://github.com/sandervrijders/stack). It is also
 available as a [zip](https://github.com/irati/stack/zipball/master) file or as
 a [tarball](https://github.com/irati/stack/tarball/master).

Who are we?
=============

This source code was developed by members of the IRATI project
([www.irati.eu][]), funded by the European commission under the 7th
Framework Programme grant number 317814.

The contributing members of the project are (alphabetical order):

-   Addy Bombeke ([@PaddyWan][]) - Ghent University/iMinds, BE
-   Bernat Gaston ([@bernatgaston][]) - Fundacio i2cat, ES
-   Dimitri Staessens ([@dstaesse][]) - Ghent University/iMinds, BE
-   Douwe De Bock ([@douwedb26][]) - Ghent University/iMinds, BE
-   Eduard Grasa ([@edugrasa][]) - Fundacio i2cat, ES
-   Francesco Salvestrini ([@salvestrini][]) - Nextworks s.r.l., IT
-   Leonardo Bergesio ([@lbergesio][]) - Fundacio i2cat, ES
-   Miquel Tarzan ([@miqueltarzan][]) - Fundacio i2cat, ES
-   Sander Vrijders ([@sandervrijders][]) - Ghent University/iMinds, BE
-   Vincenzo Maffione ([@vmaffione][]) - Nextworks s.r.l., IT

  [www.irati.eu]: http://www.irati.eu
  [@PaddyWan]: https://github.com/PaddyWan
  [@bernatgaston]: https://github.com/bernatgaston
  [@dstaesse]: https://github.com/dstaesse
  [@douwedb26]: https://github.com/douwedb26
  [@edugrasa]: https://github.com/edugrasa
  [@salvestrini]: https://github.com/salvestrini
  [@lbergesio]: https://github.com/lbergesio
  [@miqueltarzan]: https://github.com/miqueltarzan
  [@sandervrijders]: https://github.com/sandervrijders
  [@vmaffione]: https://github.com/vmaffione

License
=============

librina: See librina/LICENSE for the main license (LGPL). See
librina/LICENSE-protobuf for the protocol buffers license.

linux: See linux/COPYING for the Linux kernel license (GPL).

rina-tools: See rina-tools/LICENSE for the license (GPL).

rinad: See rinad/LICENSE for the main license (GPL). See
rinad/LICENSE-protobuf for the protocol buffers license. See
rinad/LICENSE-jsoncpp for the jsoncpp license (MIT). See
rinad/LICENSE-tclap for the TCLAP license.

wireshark: See wireshark/COPYING for the license.

Getting started
=============

## How to install the IRATI stack ##

The IRATI stack should run on all GNU/Linux OS platforms, if it does not, 
please file a [bug report](https://github.com/IRATI/stack/issues/new).

### How to install the stack in Debian/Linux 7.0 (wheezy) ###

For the kernel parts, the following packages are required:
    
* kernel-package
* libncurses5-dev

For the user-space parts, the following packages are required:

* autoconf
* automake
* libtool
* pkg-config
* git
* g++
* protobuf-compiler
* libprotobuf-dev
* openjdk-6-jdk
* maven 
* SWIG version 2.x, from http://swig.org
  * version >2.0.8 required, version 2.0.12 is known to be working fine
  * Depending on your intended setup, libpcre3-dev might be required. If
    you don't want its support, just configure SWIG without it by
    passing the --without-pcre option to its `configure' script.
* libnl-genl-3-dev and libnl-3-dev:
  * Add 'deb http://ftp.de.debian.org/debian jessie main' in
     /etc/apt/sources.list
  * Run 'apt-get update'
  * Run 'apt-get install libnl-genl-3-dev libnl-3-dev'

To use the shim-eth-vlan module you also need to install the 'vlan'
package.

To install the stack, start by running 'make menuconfig' in the linux
directory. The IRATI parts can be found at:

    +- Networking support
    |  +- RINA support 

To get a functioning stack, at least the 'Default Personality Support'
has to be built as a module, and one or more IPC processes as well.
If you are using the shim Ethernet IPC process, it is also required to
build the RFC-826 ARP compliant implementation and the RINA ARP
implementation. In order to fully be able to use the IRATI stack, 
we advise to also build the normal IPC process.

To install both the kernel and userspace, you can use the
install-from-scratch script. The script takes one parameter, the
location where to install the user-space parts; if left empty, they
will be installed in the root of the filesystem. Installation will
take a while, since a linux kernel has to be compiled and the
user-space parts have to be installed as well. After the stack has
been installed, reboot the machine. You can use 'uname -a' to check if
the new kernel was loaded, and you can browse to the folder where you
installed the user-space parts to verify they are there.

Alternatively, you can install the kernel manually. To compile and
install the kernel (and its headers), do the following:

    make headers_install
    make bzImage modules
    make modules_install install

Afterwards, run the install-user-from-scratch script to install the
user-space parts.

## Running the IRATI stack ##

First modprobe all the different modules you need. Create a VLAN
device if you are using the shim-eth-vlan. Next, the IPC Manager has
to be started in userspace, which needs some configuration info.

### Configuring the IPC Manager ###

You can use the JSON file as input to configure the IPC Manager or the telnet console.

#### The JSON config file ####

Takes a lot of sections which tell the IPC Manager to create the IPC
processes and with which config params.

#### The telnet console ####

A different way is to use the telnet console, which allows the same
flexibility in configuring.

Now applications can be run that use the IPC API.

## Tutorial: Run an application over a normal IPC Process over a shim IPC Process ##

Shows two JSON config files that the IPCM on each node can load. Next,
rina-echo-time is explained.

