# The SendIN function is the NATP function for packet going
# to the outside network.
# It change the SRC IP/PORT of the TCP/UDP packets.

# Start .text segment (program code)
.text

SendIN:
 la $1,out_dmac      #1

 li $2, 20 # print reg
 syscall


#write the ethernet layer
 la $3,pkt_in       # $3<--in pkt                   #1
 li $2,14                                           #1
 outl $op,($1),$2                                   #3

#load old IP csum in $12
# lw  $12,24($3)                                     #1
# not $12,$12                                        #1

#load old UDP csum in $13
 lw $13,40($3)                                      #1

#compute IP delta csum (4 byte)                     #8
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

 li $2, 20 # print reg
 syscall


  add $7,$6,$5
  add $7,$7,$5
  la $5,source_IP
  lw $6,($5)
  add $7,$7,$6
  lw $5,30($3)
  add $7,$7,$5




  slr $8,$7,16
  add $8,$8,$7
  adc $8,$8,0

# la  $5,source_IP                                
# lw  $6,($5)
# lw  $6,26($3)
# not $6,$6
# add $12,$12,$6
# lw  $7,26($3)
# not $7,$7
# sub $12,$12,$7
# slr $13,$12,16
# add $13,$13,$12
# adc $13,$13,0
  not $14,$8

#new IP csum in $8                                  #1
# add $8,$8,$12

#compute L4 delta csum                              #9
 la  $6,source_port                         
 lw  $9,($6)
 lw  $10,34($3)
 add $11,$11,$10
 add $11,$7,$10 
 
 
 slr $12,$11,16
 add $12,$12,$11
 adc $12,$12,0
 not $8,$11

#new L4 csum in $9                                  #1
 add $9,$8,$13

## set-up phase                                     #11
 li $2,10                                           
 outl $op,14($3),$2 
 la   $10,new_csum    #write new IP csum
 sth  ($10),$14        #write new IP csum
 outh $op,new_csum    #write new IP csum
 outw $op,source_IP   #write new src IP
 li $2,4
 outl $op,30($3),$2   #write dst IP
 outh $op,source_port #write new src port
 li $2,4
 outl $op,36($3),$2 
 
 sth  ($10),$9        #store new UDP csum
 outh $op,new_csum    #write UDP checksum
 
# compute the number of bytes from UDP csum and eop     2 + lunghezza pacchetto 64 --> 4 1500---> 93   
 ld   $2,pkt_len   # $3<--pkt_len
 subi $2,$2,42
 outl $op,42($3),$2  # out data to the output port

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
new_csum: .byte 0x00 0x00
