#include <algorithm>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <pcap.h>
#include <stdio.h>
#include "memory.h"
#include "cpu.h"
#include "dump.h"
#include "parser.h"

extern char   *optarg;
extern int32_t optind;
extern int32_t optopt;
extern int32_t opterr;
extern int32_t optreset;

using namespace std;

// Usage of the program
static void usage(char *name)
{
	cout << name << " usage:\n" <<
	        "\t-p cpap_file: input pcap file for simulation\n" <<
	        "\t-t text_stream_file: load .text with the contents of file\n" <<
	        "\t-d data_stream_file: [optional] load .data with contents of file\n" <<
	        "\t-v: very verbose single-click CPU (echo every stage, pause after each cycle)\n" <<
	        "\t-m: collect and display memory statistics\n\t\t(position relative to -t and -d is important)\n";
}


// *****************************
//           entry point
// *****************************
int32_t main(int32_t argc, char **argv)
{
    memory   mem;
	bool     verb = false;
	bool     text_loaded = false;
	//bool     show_cycles = false;
	int32_t  ch;
	uint32_t text_ptr = text_segment;
	uint32_t data_ptr = data_segment;
    FILE *pcap_file=NULL;

	while ((ch = getopt(argc, argv, "p:t:d:vmc")) != -1) {
		switch (ch) {
        
		case 't': {
			ifstream input(optarg, ios::binary);
			if (!(input.good() && input.is_open())) {
				cout << *argv << ": " << optarg << " does not exist" << endl;
				exit(20);
			}
			byte  c;
			char *pc = (char *)&c;
			while (!input.eof()) {
				input.read(pc, sizeof(byte));
				mem.set<byte>(text_ptr++, c);
			}
			input.close();
			text_loaded = true;
		}
		break;
        
        case 'p': {
            pcap_file = fopen(optarg, "rb");
    
            if (pcap_file == NULL) {
                cout << *argv << ": " << optarg << " does not exist" << endl;
                exit(20);
            } 
        }
        break;
		case 'm':
			mem.collect_stats(true);
			break;

		case 'c':
			//show_cycles = true;
			break;

		case 'd': {
			ifstream input(optarg, ios::binary);
			if (!(input.good() && input.is_open())) {
				cout << *argv << ": " << optarg << " does not exist" << endl;
				exit(20);
			}
			byte c;
			while (!input.eof()) {
				input.read((char *)&c, sizeof(byte));
				mem.set<byte>(data_ptr++, c);
			}
			input.close();
		}
		break;

		case 'v':
			verb = true;
			break;
		default:
			usage(*argv);
			exit(10);
			break;
		}
	}

	if (!text_loaded) {
		usage(*argv);
		exit(10);
	}

    if (pcap_file) {
        pcap_t* descr;
        char errbuf[512]={0};
        const char *outfile="out.pcap";
        int npkt=0, i=0;
        struct pcap_pkthdr hdr;
        u_char *pkt=NULL;
        pcap_dumper_t *dumpfile;


        /* if pcap_file is set start a loop for each packet in the pcacp file */
        cout << "starting pcap mode" << endl;
        
        descr = pcap_fopen_offline(pcap_file, errbuf);
        if(descr == NULL) { 
            printf("pcap_fopen_offline(): %s\n",errbuf); 
            exit(17); 
        } 

        /* open pcap dumper file */ 
        dumpfile = pcap_dump_open(descr, outfile);
        if(dumpfile==NULL) {
            cout << "Error opening dump file" <<endl;
            exit(17);
        }

        while ((pkt = (u_char*)pcap_next(descr,&hdr))) {
            data_ptr = data_segment;
            /* process packet */
            npkt++;

            cout << "Processing packet #" << npkt << endl;
            if (hdr.len > 2048) {
                cout << "packet too long (> 2048). Ignoring packet" << endl;
                continue;
            } 

            for (i=0; i<(int)hdr.len; i++)
                printf("%02x ", pkt[i]);
            printf("\n");


            /* store input packet into program data segment */
            for (i=0; i<(int)hdr.len; i+=4) {
				mem.set<uint32_t>(data_ptr, *(uint32_t*)&pkt[i]);
                data_ptr+=4;
            }

            /* dissect packet headers and set program metadata */
            struct sniff_ethernet *eth_p = (struct sniff_ethernet *)pkt;
            struct sniff_ip *ip_p = NULL;
            struct sniff_tcp *tcp_p = NULL;
            struct sniff_udp *udp_p = NULL;
            struct sniff_arp *arp_p = NULL;

            if (eth_p->ether_type == 0x0008) 
                ip_p = (struct sniff_ip *) (pkt + sizeof(struct sniff_ethernet));        

            else if (eth_p->ether_type == 0x0608)
                arp_p = (struct sniff_arp *) (pkt + sizeof(struct sniff_ethernet));        
             

            if (ip_p) {
                if (ip_p->ip_p == 0x11) //UDP
                    udp_p =(struct sniff_udp*)( (uint8_t *)ip_p + IP_HL(ip_p)); 
                else if (ip_p->ip_p == 0x06) //TCP
                    tcp_p = (struct sniff_tcp*)( (uint8_t*)ip_p + IP_HL(ip_p)); 
            }

            data_ptr = data_segment + 2048;
            //set packet len 
            mem.set<uint32_t>(data_ptr, (uint32_t)hdr.len);
            data_ptr += sizeof(uint32_t);
        
            //set src mac
            mem.set<uint32_t>(data_ptr, *(uint32_t*)(eth_p->ether_shost));
            data_ptr += sizeof(uint32_t);
            mem.set<uint16_t>(data_ptr, (uint16_t)eth_p->ether_shost[4]);
            data_ptr += sizeof(uint16_t);

            //set dst mac 
            mem.set<uint32_t>(data_ptr, *(uint32_t*)eth_p->ether_dhost);
            data_ptr += sizeof(uint32_t);
            mem.set<uint16_t>(data_ptr, (uint16_t)eth_p->ether_dhost[4]);
            data_ptr += sizeof(uint16_t);

            //set eth type 2 byte
            mem.set<uint16_t>(data_ptr, (uint16_t)eth_p->ether_type);
            data_ptr += sizeof(uint16_t);

            if (ip_p) {
                //set src ip
                mem.set<uint32_t>(data_ptr, *(uint32_t*)&(ip_p->ip_src));
                data_ptr += sizeof(uint32_t);

                //set dst ip
                mem.set<uint32_t>(data_ptr, *(uint32_t*)&(ip_p->ip_dst));
                data_ptr += sizeof(uint32_t);
        
                //set ip proto  1 byte
                mem.set<uint8_t>(data_ptr, (uint8_t)ip_p->ip_p);
                data_ptr += sizeof(uint8_t);
            }
            else if (arp_p) {
                //set src ip
                mem.set<uint32_t>(data_ptr, *(uint32_t*)&(arp_p->spa));
                data_ptr += sizeof(uint32_t);

                //set dst ip
                mem.set<uint32_t>(data_ptr, *(uint32_t*)&(arp_p->tpa));
                data_ptr += sizeof(uint32_t);
            }

            if (tcp_p) {
                //src port 
                mem.set<uint16_t>(data_ptr, (uint16_t)tcp_p->th_sport);
                data_ptr += sizeof(uint16_t);
 
                //dst port 
                mem.set<uint16_t>(data_ptr, (uint16_t)tcp_p->th_dport);
                data_ptr += sizeof(uint16_t);
            }
            else if (udp_p) {
            
                //src port 
                mem.set<uint16_t>(data_ptr, (uint16_t)udp_p->uh_sport);
                data_ptr += sizeof(uint16_t);

                //dst port 
                mem.set<uint16_t>(data_ptr, (uint16_t)udp_p->uh_dport);
                data_ptr += sizeof(uint16_t);
            }
                
            cout << *argv << ": Starting CPU for packet #" << npkt << endl;
 	        int cc=run_cpu(&mem, verb);
	        if (mem.is_collecting()) mem.display_memory_stats();
	        cout << *argv << "CPU Finished" <<endl; 

            /* read out packet from program data segment and store it into a pcap file */
            data_ptr = data_segment + 2048;
            uint32_t out_len = mem.get<uint32_t>(data_ptr);

            data_ptr = data_segment + 4096;
            for (i=0; i<(int)out_len; i+=4) {
                *(uint32_t*)(&pkt[i]) = mem.get<uint32_t>(data_ptr);
                data_ptr+=4;
            }

            cout << "Packet len " << out_len <<" Dumping output packet to file" <<endl;
            cout << "executed in " << cc << " cc" <<endl;
            hdr.len = out_len;
            
            for (i=0; i<(int)out_len; i++)
                printf("%02x ", pkt[i]);
            printf("\n");
            
            pcap_dump((unsigned char *)dumpfile, &hdr, pkt);
        }
    }
    else {
        /* standard mode */
	    cout << *argv << ": Starting CPU..." << endl;
        /*
 	    run_cpu(&mem, verb);
	    cout << *argv << ": CPU Finished" << endl;
	    if (mem.is_collecting()) mem.display_memory_stats();
        */
    }
}
