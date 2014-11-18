---
title: Installation
layout: default
---

The IRATI stack should run on all GNU/Linux OS platforms, if it does not, 
please file a [bug report](https://github.com/IRATI/stack/issues/new).

## How to install the stack in Debian/Linux 7.0 (wheezy) ##

For the kernel parts, the following packages are required:
    
* kernel-package
* libncurses5-dev

To install the kernel parts, run 'make menuconfig'. The IRATI parts
can be found at:

    +- Networking support
    |  +- RINA support 

At the time of writing (18/11/2014), the IRATI tree had the following
structure:

    +- Debugging support
    |  +- Debugging support
    |  +- Debug RINA syscalls
    |  +- K-IPC Manager locks debugging
    |  +- Dump heartbeat messages
    |  +- Embed assertions (slowing down everything)
    |  +- Remove code related to debugging logs
    |  +- Enable regression testing suites
    +- Core
    |  +- Memory allocation/deallocation stats
    |  |  +- Rate limit mem-stat dumps (in msecs)
    |  +- Memory allocation/deallocation extra checks
    |  |  +- Memory blocks tampering
    |  |  |  +- Memory blocks tampering verbose bloatings
    |  |  +- Memory blocks poisoning
    |  |  |  +- Memory blocks poisoning verbose bloatings
    |  |  +- Dump pointers passed to alloc()/returned by free()
    |  +- Regression testing
    |  |  +- RDS regression tests
    |  |  +- RTimer regression tests
    |  |  +- RQUEUE regression tests
    |  |  +- RFIFO regression tests
    |  |  +- RMEM regression tests
    |  +- Default Personality support
    +- IPC Processes
    |  +- Normal IPC Process
    |  |  +- RMT regression tests
    |  |  +- PFT regression tests
    |  +- Shim IPC Dummy (loopback)
    |  +- Shim IPC over Ethernet (using VLANs) support
    |  |  +- Ethernet burst limiting
    |  |  +- Receiver burst limit frames count
    |  |  +- Perform regression testing upon loading
    |  +- Shim IPC over TCP/UDP support
    |  +- Shim IPC for Hypervisors
    |  +- RFC-826 ARP compliant implementation
    |  |  +- Perform regression testing upon loading
    |  |  +- RINA ARP implementation
    |  +- VMPI support
    |  |  +- VMPI for KVM and Virtio - guest support
    |  |  +- VMPI for KVM and Virtio - host support
    |  |  +- VMPI for Xen - guest support
    |  |  +- VMPI for Xen - host support

To get a functioning stack, at least the 'Default Personality Support'
has to be built as a module, and one or more IPC processes as well.
If you are using the shim Ethernet IPC process, it is also required to
build the RFC-826 ARP compliant implementation and the RINA ARP
implementation. In order to fully be able to use the IRATI stack, 
we advise to also build the normal IPC process.

To compile and install the kernel, do the following:

    make bzImage modules
    make modules_install install

After the new kernel has been installed, reboot the machine. You can
use 'uname -a' to check if the new kernel was loaded.

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

To use the shim-eth-vlan module you also need to install the 'vlan' package.
