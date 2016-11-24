# The SendIN function is the NATP function for packet going
# to the outside network.
# It change the SRC IP/PORT of the UDP packets.
# (for TCP packet the only difference is the position of the TCP checksum)
# The incremental csum computation is based on the following formula:
# newCSUM = (oldCSum' + oldFieldSum' + newFieldSum)'
# where ' is the NOT operation

# Start .text segment (program code)
.text

SendIN:
 la $1,out_dmac
 #li $2, 20 # print reg
 #syscall

#write the ethernet layer
# la $3,pkt_in       # $3<--in pkt
# li $2,14
# outl $op,($1),$2

 la $3,pkt_in       # $3<--in pkt
 la $5,op           # $5<--op
 lw $4,($3)
 stw ($5),$4
 lw  $4,4($3)
 stw ($5),$4
 lw  $4,8($3)
 stw ($5),$4
 lw  $4,12($3)
 sth ($5),$4


#load old UDP csum in $13
 lw $13,40($3)

#keep old IP in $21
  lwf $21,26($3)

#compute IP csum (4 byte) 
  sth 24($3),$0   #set csum to 0 
  lwf $5,14($3)
  lwf $6,18($3)
  lwf $7,22($3)
  la $13,source_IP
  lwf $8,($13)
  lwf $9,30($3)
  add $10,$6,$5
  adc $10,$10,0
  add $10,$10,$7
  adc $10,$10,0  
  add $10,$10,$8
  adc $10,$10,0
  add $10,$10,$9
  adc $10,$10,0

#shrink 32 bit csum in 16 bit csum
  slr $9,$10,16
  andi $10,$10,0x0000ffff
  add $11,$10,$9
  adc $11,$11,0
  not $11,$11


#compute incremental UDP csum
  #not (old udp_csum)
  lwf $20,40($3)
  not $20,$20
  slr $20,$20,16

  #not old IP
  not $28,$21

  #new IP in $8
  add $28,$28,$8
  adc $28,$28,0

  slr $27,$28,16
  andi $28,$28,0x0000ffff
  add $22,$27,$28
  adc $22,$22,0

  #old port $23 
  lwf $23,34($3)
  not $23,$23
  slr $23,$23,16

  #new port $24
  la $21,source_port
  lwf $24,($21)
  slr $24,$24,16

  #new UDP csum $12
  add $13,$20,$22
  adc $13,$13,0

  add $13,$13,$23
  adc $13,$13,0

  add $13,$13,$24
  adc $13,$13,0
  not $13,$13

 #li $2, 20 # print reg
 #syscall

## resume sending out the packet
 li $2,10                                           
 outl $op,14($3),$2     #send beginning of IP pkt

 la   $10,new_csum_1    #write new IP csum
 sth  ($10),$11         #write new IP csum

 la   $12,new_csum_3    #write new UDP csum
 sth  ($12),$13         #write new UDP csum

 outb $op,new_csum_2    #write new IP csum
 outb $op,new_csum_1    #write new IP csum
 outw $op,source_IP     #write new src IP
 li $2,4
 outl $op,30($3),$2     #write dst IP
 outh $op,source_port   #write new src port
 li $2,4
 outl $op,36($3),$2 
 
 outb $op,new_csum_4    #write new UDP csum
 outb $op,new_csum_3    #write new UDP csum
 
# compute the number of bytes from UDP csum to EOP
 ld   $2,pkt_len   # $2<--pkt_len
 subi $2,$2,42
 la $5,op           # $5<--op


# loop to output the pkt
loop:
    bge $0,$2, end_loop # if $2 < 0, branch out of loop.
    #beqz $2, end_loop # if $22 == 0, branch out of loop.
    lw  $4,($3)
    stw ($5),$4
    subi $2,$2,4
    addi $3,$3,4
    b loop                   #loop
end_loop:

# exit: halt the PMP
exit:
 li $2, 11 # exit 
 syscall

## data segment
.data

#memory space for the pkt in
pkt_in:      .space 2048

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
srcip_offset:   .byte 0x1A 0x00 0x00 0x00       

# program arguments
metadata_space: .space 2017

#output space for pkt out
op:   .space 2048

# internal data
out_dmac:   .byte 0x00 0x34 0x56 0x78 0x9a 0xbc 
out_smac:   .byte 0x00 0x43 0x65 0x87 0xa9 0xcc 
out_type:   .byte 0x08 0x00
out_ip:     .byte 0x45 0x00
fixed_chk:  .byte 0x12 0xc3 
source_IP:   .byte 0xA0 0x50 0x50 0x14 
source_port: .byte 0x10 0x20
dest_IP:    .byte 0x90 0xab 0xcd 0xef
outer_ttl:  .byte 0x40
IP_prot:    .byte 0x04
new_csum_1: .byte 0x00
new_csum_2: .byte 0x00
new_csum_3: .byte 0x00
new_csum_4: .byte 0x00
