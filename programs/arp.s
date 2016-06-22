#
# The ARPreply function provide an ARP reply packet
#

# Start .text segment (program code)
.text

ARPreply:

## write DST MAC
   outwh $op, src_mac         # write the destination MAC


   la $1,sender_mac
   li $2,22
   outl $op, ($1),$2        #
   
   outw $op, dst_ip         # 
   outwh $op, src_mac       # 
   outw $op, src_ip         # 
   
#padding
   la $1,padding
   li $2,18
   outl $op, ($1),$2         #
# exit: halt the PMP
exit:
    li $2, 11 # exit 
    syscall

.data

#memory space for the pkt
pkt_start:     .space 2048

#metadata from parser
pkt_len:        .space 4       
src_mac:        .space 4
src_mac4:       .space 2
dst_mac:        .space 4
dst_mac4:       .space 2
eth_type:       .space 2 
src_ip:         .space 4       
dst_ip:         .space 4       
proto:          .space 1
src_port:       .space 2
dst_port:       .space 2

# program arguments
metadata_space: .space 2017

#output space
op:             .space 2048

# internal data
sender_mac:  .byte 0x00 0x34 0x56 0x78 0x9a 0xbc
eth_type:    .byte 0x08 0x06
hw_type:     .byte 0x00 0x01
prot_type:   .byte 0x08 0x00
hw_size:     .byte 0x06 0x04
opcode:      .byte 0x00 0x02
sender_mac_: .byte 0x00 0x34 0x56 0x78 0x9a 0xbc
sender_ip:   .byte 0x44 0x66 0x88 0x99
padding:     .space 24
