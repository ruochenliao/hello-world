a. Name: Ruochen Liao 
b.Student ID: 1533019693

c. I finished the all the steps in the project including the phase1, phase2 and phase3.

d. I have 4 code file which are makefile, auctionserver.c, bidder.c, seller.c

The auctionserver
   In phase1,auctionserver TCP receive the 4 request authentication request. End phase1
   In phase 2, auctionserver receives item list from seller.
   In phase 3, auction server UDP broadcasts the item lists and receives 2 bidding UDP from bidders.
   At last, auction server build TCP to tell sellers and bidders the final outcome.
 
The bidder file 
   In phase 1, bidders log in the auctionserver with its account in formation and then get the authorized information
   In phase 2, bidders do nothing
   In phase 3, bidders receive the broadcasted item lists from server and then send the binding information back through UDP connction

The seller file
   In phase 1, sellers log in the auctionserver with its account in formation and then get the authorized information 
   In phase 2, sellers send the item lists to the auctionserver through TCP.
   In phase 3, sellers get the bidding result from the auctionserver.

e  I add a "makefile" and "fork" for the convinience of compliling. 
   First step is to write "make" on nunki, it will automatically compile the auctionserver.c, bidder.c, seller.c
   second step is to write "seller" on nunki, and then write "bidder" on another window, at last 
   write "seller" on the other window. 
   the order of events: 1.make 2.auctionserver 3.bidder 4.seller

f In phase one, when auctionserver get the right account number, I add the "a1, a2, a3...a6 access successflly"
  In phase three, after auctionserver send the same bidding result to bidders and sellers.

g 1.If you run the seller file ealier than the bidder file, the bidding proccess will fail, because there are 
sleep() problem. So the input order should be consdiered well.
  2. The buffer of broadcastlist is 200*(char), the buffer of the itemlists and bidding is samller, so if the 
     input memory is too large, some information may be lost.

h reused Code: The TCP and UDP modles are copied from Beej's guide to Network Porgramming.
Comments: it is reliable code.

