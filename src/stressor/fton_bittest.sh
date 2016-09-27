#!/bin/bash

#This script reads a file in bit by bit, then transmits that file through a
#networking channel. It does this by broadcasting a great deal of traffic for
#20ms for every 1 while doing nothing for 20ms for every 0.

#Args
#1 = filename
#2 = destination ip

#check that args are valid
if [ $# -lt 2 ]; then
    echo "Usage: $0 filename destination_ip"
    exit
fi

#check that file actually exists
if [ ! -e $1 ]; then
    echo "File $1 does not exist!"
    exit
fi

#figure out the full length of the file in bytes 
TOTAL_BYTES=$(du -b $1 | cut -f1)

SENTINEL=1
while [ $SENTINEL -lt $(($TOTAL_BYTES)) ];
do
    #using head, print all bytes until byte we are looking for, then
    #using tail extract only that byte
    CURRENT_BYTE=$(head -c $SENTINEL $1 | tail -c 1)

    #CURRENT_BYTE is the current byte in character form, but we actually
    #need it in numerical form in order to play all the fun bit-shifting
    #games with it that are needed later.
    #To effect this change, we will use printf to implicity convert it
    #into a digit
    NUMERICAL_CURRENT_BYTE=$(printf "%d" \'$CURRENT_BYTE)

    #mask out each bit, then network appropriately given its value
    #we network in 
    MASK_SENTINEL=7
    while [ $MASK_SENTINEL -gt -1 ];
    do
	#bitmasking out the desired bit. we don't need to bother
	#shifting it to the ones place, since anything greater than a
	#zero can be automatically assumed to be a one
	MASK=$((1 << MASK_SENTINEL))
	MASKED_BYTE=$((NUMERICAL_CURRENT_BYTE & MASK))

	#network a one; i.e. flood network for 20ms
	if [ $MASKED_BYTE -gt 0 ]; then
	    #FIXME: temporarily testing accuracy of bits
	    #below line should be uncommented and echo should be deleted
	    #timeout 10s hping3 -i u10 --flood $2
	    echo 1

	    
        #nework a zero; i.e. sleep for ms
	else
	    #FIXME: testing; same rules as above
	    #sleep 10
	    echo 0;
	fi
	let MASK_SENTINEL-=1
    done
    
    let SENTINEL+=1
done
