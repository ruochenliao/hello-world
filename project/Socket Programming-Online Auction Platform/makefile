all: bidder seller server
seller: seller.c
	gcc -g -Wall -o seller seller.c -lsocket -lnsl -lresolv
server: auctionserver.c
	gcc -g -Wall -o server auctionserver.c -lsocket -lnsl -lresolv
bidder: bidder.c
	gcc -g -Wall -o bidder bidder.c -lsocket -lnsl -lresolv
clean:
	rm -f *.o bidder seller server
