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

Install the kernel and then the userspace parts.

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

