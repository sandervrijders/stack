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
