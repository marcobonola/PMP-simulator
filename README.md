# PMP project

The idea behind Software Defined Networking is to substitute the current network devices with programmable elements that will provide to network engineers an enormous flexibility to modify and update the network behavior. The most affirmed programming abstraction for SDN is based on the use of multiple MATs (Match-Action Tables). This abstraction allows selecting the set of actions that must be performed on each incoming packet. Until now, most of the research effort focused on the match operation, leaving the definition and implementation of the actions to the specific HW device or to the vendors. 

Instead, in this project we identify the possible actions that a network device should apply to the processed packets (MPLS labeling, encapsulation and de-encapsulation, NAT etc.) and we describe an HW architecture able to efficiently perform theise actions. The HW architecture, that we called packet manipulation processor (PMP), is an implementation of an array of small RISC microprocessors, in which the memory architecture and the instruction set has been optimized to better accomplish the specific task of packet manipulation.

The PMP simulator process a pcap trace, extract the relevant header informations and activates a sepcific five-stage MIPS CPU Pipelined to manimulate the packet trace. The MIPS simulator is based on the original simulator developed by Christopher Lord (https://github.com/clord/MIPS-CPU-Simulator).

# Getting Started

These instructions have been tested on Ubuntu 12.04. Other distributions or versions may need different steps.

# Building

Run the following commands in the PMP simulator directory to build everything:

```
$ make
```

# Running
1. assembly the microprogram

    ```
    $ ./rasm -f source_file
    ```

    This will create a output .text file with the binary executable code and a .data file with the memory data section.

2. launch the PMP simulator

    ```
    $ ./rsim -p trace.pcap -t program.t -d program.d 

    ```

    The simulator will parse the packet trace trace.pcap and will launch the PMP simulator that will execute the program in program.t with data program.d

# Contribute

Please submit your bug reports, fixes and suggestions as pull requests on GitHub, or by contacting us directly.

# License

The MIT License

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
