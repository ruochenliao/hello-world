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

#include "proxy.h"
#include "router.h"


using namespace std;

/**************************************************************************
 * tun_alloc: allocates or reconnects to a tun/tap device. 
 * steal from simpletun.c
 * refer to http://backreference.org/2010/03/26/tuntap-interface-tutorial/ for more info 
 **************************************************************************/


int tun_alloc(char *dev, int flags) 
{
  struct ifreq ifr;
  int fd, err;
  char *clonedev = (char*)"/dev/net/tun";

  if( (fd = open(clonedev , O_RDWR)) < 0 ) {
    perror("Opening /dev/net/tun");
    return fd;
  }

  memset(&ifr, 0, sizeof(ifr));

  ifr.ifr_flags = flags;

  if (*dev) {
    strncpy(ifr.ifr_name, dev, IFNAMSIZ);
  }

  if( (err = ioctl(fd, TUNSETIFF, (void *)&ifr)) < 0 ) {
    perror("ioctl(TUNSETIFF)");
    close(fd);
    return err;
  }

  strcpy(dev, ifr.ifr_name);
  return fd;
}

/*
 * print error to stderr
 */
void print_err(char *msg, ...) {
  va_list argp;
  va_start(argp, msg);
  vfprintf(stderr, msg, argp);
  va_end(argp);
}


/*
 * split string 
 */
void SplitString(char* cStr, char* cDelim, vector<string> &sItemVec)
{
    char *p;
    p=strtok(cStr,cDelim);
    while (p!=NULL)
    {
	sItemVec.push_back(p);
	p=strtok(NULL,cDelim);
    }
}


CProxy::CProxy()
{
    _stage=0;
    _num_nodes=0;
    _num_hops=0;
    _cc_seq = 1;
    _tun_fd=-1;
    _sock_fd=-1;

    unsigned char key[KEY_LEN];
    memset(key, 0,KEY_LEN);
    generate_random_key(key,  KEY_LEN);

}

CProxy::~CProxy()
{
    close(_tun_fd);
    _sock_fd=-1;
}

//get the ip of all virtual network cards.
bool CProxy::get_all_interface_info()
{
    memset(rinfo,0,sizeof(struct router_info)*MAX_ROUTER_COUNT);
    int ret = get_all_interface_ip(rinfo);
    for(int i=0; i<MAX_ROUTER_COUNT;i++)
    {
	//struct sockaddr_in addr;
	//addr.sin_addr.s_addr = rinfo[i].nIP;

    }
    if(ret>0)
	return true;
    else
	return false;
}

// parse configure file
bool CProxy::initialize_configure(char* config_fn)
{
    FILE* config_fp=fopen(config_fn,"r");
    if(!config_fp)
    {
	printf("Open config file:%s failed \n",config_fn);
	return false;
    }

    char buffer[MAX_BUF_SIZE];
    vector<string> sItem;
    char delim[]=" \t";
    while(!feof(config_fp))
    {
	memset(buffer,0,MAX_BUF_SIZE);
	if(fgets(buffer,MAX_BUF_SIZE,config_fp))
	{
	    buffer[strlen(buffer)-1]='\0';
	    if(buffer[0]=='#')
		continue;
	    sItem.clear();
	    SplitString(buffer,delim,sItem);
	    if(sItem.size()<2)
		continue;
	    else
	    {
		if(sItem[0]=="stage")
		   _stage=atoi(sItem[1].c_str());
		if(sItem[0]=="num_routers")
		    _num_nodes=atoi(sItem[1].c_str());
		if(sItem[0]=="minitor_hops")
		    _num_hops=atoi(sItem[1].c_str());
	    }
	    if(sItem[0] == "die_after"){
	    	 _die_after=atoi(sItem[1].c_str());
	    }

	}
    }
//    printf("Stage:%d\t Nodes:%d\t Hops: %lu\n",_stage,_num_nodes,_num_hops);
    fclose(config_fp);
    
    memset(_logfn,0,MAX_FN_LEN);
    sprintf(_logfn,"stage%d.proxy.out",_stage);
    FILE* logfp=fopen(_logfn,"w");
    if(!logfp)
    {
	printf("Open Log File:%s failed \n",_logfn);

    }
    fclose(logfp);
    return true;
}

void CProxy::output_log(char* out_str)
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

//create tunnel interface
bool CProxy::initialize_tun(char* tun_fn, const int flags)
{
    _tun_fd = tun_alloc(tun_fn, flags | IFF_NO_PI);
    if(_tun_fd<0)
    {
	printf("Error connecting to tun interface %s!\n", tun_fn);
	return false;
    }
    else
	return true;
}

//create socket
bool CProxy::initialize_socket()
{
    bool status=true;
    status &= _mysock.create(SOCK_DGRAM,0);
    status &= _mysock.bind(0);
    if(status)
    {
	char out_buf[MAX_BUF_SIZE];
	sprintf(out_buf,"proxy port: %d\n", ntohs((_mysock._addr).sin_port));
	printf(out_buf);
	output_log(out_buf);
    }
    else
    {
	return false;
    }

    _sock_fd=_mysock._sock;
    return status;
}


//fork routers
bool CProxy::fork_router()
{
    int pid;
    int i=0;

    for(i=0;i<_num_nodes;i++)
    {
    	if((pid=fork())==0)
    	{
    		/*
    		 * fork routers, and tell them the stage number, as well as router index, proxy port, and router's IP.
    		 */
    		CRouter *router=new CRouter(_stage, i+1,  _mysock._addr, rinfo[i+1].nIP);

    		bool status = router->initialize_socket();
    		status &= router->initialize_rawsocket();
    		status &= router->initialize_tcp_rawsocket();

    		char send_buf[MAX_BUF_SIZE];
    		int client_pid=getpid();
    		if(status)
    		{
    			memset(send_buf,0,MAX_BUF_SIZE);
    			struct up_msg* upmsg = (struct up_msg *)send_buf;
    			upmsg->_pid = htons(client_pid);
    			upmsg->_index = htons(i+1);
    			router->send_data_UDP(send_buf, sizeof(struct up_msg), _mysock._addr);

    		}
    		else
    		{
    			printf("**Router** %d, PID: %d, failed initialize socket\n", i+1, pid);
    		}
	    
    		router->run();
    		delete router;
    		exit(0);
    	}
    }
    return true;
 }

void CProxy::handle_tun_tcp_traffic(char* buf, int len,int circuit_num){

    struct sockaddr_in source,dest;
    char log_buf[MAX_BUF_SIZE];
    char send_buf[MAX_PACKET_SIZE];
    memset(send_buf,0, MAX_PACKET_SIZE);

    char src_addr_buf[MAX_BUF_SIZE];
    char dst_addr_buf[MAX_BUF_SIZE];

    int nsend=0;
//    cout<<"hello world"<<endl;
//    print_tcp_packet(buf,len);

    struct iphdr *iph = (struct iphdr *)buf;

    unsigned short iphdrlen;
    iphdrlen = iph->ihl*4;
    struct tcphdr *tcphdr = (struct tcphdr *)(buf + iphdrlen);

    source.sin_addr.s_addr = iph->saddr;
    dest.sin_addr.s_addr = iph->daddr;
    memset(log_buf, 0, MAX_BUF_SIZE);
    memset(src_addr_buf, 0, MAX_BUF_SIZE);
    memset(dst_addr_buf, 0, MAX_BUF_SIZE);
    strcpy(src_addr_buf, inet_ntoa(source.sin_addr));
    strcpy(dst_addr_buf, inet_ntoa(dest.sin_addr));
    sprintf(log_buf, "TCP from tunnel, src IP/port: %s:%d, dst IP/port: %s:%d, seqno: %u, ackno: %u\n",src_addr_buf, ntohs(tcphdr->source),dst_addr_buf, ntohs(tcphdr->dest),ntohl(tcphdr->seq), ntohl(tcphdr->ack_seq));
    output_log(log_buf);
    _cc_seq = circuit_num +1;
    unsigned short cID = compute_circuit_id(0, _cc_seq);
    //printf("cID is %d\n",cID);
    int packet_len;
	//remember the old src addr;
	_old_src = iph->saddr;
	//zero the src addr and recompute the checksum;
	iph->saddr = htonl(0);
	iph->check = 0;
	iph->check = in_cksum((unsigned short*)iph, sizeof(struct iphdr));

	//encrypt the entire packet with keys of all routers in the path
	int elen;
	char* ebuf =NULL;
	//print the content of the packet, only for debug
//	print_packet_hex(buf, len);
	encrypt_multiround_with_padding(buf, len, &ebuf, &elen, _num_hops, circuit_num);
	//print the content of the packet, only for debug
//	print_packet_hex(ebuf, elen);
	//construct the relay message
	packet_len  = construct_relay_msg(send_buf, MAX_PACKET_SIZE, cID, ebuf, elen, CC_ENCRYPTED_RELAY, _stage);
	delete [] ebuf;

    nsend = send_data_UDP(send_buf, packet_len, rinfo[path[circuit_num][0]].r_addr);
	if(nsend <=0){
		printf("**Proxy** failed send tcp packet to router\n");
    }

}

// handle icmp traffic from tunnel interface
void CProxy::handle_tun_icmp_traffic(char* buf, int len, int circuit_num)
{

    struct sockaddr_in source,dest;
    char log_buf[MAX_BUF_SIZE];
    char send_buf[MAX_PACKET_SIZE];
    memset(send_buf,0, MAX_PACKET_SIZE);

    char src_addr_buf[MAX_BUF_SIZE];
    char dst_addr_buf[MAX_BUF_SIZE];

    int nsend=0;
    //print_icmp_packet(buf,len);
    struct iphdr *iph = (struct iphdr *)buf;
    
    unsigned short iphdrlen;
    iphdrlen = iph->ihl*4;
    struct icmphdr *icmph = (struct icmphdr *)(buf + iphdrlen);

    source.sin_addr.s_addr = iph->saddr;
    dest.sin_addr.s_addr = iph->daddr;
    memset(log_buf, 0, MAX_BUF_SIZE);
    memset(src_addr_buf, 0, MAX_BUF_SIZE);
    memset(dst_addr_buf, 0, MAX_BUF_SIZE);
    strcpy(src_addr_buf, inet_ntoa(source.sin_addr));
    strcpy(dst_addr_buf, inet_ntoa(dest.sin_addr));

    sprintf(log_buf, "ICMP from tunnel,src: %s, dst: %s, type: %d\n",src_addr_buf, dst_addr_buf, icmph->type);

    output_log(log_buf);

    if(icmph->type == ICMP_ECHO){
    	if(_stage < 5){
    		/* send to router via UDP socket */
    		int rindex=0;
    		memcpy(send_buf, buf, len);
    		if(_stage == 3)
    			rindex = 1;
    		else
    			rindex =  ((ntohl(iph->daddr)) % _num_nodes) + 1;
    		printf("router index:%d\n", rindex);
    		nsend = send_data_UDP(send_buf, len, rinfo[rindex].r_addr);
    	}
    	else{
    		_cc_seq = circuit_num +1;
    		unsigned short cID = compute_circuit_id(0, _cc_seq);
    		int packet_len;
    		if(_stage == 5){
    			packet_len  = construct_relay_msg(send_buf, MAX_PACKET_SIZE, cID, buf, len, CC_RELAY_MSGTYPE, _stage);
    		}

//	    	if (_stage == 6)
    		if(_stage >= 6){
    			//remember the old src addr;
    			_old_src = iph->saddr;

    			//zero the src addr and recompute the checksum;
    			iph->saddr = htonl(0);
    			iph->check = 0;
    			iph->check = in_cksum((unsigned short*)iph, sizeof(struct iphdr));

    			//encrypt the entire packet with keys of all routers in the path
    			int elen;
    			char* ebuf =NULL;
    			//print the content of the packet, only for debug
    			print_packet_hex(buf, len);
    			encrypt_multiround_with_padding(buf, len, &ebuf, &elen, _num_hops, circuit_num);
    			//print the content of the packet, only for debug
    			print_packet_hex(ebuf, elen);

    			//construct the relay message
    			packet_len  = construct_relay_msg(send_buf, MAX_PACKET_SIZE, cID, ebuf, elen, CC_ENCRYPTED_RELAY, _stage);
    			delete [] ebuf;
    			nsend = send_data_UDP(send_buf, packet_len, rinfo[path[circuit_num][0]].r_addr);
    		}// end of if(_stage >= 6)

    		if(nsend <=0){
    			printf("**Proxy** failed send packet via UDP\n");
    		}

    	}// end of else
    }// end of if(icmph->type == ICMP_ECHO)
}

//handle icmp packet from routers

void CProxy::handle_router_icmp_traffic(char* buf, int len, struct sockaddr_in si_other){

    struct sockaddr_in source,dest;
    char log_buf[MAX_BUF_SIZE];
    char src_addr_buf[MAX_BUF_SIZE];
    char dst_addr_buf[MAX_BUF_SIZE];

    int nsend=0;


    struct iphdr *iph = (struct iphdr *)buf;
    unsigned short iphdrlen;
    iphdrlen = iph->ihl*4;
    struct icmphdr *icmph = (struct icmphdr *)(buf + iphdrlen);
	

    //print_icmp_packet(recv_buf,nread);
    source.sin_addr.s_addr = iph->saddr;
    dest.sin_addr.s_addr = iph->daddr;
    memset(log_buf, 0, MAX_BUF_SIZE);
    memset(src_addr_buf, 0, MAX_BUF_SIZE);
    memset(dst_addr_buf, 0, MAX_BUF_SIZE);
    strcpy(src_addr_buf, inet_ntoa(source.sin_addr));
    strcpy(dst_addr_buf, inet_ntoa(dest.sin_addr));


    sprintf(log_buf, "ICMP from port: %d, src: %s, dst: %s, type: %d\n",ntohs(si_other.sin_port), src_addr_buf, dst_addr_buf, icmph->type);
    output_log(log_buf);


    /* write received packet to tun */
    nsend = write_data_TUN(buf, _tun_fd, len);
    if(nsend <=0 )
    {
	printf("**Proxy** failed write packet to tun\n");
    }

}

//handle relay message from 
void CProxy::handle_relay_msg(char* buf, int len, struct sockaddr_in si_other, int circuit_num)
{
    char log_buf[MAX_BUF_SIZE];
    char send_buf[MAX_PACKET_SIZE];
    char ssi[MAX_BUF_SIZE];
    char sdest[MAX_BUF_SIZE];
    struct sockaddr_in si,dest;
    memset(send_buf,0, MAX_PACKET_SIZE);
    int nsend;
    struct iphdr *iph = (struct iphdr *)buf;
    struct cc_relay_msg * ccrelaymsg = (struct cc_relay_msg*)(iph+1);
    unsigned short iID = ntohs(ccrelaymsg->cid);
    //if this is time out to kill NEXT-NAME router
    if( ccrelaymsg->msg_type == 0x92 && _stage ==9){
    	struct self_next * iport_oport= (struct self_next *)(ccrelaymsg +1);
    	int j;
    	for(j =0; j< 10;j++){
    		if (  ntohs((rinfo[j].r_addr).sin_port) == iport_oport ->next_name){
    			delet_router[del_num] = rinfo[j].index;//(iID &0xff00)>>8;
    			printf("router %d is deleted\n",delet_router[del_num]);
    			break;
    		}
    	}
    	memset(&flow[(iID & 0x000f)-1],0,sizeof(flow_cache));
    	printf("delete flow %d\n",(iID & 0x000f)-1);
    	printf("proxy: iID is %02x\n",iID);
    	del_num ++;
    	return;
    }
//    struct iphdr *riph = (struct iphdr *)(ccrelaymsg+1);
	struct iphdr *riph = (struct iphdr *)(ccrelaymsg + 1);
	struct tcphdr*tcp_hdr = (struct tcphdr *)(riph +1);
    print_buf_hex((char*)ccrelaymsg, len-(sizeof(struct iphdr)), ntohs(si_other.sin_port)); 
    int hlen =  sizeof(struct iphdr) + sizeof(struct cc_relay_msg);

    // plen is the length of tcphdr+data
    int plen = len - hlen;
    int clen;
    char * clear_packet;
    //decrypt the packet with keys of all routers in the path.
    decrypt_multiround_with_padding(buf+hlen, plen, &clear_packet, &clen, _num_hops,circuit_num);
    memcpy(buf+hlen, clear_packet, clen);
    //check if the packet is correct, only for debug
    //print_icmp_packet(buf+hlen, clen);
    //change destination IP and recompute ip checksum;
    riph->daddr  = _old_src;
    riph->check = 0;
    riph->check = in_cksum((unsigned short*)riph, sizeof(struct iphdr));
    // change TCP checksum
    if(riph->protocol == 6){
    	compute_tcp_checksum(riph, (unsigned short *)tcp_hdr);
    }
    len = hlen + clen;
    	delete [] clear_packet;
//    }

    /* write received packet to tun */
    //printf("write received packet to tun");
    //print_tcp_packet( buf+hlen, len-hlen);
    nsend = write_data_TUN(buf+hlen, _tun_fd, len-hlen);
    if(nsend <=0 ){
    	printf("**Proxy** failed write packet to tun\n");
    }

    //log
    si.sin_addr.s_addr = riph->saddr;
    dest.sin_addr.s_addr = riph->daddr;
    strcpy(ssi, inet_ntoa(si.sin_addr));
    strcpy(sdest, inet_ntoa(dest.sin_addr));

    memset(log_buf, 0, MAX_BUF_SIZE);
    if(riph ->protocol == 6){
    	// change TCP checksum
    	struct tcphdr*tcp_hdr = (struct tcphdr *)(riph +1);
    	compute_tcp_checksum(riph, (unsigned short *)tcp_hdr);
    	sprintf(log_buf, "incoming TCP packet, circuit: 0x%02x, src IP/port: %s:%u, dst: IP/port: %s:%u, seqno: %u, ackno: %u\n", iID, ssi,ntohs(tcp_hdr->source), sdest,ntohs(tcp_hdr->dest),ntohs(tcp_hdr->seq),ntohs(tcp_hdr->ack_seq));
    	output_log(log_buf);
    }
    if(riph ->protocol == 1){
    	//print_icmp_packet(buf+hlen, len-hlen+100);
    	sprintf(log_buf,"incoming packet, circuit incoming: 0x%02x, src: %s, dst: %s\n",iID,ssi,sdest);
    	output_log(log_buf);
    }

}

void CProxy::set_router_key(int index)
{
    //router's key = proxy's key ^ 16 copy of router's index
    unsigned char key[KEY_LEN];
    for(int i=0; i< KEY_LEN; i++)
    {
	key[i]=aes_key[i] ^ ((unsigned char)index);
    }
    memcpy(rinfo[index].key, key, KEY_LEN);
    char key_hex_buf[MAX_BUF_SIZE];
    memset(key_hex_buf, 0, MAX_BUF_SIZE);
    key_to_hex_buf(key, key_hex_buf, KEY_LEN);
//    printf("router: %d, key: 0x%s", index, key_hex_buf);
}

bool CProxy::collect_router_info()
{

    //"I am up" message from router
    char recv_buf[MAX_BUF_SIZE];
    char log_buf[MAX_BUF_SIZE];
    struct sockaddr_in si_other;
    memset(recv_buf,0,MAX_BUF_SIZE);
    int nread = recv_data_UDP(recv_buf, si_other);
    if(nread == sizeof(struct up_msg))
    {
	memset(log_buf, 0, MAX_BUF_SIZE);
	//memcpy(&_r_addr, &si_other, sizeof( struct sockaddr_in));
	struct up_msg* upmsg = (struct up_msg *)recv_buf;
	int pid = ntohs(upmsg->_pid);
	int index = ntohs(upmsg->_index);
	update_router_addr(index, pid, si_other);
	struct sockaddr_in rip;
	rip.sin_addr.s_addr = rinfo[index].nIP;
	//set the key for the router
//	if(_stage == 6)
	if(_stage >= 6)
	{
	    //assign key to routers, proxy's key ^ (router's index)16
	    set_router_key(index);
	}

	sprintf(log_buf, "router: %d, pid: %d, port: %d, IP: %s\n", index, pid, ntohs(si_other.sin_port), inet_ntoa(rip.sin_addr));
	output_log(log_buf);
//	printf("**Proxy** PID: %d, received packet from port: %d, length:%d\n", getpid(), ntohs(si_other.sin_port), nread);
	return true;
    }
    else
    {
//	printf("**Proxy** PID: %d, UNKNOWN packet from port: %d, length:%d\n", getpid(), ntohs(si_other.sin_port), nread);
	return false;
    }

}
void CProxy::kill_second_router(int circuit_num){
	char send_buf[128];
	memset(send_buf,0,sizeof(send_buf));
	struct cc_relay_msg *x = (struct cc_relay_msg *)send_buf;
	x ->msg_type = 0x91;
	send_data_UDP(send_buf, sizeof(send_buf), rinfo[path[circuit_num][1]].r_addr);
}
// proxy routine
void CProxy::run()
{
    //receive I'm UP message from routers, make sure all routers are up
    for(int i=0; i< _num_nodes;) 
    {
	if(collect_router_info())
	    i++;
    }
  /* use select() to deal with two fds at once */
  int maxfd = (_tun_fd > _sock_fd)?_tun_fd: _sock_fd;
  char recv_buf[MAX_BUF_SIZE];

  struct sockaddr_in si_other;
  int nread;
// test
  struct iphdr * iph;
  struct tcphdr * tcp_hdr;
  //struct icmphdr * icmphdr;
  int i;
  int judge;
  int circuit_num = 0;
  number_path = 0;
  memset(flow,0,sizeof(flow));
  memset(path,0,sizeof(path));
  memset(oID_mem, 0, sizeof(oID_mem));

  del_num =0;
  memset(delet_router,0,sizeof(delet_router));
  while(1) {
    int ret;
    fd_set rd_set;

    FD_ZERO(&rd_set);
    FD_SET(_tun_fd, &rd_set); 
    FD_SET(_sock_fd, &rd_set);
    ret = select(maxfd + 1, &rd_set, NULL, NULL, NULL);

    if (ret < 0 && errno == EINTR){
      continue;
    }

    if (ret < 0) {
      perror("select()");
      exit(1);
    }

    if(FD_ISSET(_tun_fd, &rd_set)) 
    {
    	/* data from tun */
    	memset(recv_buf,0,MAX_BUF_SIZE);
    	nread = read_data_TUN(recv_buf, _tun_fd, MAX_BUF_SIZE);
//    	printf("**Proxy** PID:%d, received packet from tun, length: %d\n",getpid(), nread);
    	iph = NULL;
    	iph = (struct iphdr *)recv_buf;
//    	struct iphdr *iph = (struct iphdr *)recv_buf;
    	//handle ICMP packet
    	if(iph->protocol == 1)
    	{
    		judge = 0;
    		for(i = 0; i<36;i++){
    		    if( flow[i].src_ip == iph->saddr && flow[i].dst_ip == iph->daddr&&flow[i].protocol == iph ->protocol){
    		    	judge = 1;
    		    	circuit_num = i;
    		    	break;
    		    }
    		}
    		// new flow
    	    if(judge != 1){
    	    	circuit_num = number_path;
    	    	flow[number_path].src_ip = iph->saddr;
    	    	flow[number_path].dst_ip = iph->daddr;
    	    	flow[number_path].protocol = 1;
    	    	flow[number_path].number_packet = 1;
    	    	//construct circuit path
    	    	generate_random_path(circuit_num);
    	    	create_circuit(circuit_num);
    	    	printf("****************built a ICMP circuit *****************\n");
    	    	number_path ++;
    	    	oID_mem[circuit_num] = compute_circuit_id(0 , circuit_num+1);
    	    }
    	    // old flow
    	    handle_tun_icmp_traffic(recv_buf, nread, circuit_num);
    	    if(judge == 1){
    	        if(_stage ==9){
    	        	flow[circuit_num].number_packet ++;
    	        	if(flow[circuit_num].number_packet >= _die_after && ((_num_nodes -del_num) > _num_hops) ){
    	        		kill_second_router(circuit_num);
    	        		printf("number_pakcet is %d\n",flow[circuit_num].number_packet);
    	        	}
    	        }
    	    }
    	}
    	// handle TCP packet
        if(iph -> protocol == 6){
        	//printf("get packet from tunnel\n");
        	tcp_hdr = NULL;
        	judge =0;
        	tcp_hdr = (struct tcphdr *)(iph + 1);
        	for(i = 0; i<36;i++){
        		if( flow[i].src_ip == iph->saddr && flow[i].dst_ip == iph->daddr&&flow[i].dst_port == tcp_hdr->dest&&flow[i].protocol == iph->protocol){ //flow[i].src_port==tcp_hdr->source &&flow[i].dst_port == tcp_hdr->dest){
        			judge =1;
        			circuit_num = i;
        			break;
        		}
        	}
        	// there is not exiting circuit,so create a new circuit
        	if(judge != 1){
        		//rintf("flow src ip :%u, iph src ip: %u\n",flow[circuit_num].src_ip,iph ->saddr);
        		//printf("flow dst ip :%u, iph dst iph: %u\n",flow[circuit_num].dst_ip, iph->daddr);
        		//printf("flow src port : %u, tcphdr src port: %u\n",flow[circuit_num].src_port,tcp_hdr ->source);
        		//printf("flow dst port : %u. tcphdr dst port: %u\n",flow[circuit_num].dst_port,tcp_hdr ->dest);
        		printf("going to generate a new circuit with a circuit number: %d\n",number_path);
        		circuit_num = number_path;
        		flow[circuit_num].src_ip = iph->saddr;
        		flow[circuit_num].dst_ip = iph->daddr;
        		flow[circuit_num].src_port=tcp_hdr->source;
        		flow[circuit_num].dst_port = tcp_hdr->dest;
        		flow[circuit_num].protocol = 6;
    	    	generate_random_path(circuit_num);
    	    	create_circuit(circuit_num);
    	    	printf("****************built a TCP circuit *****************\n");
        		oID_mem[circuit_num] = compute_circuit_id(0 , circuit_num+1);
        		number_path ++;
        		//circuit_num = number_path;
        	}
    	    // old flow
    	    else{
    	    	if(flow[circuit_num].number_packet >= _die_after){
    	    		kill_second_router(circuit_num);
    	    	}
    	    	flow[circuit_num].number_packet ++;
    	    }
        	handle_tun_tcp_traffic(recv_buf,nread, circuit_num);
        }
    }
    if(FD_ISSET(_sock_fd, &rd_set)) 
    {
    	cout<<"proxy: receive from mit !\n"<<endl;
    	/* data from the routers via UDP socket*/
    	memset(recv_buf,0,MAX_BUF_SIZE);
      	nread = recv_data_UDP(recv_buf, si_other);
      	if(nread >0) {
      		struct iphdr *iph = (struct iphdr *)recv_buf;
      		struct cc_relay_msg * ccrelaymsg = (struct cc_relay_msg*)(iph+1);
      		circuit_num = 0;
      		circuit_num = ntohs(ccrelaymsg ->cid) -1;
      		//printf("proxy receive: incoming ID is %x\n",ntohs(ccrelaymsg ->cid) );
      		cout<< "proxy receive: circuit_num is "<<circuit_num<<endl;

      		if(iph->protocol == CC_EXT_PROTOCOL){
      			handle_relay_msg(recv_buf, nread, si_other, circuit_num);
      		}
      		else{
      			//unknown packet
      			printf("**Proxy** PID: %d, UNKNOWN packet from port: %d, length:%d\n", getpid(), ntohs(si_other.sin_port), nread);
      		}
	}
    }
  }
}  


void CProxy::update_router_addr(int index, int pid, struct sockaddr_in si_other)
{

    //struct sockaddr_in addr;
    //addr.sin_addr.s_addr = rinfo[index].nIP;

    memcpy(&(rinfo[index].r_addr), &si_other, sizeof(struct sockaddr_in));
    rinfo[index].pid = pid;
    rinfo[index].index=index;
//    printf("router:%d, pid: %d, IP: %s\n",rinfo[index].index, rinfo[index].pid, inet_ntoa(addr.sin_addr));

}

int CProxy::recv_data_UDP(char* recv_buf, struct sockaddr_in & si_other)
{
    int recv_ret=_mysock.recv_data_UDP(recv_buf, si_other);
    return recv_ret;
}

int CProxy::send_data_UDP(const char* send_buf, const int len, struct sockaddr_in & ser_addr)
{
    int send_ret=_mysock.send_data_UDP(send_buf, len, ser_addr);
    return send_ret;
}

int CProxy::read_data_TUN(char* recv_buf, int fd, int n)
{
   int n_read = read(fd, recv_buf, n);
   if(n_read<0)
   {
       perror("Reading data from tun");
       return -1;
   }
   else
       return n_read;
}

int CProxy::write_data_TUN(char* send_buf, int fd, int n)
{
   int n_write = write(fd, send_buf, n);
   if(n_write<0)
   {
       perror("Writing data to tun");
       return -1;
   }
   else
       return n_write;
}

void CProxy::generate_random_path(int circuit_num)
{
    srand ( time(NULL) );
    for(unsigned int i=0; i<_num_hops; )
    {
    	int hop = rand() % _num_nodes +1;
    	//prevent duplicated routers
    	if( random1 ==hop){
    		continue;
    	}
    	if(is_dup_hop(circuit_num,hop))
    	{
    		continue;
    	}
    	else
    	{
    		path[circuit_num][i]=hop;
    		i++;
    	}

    }
    random1 = path[circuit_num][0];

    char log_buf[MAX_BUF_SIZE];
    for(unsigned int i=0; i<_num_hops; i++)
    {

	memset(log_buf, 0, MAX_BUF_SIZE);
	//sprintf(log_buf, "hop: %d, router: %d\n",i+1, path[i]);
	sprintf(log_buf, "hop: %d, router: %d\n",i+1, path[circuit_num][i]);
	printf(log_buf);
	output_log(log_buf);
    }
}

bool CProxy::is_dup_hop(int circuit_num,int hop)
{
    for(unsigned int i=0; i< _num_hops; i++){
    	if(hop==path[circuit_num][i]){
    		return true;
    	}
    	if(delet_router[i]==hop &&_stage ==9){
    		return true;
    	}
    }
    return false;
}

//create circuit 
bool CProxy::create_circuit(int circuit_num)
{
    int nsend=0;
    int nread=0;
    char msg[MAX_PACKET_SIZE];
    char recv_buf[MAX_PACKET_SIZE];
    char log_buf[MAX_BUF_SIZE];
    char *p;

    struct sockaddr_in si_other;
    //compute ID:
    _cc_seq = circuit_num +1;
    unsigned short cID = compute_circuit_id(0, _cc_seq);
    //printf("proxy: cID is %u\n",cID);
    unsigned short next_hop;
    unsigned short last_hop =strtol("0xffff", &p, 16) ;
    for(unsigned int i=1; i<= _num_hops; i++){
    	if(i==_num_hops){
    		next_hop =  last_hop;
    	}
    	else{
    		//figure out the next hop UDP port;
    		int nr_index = path[circuit_num][i];
    		next_hop = (rinfo[nr_index].r_addr).sin_port;
    	}

	memset(msg, 0, MAX_PACKET_SIZE);
	int packet_len;
	//first send faked deffie hellman message to distribute keys to routers;
//	if(_stage == 6)
	if(_stage >=6 ){
	    //send faked deffie hellman message to router
	    memset(msg, 0, MAX_PACKET_SIZE);
	    char* encrypted_key = NULL;
	    int elen;
	    //encrypt the key;
	    encrypt_multiround_with_padding((char *)rinfo[path[circuit_num][i-1]].key, KEY_LEN, &encrypted_key, &elen, i-1,circuit_num);
	    //construct the faked deffie hellman message
	    packet_len = construct_deffie_hellman_padding_msg(msg, MAX_PACKET_SIZE, cID, (unsigned char*)encrypted_key, elen);
	    delete [] encrypted_key;

	    //log
	    memset(log_buf, 0, MAX_BUF_SIZE);
	    char key_hex_buf[MAX_BUF_SIZE];
	    memset(key_hex_buf, 0, MAX_BUF_SIZE);
	    int key_buf_len = key_to_hex_buf(rinfo[path[circuit_num][i-1]].key, key_hex_buf, KEY_LEN);
	    int index = sprintf(log_buf, "new-fake-diffe-hellman, router index: %d, circuit outgoing: 0x%02x, key: 0x", path[circuit_num][i-1], cID);
	    memcpy(log_buf+index, key_hex_buf, key_buf_len);
	    output_log(log_buf);
	    
	    //send out the packet, always to the first hop of the path
	    nsend = send_data_UDP(msg, packet_len, rinfo[path[circuit_num][0]].r_addr);
	    if(nsend <=0 ){
	    	printf("**Proxy** failed send Deffie-Hellman message\n");
	    }
	}

	//start building circuit 
	memset(msg, 0, MAX_PACKET_SIZE);
//	if(_stage == 6)
	if(_stage >= 6){
	    char clear_port[ENC_PORT_LEN];
	    memset(clear_port, 0, ENC_PORT_LEN);
	    int port_len = sprintf(clear_port, "%d", next_hop);
	    //printf("next_hop is %d\n",next_hop);
	    char* encrypted_port = NULL;
	    int elen;
	    //encrypt the port number;
	    encrypt_multiround_with_padding(clear_port, port_len, &encrypted_port, &elen, i,circuit_num);

	    //construct the circuit extend message
	    //printf("proxy: cID0 is %u\n",cID);
	    packet_len= construct_encrypted_circuit_ext_msg_padding(msg, MAX_PACKET_SIZE, cID, (unsigned char*)encrypted_port, elen);
	    delete [] encrypted_port;
	}
/*
	if(_stage == 5)
	{
	    packet_len= construct_circuit_ext_msg(msg, MAX_PACKET_SIZE, cID, next_hop);
	}
*/
	//always send circuit extend message to the first hop of the path
	nsend = send_data_UDP(msg, packet_len, rinfo[path[circuit_num][0]].r_addr);
	if(nsend <=0 )
	{
	    printf("**Proxy** failed send circuit message\n");
	}


	//receive reply from first hop
	memset(recv_buf,0,MAX_BUF_SIZE);
	nread = recv_data_UDP(recv_buf, si_other);
	print_buf_hex((char*)(recv_buf+sizeof(struct iphdr)), nread-(sizeof(struct iphdr)), ntohs(si_other.sin_port)); 
	if(nread >0) 
	{
	    struct iphdr *iph = (struct iphdr *)recv_buf;
	    struct cc_ext_done_msg * ccextdonemsg = (struct cc_ext_done_msg *)(iph+1);
	    unsigned short iID = ntohs(ccextdonemsg->cid);
	    /* check if it is a circuit packet */
	    if(iph->protocol == CC_EXT_PROTOCOL && (ccextdonemsg->msg_type ==  CC_EXT_DONE_MSGTYPE || ccextdonemsg->msg_type == CC_ENCRYPTED_EXT_DONE))
	    {
//		printf("**Proxy**, extend-done circuit from port: %d, length: %d\n", ntohs(si_other.sin_port), nread);
		memset(log_buf, 0, MAX_BUF_SIZE);
		sprintf(log_buf, "incoming extend-done circuit, incoming: 0x%d from port: %d\n", iID, ntohs(si_other.sin_port));
		output_log(log_buf);
	    }
	}

    }
    return true;
}

int CProxy::construct_deffie_hellman_padding_msg(char* buf, int len, unsigned short cID, unsigned char key[], int keylen)
{
    memset(buf, 0, len);
    struct iphdr * iph = (struct iphdr *)buf;
    //fill IP header
    iph->protocol = CC_EXT_PROTOCOL;
    // use loop address 
    iph->saddr = inet_addr("127.0.0.1");
    iph->daddr = inet_addr("127.0.0.1");
	
    struct cc_deffie_hellman_msg * cc_dh_msg = ( struct  cc_deffie_hellman_msg*)(iph+1);
    int hdlen = sizeof(struct iphdr) + sizeof(struct cc_deffie_hellman_msg);

    cc_dh_msg->msg_type = FAKE_DIFFIE_HELLMAN;
    cc_dh_msg->cid = htons(cID);

    memcpy(buf+hdlen, key, keylen);
    return (hdlen + keylen);
}


int CProxy::construct_deffie_hellman_msg(char* buf, int len, unsigned short cID, unsigned char key[] )
{
    memset(buf, 0, len);
    struct iphdr * iph = (struct iphdr *)buf;
    //fill IP header
    iph->protocol = CC_EXT_PROTOCOL;
    // use loop address 
    iph->saddr = inet_addr("127.0.0.1");
    iph->daddr = inet_addr("127.0.0.1");
	
    struct cc_deffie_hellman_msg * cc_dh_msg = ( struct  cc_deffie_hellman_msg*)(iph+1);
    cc_dh_msg->msg_type = FAKE_DIFFIE_HELLMAN;
    cc_dh_msg->cid = htons(cID);
    //memcpy(cc_dh_msg->key, key, KEY_LEN);
    return (sizeof(struct iphdr) + sizeof(struct cc_deffie_hellman_msg));
}


int CProxy::construct_encrypted_circuit_ext_msg_padding(char* buf, int len, unsigned short cID, unsigned char nport[], int port_len)
{
    memset(buf, 0, len);
    struct iphdr * iph = (struct iphdr *)buf;
    //fill IP header
    iph->protocol = CC_EXT_PROTOCOL;
    // use loop address 
    iph->saddr = inet_addr("127.0.0.1");
    iph->daddr = inet_addr("127.0.0.1");

    struct cc_encrypt_ext_msg * cc_eext_msg = ( struct cc_encrypt_ext_msg *)(iph+1);
    int hlen = sizeof(struct iphdr) + sizeof(struct cc_encrypt_ext_msg);

    // msg type: 0x62
    cc_eext_msg->msg_type = CC_ENCRYPTED_EXT;
    // circuit ID
    cc_eext_msg->cid = htons(cID);
    // next hop UDP port number
    memcpy(buf+hlen, nport, port_len);
    //memcpy(cc_eext_msg->encrypted_port, nport, ENC_PORT_LEN);
    return (hlen + port_len);
}

int CProxy::construct_circuit_ext_msg(char* buf, int len, unsigned short cID, unsigned short nport)
{
    memset(buf, 0, len);
    struct iphdr * iph = (struct iphdr *)buf;
    //fill IP header
    iph->protocol = CC_EXT_PROTOCOL;
    // use loop address 
    iph->saddr = inet_addr("127.0.0.1");
    iph->daddr = inet_addr("127.0.0.1");

    struct cc_ext_msg * cc_ext_msg = ( struct cc_ext_msg *)(iph+1);
    if(_stage == 5)
    {
       	// msg type: 0x52
    	cc_ext_msg->msg_type = CC_EXT_MSGTYPE;
    }

    // circuit ID
    cc_ext_msg->cid = htons(cID);
    // next hop UDP port number
    cc_ext_msg->next_hop = nport; 
    return (sizeof(struct iphdr) + sizeof(struct cc_ext_msg));
}



void CProxy::print_buf_hex(char* buf, int buf_len, int port)
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


void CProxy::generate_random_key(unsigned char key[], int len)
{
    //generate random key;
    srand ( time(NULL) );
    for(int i=0; i< len; i++)
    {
	key[i] = rand() % 256;
    }
    memset(aes_key, 0, len);
    memcpy(aes_key, key, len);
}

//onion encryption of data
void CProxy::encrypt_multiround_with_padding(char* ctext , int inlen, char**  etext, int* outlen,  int round, int circuit_num)
{

    if(round == 0)
    {
    	*etext = new char [inlen];
    	memset(*etext, 0, inlen);
    	memcpy(*etext, ctext, inlen);
    	*outlen = inlen;
    	return;
    }

    char * inbuf = new char [inlen];
    memset(inbuf, 0, inlen);
    memcpy(inbuf, ctext, inlen);


    int ilen = inlen;
    int olen;
    char * outbuf=NULL;
    for(int i = (round-1); i >= 0; i--)
    {
    	encrypt_msg_with_padding(inbuf, ilen, &outbuf, &olen, rinfo[path[circuit_num][i]].key);
    	//remember the new length
    	ilen = olen;
    	delete [] inbuf;

    	inbuf = new char [ilen];
    	memcpy(inbuf, outbuf, ilen);
    	delete [] outbuf;
    }
    *etext = inbuf;
    *outlen = olen;
}

//onion decrpytion of data.
void CProxy::decrypt_multiround_with_padding(char* ctext , int inlen, char**  etext, int* outlen,  int round,int circuit_num)
{

    if(round == 0)
    {

	*etext = new char [inlen];
	memset(*etext, 0, inlen);
	memcpy(*etext, ctext, inlen);
	*outlen = inlen;
	return;
    }

    char * inbuf = new char [inlen];
    memset(inbuf, 0, inlen);
    memcpy(inbuf, ctext, inlen);


    int ilen = inlen;
    int olen;
    char * outbuf=NULL;
    for(int i = 0;  i < round; i++)
    {

	decrypt_msg_with_padding(inbuf, ilen, &outbuf, &olen, rinfo[path[circuit_num][i]].key);
	//remember the new length
	ilen = olen;
	delete [] inbuf;
	inbuf = new char [ilen];
	memcpy(inbuf, outbuf, ilen);
	delete [] outbuf;
    }
    *etext = inbuf;
    *outlen = olen;
}

