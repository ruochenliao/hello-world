/*
* This code "USC CSci551 Projects A and B FA2012" is
* Copyright (C) 2012 by Zi Hu.
* All rights reserved.
*
* This program is released ONLY for the purposes of Fall 2012 CSci551
* students who wish to use it as part of their Project C assignment.
* Use for another other purpose requires prior written approval by
* Zi Hu.
*
* Use in CSci551 is permitted only provided that ALL copyright notices
* are maintained and that this code is distinguished from new
* (student-added) code as much as possible.  We new services to be
* placed in separate (new) files as much as possible.  If you add
* significant code to existing files, identify your new code with
* comments.
*
* As per class assignments, use of any code OTHER than this provided
* code requires explicit approval, ahead of time, by the professor.
*
*/

#include "router.h"


CRouter::CRouter(int stage, int index, struct sockaddr_in paddr, unsigned long ip)
{
    _stage = stage;
    _index = index;
    _cc_seq = 1;
    _rip = ip;
    memset(&cc,0,sizeof(struct circuit));
    memcpy(&_paddr, &paddr, sizeof( struct sockaddr_in));
//    printf("router: proxy port: %d\n", ntohs(_paddr.sin_port));

    memset(_logfn, 0, MAX_FN_LEN);
    sprintf(_logfn,"stage%d.router%d.out",_stage,_index);

}


CRouter::~CRouter()
{
}
//create socket
bool CRouter::initialize_socket()
{
    bool status=true;
    status &= _mysock.create(SOCK_DGRAM,0);
    status &= _mysock.bind(0);
    if(status)
    {
	char out_buf[MAX_BUF_SIZE];

	struct sockaddr_in rip;
	rip.sin_addr.s_addr = _rip;
    	memset(out_buf,0, MAX_BUF_SIZE);
	sprintf(out_buf, "router: %d, pid: %d, port: %d, IP: %s\n",_index, getpid(), get_port(), inet_ntoa(rip.sin_addr));
	FILE* logfp=fopen(_logfn,"w");
    	if(!logfp)
    	{
		printf("Open Log File:%s failed \n",_logfn);
    	}	
	else
	{
		fputs(out_buf, logfp);
    		fclose(logfp);
	}
    }
    return status;
    
}

//create ICMP raw socket
bool CRouter::initialize_rawsocket()
{
    bool status=true;
    status &= _icmp_sock.create(SOCK_RAW,IPPROTO_ICMP);
    if(status)
    {
//	printf("icmp socket create succeed\n");
    }

    in_addr_t src=_rip;
    struct sockaddr_in src_addr;
    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = src;
    
    status &= _icmp_sock.bind_rawsock(src_addr);

    return status;
}


//bind raw ICMP socket to router's IP
bool CRouter::bind_rawsock_src(struct sockaddr_in src)
{
   return( _icmp_sock.bind_rawsock(src));
}


int CRouter::recv_data_UDP(char* recv_buf, struct sockaddr_in & si_other)
{
    int recv_ret=_mysock.recv_data_UDP(recv_buf, si_other);
    return recv_ret;
}

int CRouter::send_data_UDP(const char* send_buf, const int len, struct sockaddr_in & ser_addr)
{
    int send_ret=_mysock.send_data_UDP(send_buf, len, ser_addr);
    return send_ret;
}


int CRouter::send_ICMP_packet(struct sockaddr_in dst_addr)
{
    int send_ret=_icmp_sock.send_icmp_rawsock(dst_addr);
    return send_ret;
}


int CRouter::recv_data_ICMP(char* recv_buf, struct sockaddr_in & ser_addr)
{
    int recv_ret=_icmp_sock.recv_data_RAW(recv_buf, ser_addr);
    return recv_ret;
}
// tcp raw socket
bool CRouter::initialize_tcp_rawsocket(){
    bool status=true;
    status &= _tcp_sock.create(SOCK_RAW,IPPROTO_TCP);
    if(status){
//    	printf("tcp socket create succeed\n");
    }
    in_addr_t src=_rip;
    struct sockaddr_in src_addr;
    src_addr.sin_family = AF_INET;
    src_addr.sin_addr.s_addr = src;
    status &= _tcp_sock.bind_rawsock(src_addr);

    return status;
}


int CRouter::get_port()
{
    return  ntohs((_mysock._addr).sin_port);
}


//handle circuit extend done message
void CRouter::handle_ccext_done_msg(char* buf, int len, struct sockaddr_in si_other)
{

    char log_buf[MAX_BUF_SIZE];
    int nsend;
    struct iphdr *iph = (struct iphdr *)buf;
    struct cc_ext_msg * ccextmsg = (struct cc_ext_msg*)(iph+1);
    print_buf_hex((char*)ccextmsg, len-(sizeof(struct iphdr)), ntohs(si_other.sin_port));
    unsigned short iID = ntohs(ccextmsg->cid);
    memset(log_buf, 0, MAX_BUF_SIZE);
    int i;
    int judge =0;
    int circuit_num = iID & 0x000f;
    for(i=0;i<=_cc_seq+1;i++){
    	if(iID == cc[i]._oid ){
    		judge =1;
    		break;
    		//circuit_num = i;
    	}
    }
//    printf("extend done: incoming id is %x\n",iID);
//    printf("extend done: circuit 2 has oid %x\n",cc[2]._oid);
    if(judge == 1){
//    if(iID == cc._oid){
        //forward the message back along the partial created path.
    	ccextmsg->cid = htons(cc[circuit_num]._iid);
    	struct sockaddr_in prev_hop;
    	prev_hop.sin_family = AF_INET;
    	prev_hop.sin_port = htons(cc[circuit_num]._iport);
    	prev_hop.sin_addr.s_addr =  htonl(INADDR_ANY);

    	nsend = send_data_UDP(buf, len, prev_hop);
        if( nsend <= 0)
        {
	    printf("**Router** %d, failed send circuit message via UDP\n", _index);
        }

        sprintf(log_buf, "forwarding extend-done circuit, incoming: 0x%02x, outgoing: 0x%02x at %d\n",iID, cc[circuit_num]._iid, cc[circuit_num]._iport );
        output_log(log_buf);
    }
    else{
    	printf("**Router** %d, received unknown circuit message via UDP\n", _index);
		sprintf(log_buf, "unknown extend-done circuit: incoming:0x%x, port:%d\n",iID, ntohs(si_other.sin_port));
		output_log(log_buf);
    }
}

//handle relay message
void CRouter::handle_relay_msg(char* buf, int len, struct sockaddr_in si_other)
{
//("0cc[circuit_num]._iport is %u\n",cc[1]._iport);
    char log_buf[MAX_BUF_SIZE];
    char send_buf[MAX_PACKET_SIZE];
    char ssi[MAX_BUF_SIZE];
    char sso[MAX_BUF_SIZE];
    char sdest[MAX_BUF_SIZE];
    //char odest[MAX_BUF_SIZE];
    struct sockaddr_in si,so,dest;//,dest2;
    memset(log_buf, 0, MAX_BUF_SIZE);
    memset(send_buf,0, MAX_PACKET_SIZE);
    int nsend;
    char *p;
    struct iphdr *iph = (struct iphdr *)buf;
    unsigned short last_hop = strtol("0xffff", &p, 16);
    struct cc_relay_msg * ccrelaymsg = (struct cc_relay_msg*)(iph+1);
    print_buf_hex((char*)ccrelaymsg, len-(sizeof(struct iphdr)), ntohs(si_other.sin_port));
    unsigned short iID = ntohs(ccrelaymsg->cid);
    int hlen = sizeof(struct iphdr) + sizeof( struct cc_relay_msg);
    int plen = 0;
    // unknow packets

    int dlen;
	plen = len - hlen;
	char * decrypted_payload = NULL;

	char * clear_payload = new char [plen];
	memset(clear_payload, 0, plen);
	memcpy(clear_payload, buf+hlen, plen);
		
	//decrypt the payload with its key.
	int circuit_num1 = iID & 0x000f;
	decrypt_msg_with_padding(clear_payload, plen, &decrypted_payload, &dlen, aes_key[circuit_num1]);
	memcpy(buf+hlen, decrypted_payload, dlen);
	delete [] decrypted_payload;
	delete [] clear_payload;
	len = hlen + dlen;
	// get the ip addr of the encrypted packet
	struct iphdr *riph = (struct iphdr *)(ccrelaymsg+1);
	si.sin_addr.s_addr = riph->saddr;
	so.sin_addr.s_addr = _rip;
	dest.sin_addr.s_addr = riph->daddr;

	strcpy(ssi, inet_ntoa(si.sin_addr));
	strcpy(sso, inet_ntoa(so.sin_addr));
	strcpy(sdest, inet_ntoa(dest.sin_addr));

    int i;
    int judge =0;
    int circuit_num =iID & 0x000f;
    //printf("handle_relay_msg: circuit_num is %d\n",circuit_num);
    //printf("handle_relay_msg: iID is %02x\n",iID);
    for(i=0;i<=_cc_seq+1;i++){
    	if(iID == cc[i]._iid ){
    		judge =1;
    		break;
    		//circuit_num = i;
    	}
    }
    // if there is no such circuit to send message
    if(judge != 1){
    	perror("relay msg: unable to find such circuit\n");
//    if(iID != cc._iid ){
        for(i=0;i<=_cc_seq;i++){
        	//printf("cc[_cc_seq]._iid is %x",cc[_cc_seq]._iid);
        	printf("_cc_seq is %x",_cc_seq);
        }
    	printf("handle_relay_msg: iID is %x\n",iID);
    	perror("handle_relay_msg: get error cc._iid\n");
//    	printf("**Router** %d, received UNNORMAL relay message via UDP\n", _index);
    	sprintf(log_buf, "unknown incoming circuit: 0x%02x, src: %s, dst: %s\n", iID, ssi, sdest);
    	output_log(log_buf);
    	exit(1);
    }
	//last hop? send out via raw socket.
    if(cc[circuit_num]._oport == last_hop){
    		//remember the src ip address
    		cc[circuit_num]._iip = riph ->saddr;
    		cc[circuit_num]._oip = riph ->daddr;

    		//change the src IP address with the last hop;
    		riph->saddr = _rip;
    		//change tcp checksum
    		riph->check = 0;
    		riph->check = in_cksum((unsigned short*)riph, sizeof(struct iphdr));

    		printf("**Router** %d, reach the final hop, send packet out via raw socket\n",_index);
    		//for ICMP packet
    		//printf("riph -> protocol is %d\n",riph -> protocol);
    	    if(riph -> protocol == 1){
    	    	//print_icmp_packet(buf+hlen, len-hlen);
    	    	//send out the packet via raw socket.
    	    	sprintf(log_buf,"outgoing packet, circuit incoming: 0x%02x, incoming src: %s, outgoing src: %s, dst: %s\n",cc[circuit_num]._iid,ssi,sso,sdest);
    	    	output_log(log_buf);

    	        struct sockaddr_in serv_icmp;
    	        serv_icmp.sin_family = AF_INET;
    	        serv_icmp.sin_addr.s_addr = dest.sin_addr.s_addr;
				nsend = _icmp_sock.send_icmp_raw_sock(serv_icmp,buf+hlen,dlen);
    	        if(nsend<=0){
    	        	//printf("**Router** %d, failed send packet via RAW socket\n", _index);
    	        	perror("**Router** %d, failed send packet via RAW socket\n");
    	        	exit(1);
    	        }
    	    	//handle_proxy_icmp_traffic(buf+hlen, len-hlen, si_other);
    	    }
    	    // for TCP packet
    	    if(riph-> protocol == 6){
        		struct tcphdr * tcp_hdr = (struct tcphdr *)(riph + 1);
        		cc[circuit_num].last_hop_port = tcp_hdr ->dest;
        		sprintf(log_buf, "outgoing TCP packet, circuit incoming: 0x%02x, incoming src IP/port:%s:%d, outgoing src IP/port: %s:%d, dst IP/port: %s:%d, seqno: %u, ackno: %u\n", cc[circuit_num]._iid, ssi,ntohs(tcp_hdr->source) ,sso,cc[circuit_num]._oport,sdest,ntohs(tcp_hdr->dest),ntohl(tcp_hdr->seq),ntohl(tcp_hdr->ack_seq) );
        		output_log(log_buf);

        		tcp_hdr->check = 0;
        		compute_tcp_checksum(riph,(unsigned short *)tcp_hdr);
    	        struct sockaddr_in serv;
    	        serv.sin_family = AF_INET;
    	        serv.sin_addr.s_addr = dest.sin_addr.s_addr;
    	        nsend = _tcp_sock.send_tcp_rawsock(serv,buf+hlen,dlen);

    	        if(nsend<=0){
    	        	//printf("**Router** %d, failed send packet via RAW socket\n", _index);
    	        	perror("**Router** %d, failed send packet via RAW socket\n");
    	        	exit(1);
    	        }

    	    }
    }// end of if(cc._oport == last_hop)
    	// if it is not the last hop, relay the message to the next hop
    else{
    	 	struct sockaddr_in next_hop;
    	    next_hop.sin_family = AF_INET;
    	    next_hop.sin_addr.s_addr =  htonl(INADDR_ANY);
    		ccrelaymsg->cid = htons(cc[circuit_num]._oid);
    	    next_hop.sin_port = htons(cc[circuit_num]._oport);
    	    nsend = send_data_UDP(buf, len, next_hop);
    	    if( nsend <= 0){
    	        	perror("replay packet: fail to send message via UDP\n");
    	    }

    	    sprintf(log_buf, "relay encrypted packet, circuit incoming: 0x%02x, outgoing: 0x%02x\n", cc[circuit_num]._iid, cc[circuit_num]._oid);
	        output_log(log_buf);
    }
}
    //handle relay reply packet
//    if(ccrelaymsg->msg_type == CC_RELAY_BACK_MSGTYPE ||  ccrelaymsg->msg_type == CC_ENCRYPTED_RELAY_REPLY)
void CRouter::handle_relay_reply_msg(char* buf, int len, struct sockaddr_in si_other){
//   if(ccrelaymsg->msg_type == CC_ENCRYPTED_RELAY_REPLY)
		char log_buf[MAX_BUF_SIZE];
		memset(log_buf, 0, MAX_BUF_SIZE);
	    struct iphdr *iph = (struct iphdr *)buf;
	    struct cc_relay_msg * ccrelaymsg = (struct cc_relay_msg*)(iph+1);
	    print_buf_hex((char*)ccrelaymsg, len-(sizeof(struct iphdr)), ntohs(si_other.sin_port));
		unsigned short iID = ntohs(ccrelaymsg->cid);

    	int hlen = sizeof(struct iphdr) + sizeof( struct cc_relay_msg);
		int plen = len - hlen;
		char * encrypted_payload = NULL; 
		int olen;
		char * clear_payload = new char [plen];
		memset(clear_payload, 0, plen);
		memcpy(clear_payload, buf+hlen, plen);
		//encrypt the payload with its key.
		int circuit_num = iID & 0x000f;
		encrypt_msg_with_padding(clear_payload, plen, &encrypted_payload, &olen, aes_key[circuit_num]);
		memcpy(buf+hlen, encrypted_payload, olen);
	    int i;
	    int judge =0;
	    //int circuit_num =0;
	    for(i=0;i<=_cc_seq+1;i++){
	    	if(iID == cc[i]._oid ){
	    		judge =1;
	    		break;
	    		//circuit_num = i;
	    	}
	    }
	    //printf("relay reply: circuit 2 _cc_seq = %d\n",_cc_seq);
	    //printf("relay reply: circuit 2 incoming id is %x\n",iID);
	    //printf("relay reply: circuit 2 oid is %x\n",cc[2]._oid);

		sprintf(log_buf, "relay reply encrypted packet, circuit incoming: 0x%02x, outgoing: 0x%02x\n", iID, cc[circuit_num]._iid);
		output_log(log_buf);
		delete [] encrypted_payload;
		delete [] clear_payload;
		len = hlen + olen;
		if(judge != 1){
//    	if (iID != cc._oid){
			perror("relay reply: unable find such circuit\n");
        	printf("**Router** %d, received UNNORMAL relay message via UDP\n", _index);
        	//sprintf(log_buf, "unknown incoming circuit: 0x%02x, src: %s, dst: %s\n", iID, ssi, sdest);
        	sprintf(log_buf, "unknown incoming circuit: 0x%02x\n", iID);
    	    output_log(log_buf);
    	    exit(1);
    	}

    	struct sockaddr_in next_hop;
    	next_hop.sin_family = AF_INET;
    	next_hop.sin_addr.s_addr =  htonl(INADDR_ANY);
	    ccrelaymsg->cid = htons(cc[circuit_num]._iid);
	    next_hop.sin_port = htons(cc[circuit_num]._iport);
       	int nsend = send_data_UDP(buf, len, next_hop);
    	if( nsend <= 0){
    	   printf("**Router** %d, failed send relay message via UDP\n", _index);
    	}
	    //output_log(log_buf);

}


//handle deffie hellman message
void CRouter::handle_deffie_hellman_msg(char* buf, int len, struct sockaddr_in si_other)
{
    char log_buf[MAX_BUF_SIZE];
    char send_buf[MAX_PACKET_SIZE];
    memset(send_buf,0, MAX_PACKET_SIZE);
    int nsend;

    struct iphdr *iph = (struct iphdr *)buf;
    struct cc_deffie_hellman_msg * cc_dh_msg = (struct cc_deffie_hellman_msg*)(iph+1);

    print_buf_hex((char*)cc_dh_msg, len-(sizeof(struct iphdr)), ntohs(si_other.sin_port));
    unsigned short iID = ntohs(cc_dh_msg->cid);
    int hlen = sizeof( struct iphdr) + sizeof( struct cc_deffie_hellman_msg);
    int plen = len - hlen;
    //_cc_seq = iID & 0x000f;
    int circuit_num =  iID & 0x000f;
    circuit_num_current = circuit_num;
    unsigned short oID = compute_circuit_id(_index, circuit_num);
    //unsigned short oID = compute_circuit_id(_index, _cc_seq);
    int i;
    int judge =0;
    //int circuit_num =0;
    for(i=0;i<=_cc_seq+1;i++){
    	//if(iID == cc[_cc_seq]._iid ){
    	if(iID == cc[i]._iid ){
    		judge =1;
    		//circuit_num = i;
    	}
    }
    // This key is not for this router, because the backup has the iID already, so relay this hello to next hop
//    if(iID == cc._iid )
    if(circuit_num == 2){
    	printf("hello: circuit 2 has iID %d\n",cc[2]._iid);
    }
//    for(i=0;i<=_cc_seq && circuit_num == 2;i++){
    	//if(iID == cc[_cc_seq]._iid ){
    	//	printf("router: cc[i]._iid is %x\n",cc[i]._iid);
    	//	printf("router: iID is %d\n",iID);
//    }
    // the router has the iID, forward this message
    if(judge == 1){
        //decrypt the key, forward the message along the partial created path.
    	cc_dh_msg->cid = htons(oID);
    	char* decrypted_key = NULL;
    	int klen;
    	decrypt_msg_with_padding(buf+ hlen, plen, &decrypted_key, &klen, aes_key[circuit_num]);
    	memcpy(buf+hlen, decrypted_key, klen);
    	delete [] decrypted_key;
    	len = hlen+klen;

    	struct sockaddr_in next_hop;
    	next_hop.sin_family = AF_INET;
    	next_hop.sin_port = htons(cc[circuit_num]._oport);
    	next_hop.sin_addr.s_addr =  htonl(INADDR_ANY);
    	nsend = send_data_UDP(buf, len, next_hop);
        if( nsend <= 0){
        	printf("**Router** %d, failed send circuit message via UDP\n", _index);
        }

        //log
        memset(log_buf, 0, MAX_BUF_SIZE);
        char key_hex_buf[MAX_BUF_SIZE];
        memset(key_hex_buf, 0, MAX_BUF_SIZE);
        int key_buf_len = key_to_hex_buf((unsigned char*)buf+hlen, key_hex_buf,plen);
    	int index = sprintf(log_buf, "fake-diffie-hellman, forwarding,  circuit incoming: 0x%02x, key: 0x",iID);
    	memcpy(log_buf+index, key_hex_buf, key_buf_len);
    	output_log(log_buf);
    }
    //The backup doesn't has the iID, it is the destination of this message, just remember the key
    else{
    	//printf("router: new circuit _cc_seq is %d\n",_cc_seq);

    	memcpy(aes_key[circuit_num], buf+hlen, plen);
    	char key_hex_buf[MAX_BUF_SIZE];
    	memset(key_hex_buf, 0, MAX_BUF_SIZE);
    	memset(log_buf, 0, MAX_BUF_SIZE);
    	int key_buf_len = key_to_hex_buf((unsigned char*)buf+hlen, key_hex_buf,plen);
    	int index = sprintf(log_buf, "fake-diffie-hellman, new circuit incoming: 0x%02x, key: 0x",iID);
    	memcpy(log_buf+index, key_hex_buf, key_buf_len);
    	output_log(log_buf);
    }

}



//handle circuit extend message
void CRouter::handle_ccext_msg(char* buf, int len, struct sockaddr_in si_other)
{
    char log_buf[MAX_BUF_SIZE];
    char send_buf[MAX_PACKET_SIZE];
    memset(send_buf,0, MAX_PACKET_SIZE);
    int nsend;
    struct iphdr *iph = (struct iphdr *)buf;
    struct cc_ext_msg * ccextmsg = (struct cc_ext_msg*)(iph+1);
    print_buf_hex((char*)ccextmsg, len-(sizeof(struct iphdr)), ntohs(si_other.sin_port));
    unsigned short iID = ntohs(ccextmsg->cid);
    //printf("iID is %d\n",iID);
    unsigned short next_port;
    //compute it's own circuit ID;
    //_cc_seq = iID & 0x000f;
    int circuit_num= iID & 0x000f;
    unsigned short oID = compute_circuit_id(_index, circuit_num);
    //unsigned short oID = compute_circuit_id(_index, _cc_seq);
    //header length (IP header + circuit extend header)
    int hlen = sizeof(struct iphdr) +sizeof( struct cc_encrypt_ext_msg );
    // payload length
    int plen = len - hlen;

    int i;
    int judge =0;
    //int circuit_num =0;
    for(i=0;i<=_cc_seq+1;i++){
    	if(iID == cc[i]._iid ){
    		judge =1;
    		//circuit_num = i;
    	}
    }
//    if(iID == cc._iid )
    //if found this circuit exist,relay this extend message
    if(judge == 1){
//    	if(_stage >=6){
	   
    	//decrypt the port number in the circuit extend message
    	char* decrypted_port = NULL;
    	int elen;
    	decrypt_msg_with_padding(buf+ hlen, plen, &decrypted_port, &elen, aes_key[circuit_num]);
    	memcpy(buf+hlen, decrypted_port, elen);
    	delete [] decrypted_port;
    	len = hlen + elen;
//    	}
        //forward the message along the partial created path.
    	ccextmsg->cid = htons(oID);
    	//printf("the oID original is %x",oID);
    	struct sockaddr_in next_hop;
    	next_hop.sin_family = AF_INET;
    	next_hop.sin_port = htons(cc[circuit_num]._oport);
    	next_hop.sin_addr.s_addr =  htonl(INADDR_ANY);
    	nsend = send_data_UDP(buf, len, next_hop);
        if( nsend <= 0){
        	printf("**Router** %d, failed send circuit message via UDP\n", _index);
        }

        memset(log_buf, 0, MAX_BUF_SIZE);
        sprintf(log_buf, "forwarding extend circuit: incoming: 0x%02x, outgoing: 0x%02x at %d\n",cc[circuit_num]._iid, cc[circuit_num]._oid, cc[circuit_num]._oport );
        output_log(log_buf);
    }
    // if router table doesn't contain this circuit, it is a new circuit, send extend done back
    else{
    	char * decrypted_port = NULL;
    	int elen;
    	//decrypt the port number in the circuit extend message
    	decrypt_msg_with_padding(buf+ hlen, plen, &decrypted_port, &elen, aes_key[circuit_num]);
    	//get the port number (string to integer);
    	char * tport = new char [elen];
    	memset(tport, 0, elen);
    	memcpy(tport, decrypted_port, elen);
    	next_port = ntohs(atoi(tport));
    	delete [] tport;
    	delete [] decrypted_port;
    	//printf("**Router** %d, decrypted next hop:%d\n", _index, next_port);
//    	}
    	//remember this circuit and send circuit extend done msg back;
        if(cc[circuit_num]._iid == 0){
        	cc[circuit_num]._iid = iID;
        	cc[circuit_num]._oid = oID;
        	//printf("oID is %x\n",oID);
        	cc[circuit_num]._iport = ntohs(si_other.sin_port);
        	cc[circuit_num]._oport = next_port;
        	cc[circuit_num].seq_num = circuit_num;
        }
        if(circuit_num == 1){
        	printf("cctex: circuit 1 has iID %d\n",cc[1]._iid);
        }
        if(circuit_num == 2){
        	printf("cctex: circuit 2 has iID %d\n",cc[2]._iid);
        }
        //_cc_seq++;
        memcpy(send_buf, buf, len);
        iph = (struct iphdr *)send_buf;
        struct cc_ext_done_msg * ccextdonemsg = (struct cc_ext_done_msg *)(iph+1); 
        //different message type for stage 5 and 6
        if(_stage == 5)
        	ccextdonemsg->msg_type =  CC_EXT_DONE_MSGTYPE;
        else
        	ccextdonemsg->msg_type = CC_ENCRYPTED_EXT_DONE;

        int packet_len = sizeof(struct iphdr) + sizeof(struct cc_ext_done_msg) ;
        nsend = send_data_UDP(send_buf, packet_len, si_other);
        if( nsend <= 0){
        	printf("**Router** %d, failed send circuit message via UDP\n", _index);
        }

        memset(log_buf, 0, MAX_BUF_SIZE);
//        sprintf(log_buf, "new extend circuit: incoming: 0x%02x, outgoing: 0x%02x at %d\n",cc[_cc_seq-1]._iid, cc[_cc_seq-1]._oid, cc[_cc_seq-1]._oport );
        sprintf(log_buf, "new extend circuit: incoming: 0x%02x, outgoing: 0x%02x at %d\n",cc[circuit_num]._iid, cc[circuit_num]._oid, cc[circuit_num]._oport );
        output_log(log_buf);
    }
    	
}

//this function was wrote for stage 3, but it is actually no needed anymore.
void CRouter::self_reply_icmp(char* buf, int len, struct sockaddr_in si_other)
{

    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)buf;
    iphdrlen = iph->ihl*4;
    struct icmphdr *icmph = (struct icmphdr *)(buf + iphdrlen);


    char log_buf[MAX_BUF_SIZE];
    struct sockaddr_in source,dest;
    char src_addr_buf[MAX_BUF_SIZE];
    char dst_addr_buf[MAX_BUF_SIZE];

    /* check if it is an ICMP packet */
    if(iph->protocol == 1)
    {

	printf("router:%d, self reply\n",_index);
	source.sin_addr.s_addr = iph->saddr;
	dest.sin_addr.s_addr = iph->daddr;
	memset(log_buf, 0, MAX_BUF_SIZE);
	memset(src_addr_buf, 0, MAX_BUF_SIZE);
	memset(dst_addr_buf, 0, MAX_BUF_SIZE);
	strcpy(src_addr_buf, inet_ntoa(source.sin_addr));
	strcpy(dst_addr_buf, inet_ntoa(dest.sin_addr));

	sprintf(log_buf, "ICMP from port: %d, src: %s, dst: %s, type: %d\n",ntohs(si_other.sin_port), src_addr_buf, dst_addr_buf, icmph->type);
	output_log(log_buf);
	   
	/* exchange src and dst */
	source.sin_addr.s_addr = iph->saddr;
	dest.sin_addr.s_addr = iph->daddr;
	iph->saddr = dest.sin_addr.s_addr;
	iph->daddr = source.sin_addr.s_addr;


	/* recompute ip header checksum */
	iph->check=0;
	unsigned short checksum = in_cksum((unsigned short*)iph, sizeof(struct iphdr));
	iph->check=checksum;


	/* change type to 0: icmp echo-reply */
	icmph->type = (unsigned int)0;

	/* recompute icmp header check sum */
	icmph->checksum=0;
	checksum = in_cksum((unsigned short*)icmph, sizeof(struct icmphdr));
	icmph->checksum=checksum;
	   

	/*send the packet back to proxy. */ 
	int nsend = send_data_UDP(buf, len, si_other);
	if( nsend <= 0)
	{
	    printf("**Router** %d,self_reply_icmp failed send packet via UDP\n", _index);
	}
    }
    else
    {
	printf("**Router** %d, received unknown packet\n", _index);

    }

}

//handle icmp packet from proxy
void CRouter::handle_proxy_icmp_traffic(char* buf, int len, struct sockaddr_in si_other)
{
	//print_icmp_packet(buf,len);
    char log_buf[MAX_BUF_SIZE];
    struct sockaddr_in source,dest;
    char src_addr_buf[MAX_BUF_SIZE];
    char dst_addr_buf[MAX_BUF_SIZE];
    int nsend;
    memcpy(packet_buf, buf,len);
    packet_len = len;
    //print_icmp_packet(recv_buf,nread);
    unsigned short iphdrlen;
    struct iphdr *iph = (struct iphdr *)buf;
    iphdrlen = iph->ihl*4;
    struct icmphdr *icmph = (struct icmphdr *)(buf + iphdrlen);
    source.sin_addr.s_addr = iph->saddr;
    dest.sin_addr.s_addr = iph->daddr;
    memset(log_buf, 0, MAX_BUF_SIZE);
    memset(src_addr_buf, 0, MAX_BUF_SIZE);
    memset(dst_addr_buf, 0, MAX_BUF_SIZE);
    strcpy(src_addr_buf, inet_ntoa(source.sin_addr));
    strcpy(dst_addr_buf, inet_ntoa(dest.sin_addr));

    if(_stage<5)
    {
    	sprintf(log_buf, "ICMP from port: %d, src: %s, dst: %s, type: %d\n",ntohs(si_other.sin_port), src_addr_buf, dst_addr_buf, icmph->type);
    	output_log(log_buf);
    }
    //send out the icmp packet through raw socket
    struct sockaddr_in serv;
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = dest.sin_addr.s_addr;
    nsend = send_ICMP_packet(serv);
    if(nsend<=0)
    {
	printf("**Router** %d, failed send packet via RAW socket\n", _index);
    }
}

//handle tcp packet from raw socket
void CRouter::handle_rawsock_tcp_traffic(char *buf, int len){

    char log_buf[MAX_BUF_SIZE];
    struct sockaddr_in source,dest;
    char src_addr_buf[MAX_BUF_SIZE];
    char dst_addr_buf[MAX_BUF_SIZE];
    int nsend = 0;

    struct iphdr *iph = (struct iphdr *)buf;
    struct tcphdr * tcp_hdr =(struct tcphdr*)(iph + 1);
    //look for the corresponding circuit
    int i;
    int judge =0;
    int circuit_num =0;
    for(i=0;i<=_cc_seq;i++){
    	if(iph->saddr == cc[i]._oip && tcp_hdr->source ==cc[i].last_hop_port){
    		circuit_num = i;
    		//printf("i is %d\n",i);
    		judge =1;
    		break;
    	}
    }
    if(judge != 1) {
    	perror("handle_rawsock_tcp_traffic: not found the return circuit_num\n");
    	printf("src port number: %u\n",ntohs(tcp_hdr->source));
    	printf("last hop stored is %u\n",ntohs(cc[2].last_hop_port ));
    	struct in_addr x;
    	struct in_addr y;
    	struct in_addr z;
    	x.s_addr = iph->saddr;
    	y.s_addr = iph->daddr;
    	z.s_addr = cc[2]._oip;
    	printf("cc[2] oip is %s\n",inet_ntoa(z));
    	printf("src ip is %s, ",inet_ntoa(x));
    	printf("dst ip is %s\n",inet_ntoa(y));
    	printf("src ip is %u,dst is %u\n",iph->saddr, iph->daddr);
    	return;
    }

    //    print_tcp_packet(buf,len);

//    struct iphdr * original_iphdr = (struct iphdr*)	packet_buf;

    source.sin_addr.s_addr = iph->saddr;
    dest.sin_addr.s_addr = iph->daddr;
    memset(log_buf, 0, MAX_BUF_SIZE);
    memset(src_addr_buf, 0, MAX_BUF_SIZE);
    memset(dst_addr_buf, 0, MAX_BUF_SIZE);
    strcpy(src_addr_buf, inet_ntoa(source.sin_addr));
    strcpy(dst_addr_buf, inet_ntoa(dest.sin_addr));

	sprintf(log_buf,"incoming TCP packet, src IP/port: %s:%d, dst IP/port: %s:%d, seqno: %u, ackno: %u, outgoing circuit: 0x%02x\n",src_addr_buf,ntohs(tcp_hdr->source),dst_addr_buf,ntohs(tcp_hdr->dest),ntohl(tcp_hdr->seq),ntohl(tcp_hdr->ack_seq),cc[circuit_num]._iid);
    output_log(log_buf);

//    int old_plen = packet_len;
	char send_buf[MAX_BUF_SIZE];
	memset(send_buf, 0, MAX_BUF_SIZE);
	int new_packet_len = 0;

	 // if stage >=6//
	//zero the dst ip;
//	iph->daddr = htonl(0);
	//recompute checksume;
	iph -> check = 0;
	unsigned short checksum = in_cksum((unsigned short*)iph, sizeof(struct iphdr));
	iph->check=checksum;
	compute_tcp_checksum(iph,(unsigned short *)tcp_hdr);
	//encrpyt packet
	char * encrypted_payload = NULL;
	int olen;
	//encrypt the packet
//	char * clear_payload = new char [old_plen];
	char * clear_payload = new char [len];
//	memset(clear_payload, 0, old_plen);
	memset(clear_payload,0,len);
//	memcpy(clear_payload, buf, old_plen);
	memcpy(clear_payload,buf,len);
//	encrypt_msg_with_padding(clear_payload, old_plen, &encrypted_payload, &olen, aes_key);
	encrypt_msg_with_padding(clear_payload, len, &encrypted_payload, &olen, aes_key[circuit_num]);
	memcpy(buf, encrypted_payload, olen);
	//construct the encrypted relay message.
	new_packet_len  = construct_relay_msg(send_buf, MAX_PACKET_SIZE, cc[circuit_num]._iid, buf, olen, CC_ENCRYPTED_RELAY_REPLY, _stage);
//	printf("cc[circuit_num]._iid is %x\n",cc[circuit_num]._iid);
	delete [] encrypted_payload;
	delete [] clear_payload;
	struct sockaddr_in next_hop;
	next_hop.sin_family = AF_INET;
	next_hop.sin_port = htons(cc[circuit_num]._iport);
	//next_hop.sin_port = cc[circuit_num]._iport;
	//printf("next_hop.sin_port is %u\n",ntohs(next_hop.sin_port));
	next_hop.sin_addr.s_addr =  htonl(INADDR_ANY);

	//printf("you arrives here\n");
	nsend = send_data_UDP(send_buf, new_packet_len , next_hop);
    if( nsend <= 0){
    	printf("**Router** %d,handle_rawsock_tcp__traffic failed send packet via UDP\n", _index);
    }

}
void CRouter::raw_mit_to_tunnle(char buf[], int nread){

    struct iphdr *iph = (struct iphdr *)buf;
    //struct icmphdr *icmph = (struct icmphdr *)(iph+1);
    //encrypt the packet
    char * encrypted_payload = NULL;
    int olen;
    //encrypt the packet
    char * clear_payload = new char [nread];
    memset(clear_payload, 0, nread);
    char reply_packet[MAX_PACKET_SIZE];
    memset(reply_packet,0, MAX_PACKET_SIZE);
    memcpy(clear_payload, buf, nread);
    int i;
    int judge =0;
    int circuit_num = 0;
    for(i=0;i<=_cc_seq;i++){
    	if(iph->saddr == cc[i]._oip ){
    		circuit_num = i;
    		judge =1;
    	}
    }
    if(judge != 1) perror("handle_rawsock_icmp_traffic: not found the return circuit_num\n");
    encrypt_msg_with_padding(clear_payload, nread, &encrypted_payload, &olen, aes_key[circuit_num]);
    //print_icmp_packet(buf,len);
    memcpy(reply_packet, encrypted_payload, olen);
    //construct the encrypted relay message.
    char send_buf[MAX_BUF_SIZE];
    memset(send_buf, 0, MAX_BUF_SIZE);
    int new_packet_len  = construct_relay_msg(send_buf, MAX_PACKET_SIZE, cc[circuit_num]._iid, reply_packet, olen, CC_ENCRYPTED_RELAY_REPLY, _stage);
	delete [] encrypted_payload;
	delete [] clear_payload;
	printf("mit to proxy\n");
	struct sockaddr_in next_hop;
	next_hop.sin_family = AF_INET;
	next_hop.sin_port = htons(cc[circuit_num]._iport);
	next_hop.sin_addr.s_addr =  htonl(INADDR_ANY);
	printf("next_hop.sin_port is %u\n",ntohs(next_hop.sin_port) );


	int nsend = send_data_UDP(send_buf, new_packet_len , next_hop);
	char log_buf[MAX_BUF_SIZE];
	char src_addr_buf[MAX_BUF_SIZE];
	char dst_addr_buf[MAX_BUF_SIZE];

	struct sockaddr_in source,dest;
    source.sin_addr.s_addr = iph->saddr;
    dest.sin_addr.s_addr = iph->daddr;
    memset(log_buf, 0, MAX_BUF_SIZE);
    memset(src_addr_buf, 0, MAX_BUF_SIZE);
    memset(dst_addr_buf, 0, MAX_BUF_SIZE);
    strcpy(src_addr_buf, inet_ntoa(source.sin_addr));
    strcpy(dst_addr_buf, inet_ntoa(dest.sin_addr));
	//memset(dst_addr_buf,0,sizeof(dst_addr_buf));
	sprintf(log_buf, "incoming packet, src:%s, dst: %s, outgoing circuit: 0x%02x\n", src_addr_buf, dst_addr_buf, cc[circuit_num]._iid);
    output_log(log_buf);
    if( nsend <= 0){
    	printf("**Router** %d,handle_rawsock_icmp_traffic failed send packet via UDP\n", _index);
    }
}
//handle icmp packet from raw socket
void CRouter::handle_rawsock_icmp_traffic(char* buf, int len){
    char log_buf[MAX_BUF_SIZE];
    struct sockaddr_in source,dest;
    char src_addr_buf[MAX_BUF_SIZE];
    char dst_addr_buf[MAX_BUF_SIZE];
    int nsend;

    struct iphdr *iph = (struct iphdr *)buf;
    //struct icmphdr *icmph = (struct icmphdr *)(iph+1);
    //icmp look for the corresponding circuit
    int i;
    int judge =0;
    int circuit_num = 0;
    for(i=0;i<=_cc_seq;i++){
    	if(iph->saddr == cc[i]._oip ){
    		circuit_num = i;
    		judge =1;
    	}
    }
    if(judge != 1) perror("handle_rawsock_icmp_traffic: not found the return circuit_num\n");

    //print_icmp_packet(buf,len);
    char reply_packet[MAX_PACKET_SIZE];
    memset(reply_packet,0, MAX_PACKET_SIZE);
		 		 
    //copy the original icmp packet;
    memcpy(reply_packet, packet_buf, MAX_PACKET_SIZE);
    struct iphdr *reply_iph = (struct iphdr *)reply_packet;
    struct icmphdr *reply_icmph = (struct icmphdr *)(reply_iph+1);

    source.sin_addr.s_addr = iph->saddr;
    dest.sin_addr.s_addr = iph->daddr;
    memset(log_buf, 0, MAX_BUF_SIZE);
    memset(src_addr_buf, 0, MAX_BUF_SIZE);
    memset(dst_addr_buf, 0, MAX_BUF_SIZE);
    strcpy(src_addr_buf, inet_ntoa(source.sin_addr));
    strcpy(dst_addr_buf, inet_ntoa(dest.sin_addr));

    reply_iph->daddr = reply_iph->saddr; 
    reply_iph->saddr = iph->saddr;

    reply_iph->check=0;
    reply_iph->check=in_cksum((unsigned short*)reply_iph, sizeof(struct iphdr));
/*
    iph -> daddr = htonl(0);
    iph->check = 0;
    iph->check = in_cksum((unsigned short*)iph, sizeof(struct iphdr));
    //iph->daddr = reply_iph ->saddr;
    iph->check=0;
    unsigned short checksum = in_cksum((unsigned short*)iph, sizeof(struct iphdr));
    iph->check=checksum;
*/
    reply_iph ->frag_off = iph->frag_off;
    reply_iph ->tot_len = iph ->tot_len;

    reply_icmph->type = (unsigned int)0;
    reply_icmph->checksum=0;
    unsigned short checksum = in_cksum((unsigned short*)reply_icmph, sizeof(struct icmphdr));
    reply_icmph->checksum=checksum;
    int old_plen = packet_len; 
    char send_buf[MAX_BUF_SIZE];
    memset(send_buf, 0, MAX_BUF_SIZE);
    int new_packet_len = 0;

    //zero the dst ip;
    reply_iph->daddr = htonl(0);

    //recompute checksume;
    reply_iph->check = 0;
    reply_iph->check = in_cksum((unsigned short*)reply_iph, sizeof(struct iphdr));
    //encrypt the packet
    char * encrypted_payload = NULL;
    int olen;
    //encrypt the packet
    char * clear_payload = new char [old_plen];
    memset(clear_payload, 0, old_plen);
    memcpy(clear_payload, reply_packet, old_plen);


    encrypt_msg_with_padding(clear_payload, old_plen, &encrypted_payload, &olen, aes_key[circuit_num]);
    //print_icmp_packet(buf,len);
    memcpy(reply_packet, encrypted_payload, olen);
    //construct the encrypted relay message.
    new_packet_len  = construct_relay_msg(send_buf, MAX_PACKET_SIZE, cc[circuit_num]._iid, reply_packet, olen, CC_ENCRYPTED_RELAY_REPLY, _stage);
	delete [] encrypted_payload;
	delete [] clear_payload;
//		}
	struct sockaddr_in next_hop;
	next_hop.sin_family = AF_INET;
	next_hop.sin_port = htons(cc[circuit_num]._iport);
	next_hop.sin_addr.s_addr =  htonl(INADDR_ANY);
	printf("next_hop.sin_port is %u\n",ntohs(next_hop.sin_port) );
	nsend = send_data_UDP(send_buf, new_packet_len , next_hop);

	sprintf(log_buf, "incoming packet, src:%s, dst: %s, outgoing circuit: 0x%02x\n", src_addr_buf, dst_addr_buf, cc[circuit_num]._iid);
    output_log(log_buf);
    if( nsend <= 0){
    	printf("**Router** %d,handle_rawsock_icmp_traffic failed send packet via UDP\n", _index);
    }
}

void CRouter::run()
{
  /* use select() to handle three descriptors at once */
  int maxfd =  _mysock._sock > _icmp_sock._sock? _mysock._sock: _icmp_sock._sock;
  maxfd = maxfd > _tcp_sock._sock ? maxfd:_tcp_sock._sock;

  char recv_buf[MAX_PACKET_SIZE];
  int nread;
  memset(cc,0,sizeof(cc));
  _cc_seq = 1;
  char log_x[128];
  struct timeval *timer;
  timer= (struct timeval *)malloc(sizeof(struct timeval));
  int start_count =0;
  memset(timer,0,sizeof(struct timeval));
  while(1) {
    int ret;
    fd_set rd_set;

    FD_ZERO(&rd_set);
    FD_SET(_mysock._sock, &rd_set);
    FD_SET(_icmp_sock._sock, &rd_set);
    FD_SET(_tcp_sock._sock, &rd_set);
 /*
    if(_stage !=9){
    	ret = select(maxfd + 1, &rd_set, NULL, NULL, NULL);
    }
*/
 //   if(_stage == 9){
    	//if(start_count == 0){
    		//timer->tv_sec = 1200;
    	//}
    	//else{
    	//}
    {
    	if(start_count == 1){
    		timer->tv_sec =5;
    		timer->tv_usec=0;
    	}
    	if(start_count ==0){
    		timer->tv_sec = 1200;
    		timer->tv_usec=0;
    	}
    		//printf("_stage is %d\n",_stage);
        ret = select(maxfd + 1, &rd_set, NULL, NULL, timer);
        if (ret == 0 &&_stage ==9) {
        	printf("ret is smaller than 0?\n");
        	char buf_x[128];
        	char log_x[128];
        	char self_next_x[128];
        	memset(log_x,0,sizeof(log_x));
        	memset(buf_x,0,sizeof(buf_x));
        	memset(self_next_x,0,sizeof(self_next_x));
        	struct self_next * x= (struct self_next *)self_next_x;
			memset(x,0,sizeof(struct self_next));
			//construct kill msg to proxy
        	x ->self_name = get_port();	// host addr
        	x ->next_name = cc[circuit_num_current]._oport; // host addr
        	sprintf(log_x,"router %u worried about %u on citcuit 0x%02x\n",x->self_name,x->next_name,cc[circuit_num_current]._oid);
        	output_log(log_x);
        	construct_relay_msg(buf_x, sizeof(buf_x), cc[circuit_num_current]._iid, (char*)x, sizeof(struct self_next), 0x92, 9);
        	//send to proxy
        	struct sockaddr_in next_hop_x;
        	next_hop_x.sin_family = AF_INET;
        	next_hop_x.sin_addr.s_addr =  htonl(INADDR_ANY);
    	    next_hop_x.sin_port = htons(cc[circuit_num_current]._iport);
    	    //printf("kill command is begin to send out\n");
           	int nsend = send_data_UDP(buf_x, sizeof(buf_x), next_hop_x);
        	if( nsend <= 0){
        	   perror("router respond time out message to proxy\n");
        	}
        	start_count =0;
        }
    }

    if (ret < 0 && errno == EINTR){
      continue;
    }

    if (ret < 0) {
      perror("select()");
      exit(1);
    }

    //icmp data from raw socket
    if(FD_ISSET(_icmp_sock._sock, &rd_set)) {
    	start_count =0;
    	memset(recv_buf,0,MAX_PACKET_SIZE);
    	struct sockaddr_in si_other;
    	nread=recv_data_ICMP(recv_buf,si_other);

    	if(nread > 0){
    		printf("**Router** %d, PID: %d, ICMP from raw socket, length: %d\n", _index, getpid(), nread);
    		struct iphdr *iph = (struct iphdr *)recv_buf;
    		if(iph->protocol == 1){
    			printf("nread is %d\n",nread);
    			//print_icmp_packet(recv_buf,nread);
    			raw_mit_to_tunnle(recv_buf, nread);
    			//handle_rawsock_icmp_traffic(recv_buf, nread);
    		}
    	}
    }
    // data from tcp raw sock
    if(FD_ISSET(_tcp_sock._sock, &rd_set)){
    	start_count =0;
    	printf("data from tcp raw sock\n");
    	memset(recv_buf,0,MAX_PACKET_SIZE);
    	struct sockaddr_in addr_other;
    	nread= _tcp_sock.recv_data_UDP(recv_buf,addr_other);

    	if(nread > 0){
    		//printf("**Router** %d, PID: %d, tcp from raw socket, length: %d\n", _index, getpid(), nread);
    		struct iphdr *iph = (struct iphdr *)recv_buf;
    		if(iph->protocol == 6){
    			//printf("\n\nget response from mit!!!\n\n");
    			handle_rawsock_tcp_traffic(recv_buf, nread);
    			//while(1);
    		}
    	}
    }
    // data from UDP socket, means it is from tunnel or from relay message
    if(FD_ISSET(_mysock._sock, &rd_set)) 
    {
    	memset(recv_buf,0,MAX_PACKET_SIZE);
    	struct sockaddr_in si_other;
    	//printf("hello world0\n");
    	nread = recv_data_UDP(recv_buf, si_other);
    	//printf("hello world1\n");
    	if(nread != 0) 
    	{
    	    struct iphdr *iph = (struct iphdr *)recv_buf;
    	    struct cc_relay_msg *x = (struct cc_relay_msg *)recv_buf;
    	    if(x ->msg_type == 0x91&& _stage ==9){
    	    	printf("msg_type is 0x91\n");
    	    	memset(log_x,0,sizeof(log_x));
    	    	sprintf(log_x,"router %d killed\n",_index);
    	    	output_log(log_x);
    	    	exit(1);
    	    }
	    /* check if it is an ICMP packet */
	    if(iph->protocol == 1){
	    	printf("Let's ICMP!!!\n big error\n");
//	    	printf("**Router** %d, PID: %d, ICMP from port: %d, length: %d\n", _index, getpid(), ntohs(si_other.sin_port), nread);
	    	//if(iph->daddr == _rip)
	    	//	self_reply_icmp(recv_buf,nread,si_other);
	    	//else
	    		handle_proxy_icmp_traffic(recv_buf,nread,si_other);
	    }
	    // circuit extend message 
	    else if (iph->protocol == CC_EXT_PROTOCOL){
//	    	printf("**Router** %d, PID: %d, CIRCUIT Msg from port: %d, length: %d\n", _index, getpid(), ntohs(si_other.sin_port), nread);
//	    	printf("I entered the CC_EXIT_PROTOCOL\n");
	    	struct cc_ext_msg * ccextmsg = (struct cc_ext_msg*)(iph+1);
    		if(_cc_seq < (  ntohs(ccextmsg ->cid) & 0x000f) ){
    			_cc_seq = (   ntohs(ccextmsg ->cid) & 0x000f);
    			printf("hello: _cc_seq is %d\n",_cc_seq);
    		}
	    	if(ccextmsg->msg_type == FAKE_DIFFIE_HELLMAN)
	    	{
	    		handle_deffie_hellman_msg(recv_buf,nread,si_other);
	    	}
	    	if(ccextmsg->msg_type == CC_EXT_MSGTYPE || ccextmsg->msg_type == CC_ENCRYPTED_EXT)
	    	{
	    		//circuit extend message
	    		handle_ccext_msg(recv_buf,nread,si_other);
	    	}

	    	if(ccextmsg->msg_type == CC_EXT_DONE_MSGTYPE || ccextmsg->msg_type == CC_ENCRYPTED_EXT_DONE)
	    	{
	    		handle_ccext_done_msg(recv_buf,nread,si_other);
	    	}
	    	// stage 7: relay packet
//	    	if(ccextmsg->msg_type == CC_RELAY_MSGTYPE || ccextmsg->msg_type == CC_RELAY_BACK_MSGTYPE || ccextmsg->msg_type == CC_ENCRYPTED_RELAY || ccextmsg->msg_type == CC_ENCRYPTED_RELAY_REPLY)
	    	if(ccextmsg->msg_type == CC_ENCRYPTED_RELAY)
	    	{
	        	if(_stage == 9){
	        		start_count =1;
	        	}
	    		handle_relay_msg(recv_buf,nread,si_other);
	    	}
	    	if( ccextmsg->msg_type == CC_ENCRYPTED_RELAY_REPLY ){
	    		printf("begin relay reply\n");
	    		start_count = 0;
	    		handle_relay_reply_msg(recv_buf,nread,si_other);
	    		//while(1);
	    	}

	    }// end of else if()
	} //end of if(nread != 0)
    } // end of if(it is from udp)
  } // end of while(1)
} 


void CRouter::output_log(char* out_str)
{
    FILE* logfp=fopen(_logfn,"a");
    if(!logfp)
    {
	printf("Open Log File:%s failed \n",_logfn);
	return;

    }
    fputs(out_str,logfp);
    fflush(logfp);
    fclose(logfp);
}

void CRouter::print_buf_hex(char* buf, int buf_len, int port)
{
    char log_buf[MAX_BUF_SIZE];
    memset(log_buf, 0, MAX_BUF_SIZE);
    int index=0;
    index += sprintf(log_buf, "pkt from port: %d, length: %d, contents: 0x", port, buf_len);
    for(int i=0; i<buf_len; i++)
    {
	index += sprintf(log_buf+index, "%02x", (unsigned char)buf[i]);
    }
    sprintf(log_buf+index, "\n");
    output_log(log_buf);
}

