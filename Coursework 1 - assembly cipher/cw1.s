@ Simple Cipher in ARM v7
@ 03/11/2019
@ Robin Rai (100242165) and Hemal Munbodh (100246776)

@ NOTE: KEY FOR COMMENTS
@
@ 1. C code
@ 2. Input arguments
@ 3. Other comments about function
@ 4. Actual function

.data 								@ data section starts here
.balign 4

euclidError: .asciz "Error: Key lengths are not co-prime. Exiting..\n"

.text								@ code section starts here
.balign 4




@#include <stdio.h>
@#include <stdlib.h>
@#include <string.h>
@int strLength(char *input) {
@    int counter = 0;
@    while (*(input + counter) != '\0') {
@        counter++;
@    }
@    return counter;
@}

@ int length(r0) strLength(char * input(r0))
@ r3 is the counter. r2 is scratch. Finds length of a given string
strLength:
	PUSH		{r11, lr}			@ push lr to stack and unused register for alignment.
	MOV			r3,#0				@ sets counter to 0

	strLengthLoop:
		LDRB		r2,[r0],#1		@ loads each character pointed at by r0 into r2. Post indexes by one character every loop.

		CMP			r2,#0			@ compare to null character at end of string
		BEQ			strLengthEnd	@ branch to end on reaching null character

		ADD			r3,r3,#1		@ increment counter

		B			strLengthLoop	@ loop again to start of label
  
	strLengthEnd:					@ end function and return
		MOV			r0,r3			@ Move counter (string length) into r0
		POP			{r11, lr}		@ pop lr and alignment register
		BX			lr				@ go to lr (next instruction)





@int gcd(int key1Length, int key2Length) {
@    while (key1Length != key2Length) {
@        if (key1Length > key2Length) {
@            key1Length = key1Length - key2Length;
@        } else {
@            key2Length = key2Length - key1Length;
@        }
@    }
@    return key1Length;
@}

@ int gcdInt(r0) gcd(int key1Length(r0), int key2Length(r1))
@ Finds gcd between two ints.
gcd:
	PUSH {r11, lr}					@ push lr to stack and unused register for alignment.
	gcdloop:
		CMP			r0, r1			@ compares keyLength 1 and 2
		BEQ			gcdend			@ ends if theyre equal. if they are equal, they are either 1/coprime, or not. check for coprime is done in main to exit
		BLT			lessThan		@ if key1 is less than key 2 go to lessThan label

									@ if key1 > key2
		SUB			r0, r0, r1		@ subs key1 from key2
		B			gcdloop			@ go to start of loop label
	
									@ if key1 < key2
		lessThan:
			SUB		r1, r1, r0		@ subs key2 from key1
			B		gcdloop			@ go to start of loop label
	
	gcdend:							@ end function and return gcdInt
		POP  {r11, lr}				@ pop lr and alignment register
		BX lr						@ go to lr (next instruction)
  
  



@int mod(int modder, int moddee) {
@    while (moddee >= modder) {
@        if (modder == moddee) {
@            int modInt = 0;
@			 return modInt;
@        } else {
@            int modInt = moddee - modder;
@        }
@    }
@    return modInt;
@}

@ int modInt(r0) mod(int moddee(r0), int modder(r1))
@ Mods moddee with modder. If there is no mod, returns 0.
mod:
	PUSH			{r11, lr}		@ push lr to stack and unused register for alignment.
	modLoop:
		CMP			r0, r1			@ compares moddee to modder
		BEQ			noModEnd		@ if equal go to noModEnd
		BLT			modEnd			@ if moddee < modder function has finished

									@ if moddee > modder
		SUB			r0, r0, r1		@ moddee - modder
		B			modLoop			@ go to start of loop label

	noModEnd:						@ if moddee = modder, end function and return modInt
		MOV			r0 , #0			@ modInt = 0
		POP			{r11, lr}		@ pop lr and alignment register
		BX lr						@ go to lr (next instruction)

	modEnd:							@ end function and return modInt
		POP			{r11, lr}		@ pop lr and alignment register
		BX lr						@ go to lr (next instruction)





@int encode(int messageChar, int keyChar) {
@    messageChar = messageChar - 96;
@    keyChar = keyChar - 96;
@
@    if (((messageChar - keyChar) + 2) < 1) {
@        int cipherChar = messageChar - keyChar + 2 + 26;
@        cipherChar += 96;
@        return cipherChar;
@    } else if (((messageChar - keyChar) + 2) > 26) {
@        int cipherChar = messageChar - keyChar + 2 - 26;
@        cipherChar += 96;
@        return cipherChar;
@    } else {
@        int cipherChar = messageChar - keyChar + 2;
@        cipherChar += 96;
@        return cipherChar;
@    }
@}

@ char cipherChar(r0) encode(char messageChar(r0, char keyChar(r1))
@ Uses r2 as scratch. encodes the messageChar with the keyChar to produce cipherChar.  		
encode:
	PUSH		{r11, lr}			@ push lr to stack and unused register for alignment.

	SUB			r0, r0, #96			@ translates ascii a-z from 97-122 in ascii to 1-26
	SUB			r1, r1, #96			@ same as above
	
	SUB			r2, r0, r1			@ r2 is a testEncode.
	ADD			r2, r2, #2			
	CMP			r2, #1				@ compare testEncode(r2) with 1/letter 'a'
	BLT			encodeIfSmall		@ if testEncode < 'a' go to label encodeIfSmall
	CMP			r2, #26				@ if testEncode > 'z' go to label encodeIfBig
	BGT			encodeIfBig

									@ if testEncode is within a-z range encode normally
	SUB			r0, r0, r1			@ cipherChar = messageChar - keyChar
	ADD			r0, r0, #2			@ cipherChar = cipherChar + 2
	ADD			r0, r0, #96			@ restores ascii format on cipherChar
	B			encodeEnd			@ branch to end of function label
	
									@ if testEncode was out of a-z range
	encodeIfSmall:					@ encodes then adds 26 to restore a-z range
		SUB			r0, r0, r1		@ cipherChar = messageChar - keyChar
		ADD			r0, r0,	#28		@ cipherChar = cipherChar + 2 + 26
		ADD			r0, r0, #96		@ restores ascii format on cipherChar
		B encodeEnd					@ branch to end of function label

									@ if testEncode was out of a-z range
	encodeIfBig:					@ encodes then subtracts 26 to restore a-z range
		SUB			r0, r0, r1		@ cipherChar = messageChar - keyChar
		SUB			r0, r0,	#24		@ cipherChar = cipherChar + 2 - 26
		ADD			r0, r0, #96		@ restores ascii format on cipherChar
		B encodeEnd					@ branch to end of function label
		
	encodeEnd:						@ end function and return cipherChar
		POP {r11, lr}				@ pop lr and alignment register
		BX lr						@ go to lr (next instruction)
	




@int decode(int cipherChar, int keyChar) {
@    cipherChar = cipherChar - 96;
@    keyChar = keyChar - 96;
@    int messageChar = cipherChar + keyChar - 2;
@    if (messageChar > 26) {
@        messageChar = messageChar - 26;
@    }
@    if (messageChar < 1) {
@        messageChar = messageChar + 26;
@    }
@    messageChar = messageChar + 96;
@    return messageChar;
@}

@ char messageChar(r0) decode(char cipherChar(r0, char keyChar(r1))
@ Uses r2 as scratch.	decodes the cipherChar with the keyChar to produce messageChar.	
decode:
	PUSH		{r11, lr}			@ push lr to stack and unused register for alignment.
	SUB			r0, r0, #96			@ translates ascii a-z from 97-122 in ascii to 1-26
	SUB			r1, r1, #96			@ same as above
	
	ADD			r2, r0, r1			@ r2 is a testDecode.
	SUB			r2, r2, #2			
	CMP			r2, #26				@ compare testDecode(r2) with 1/letter 'z'
	BGT			decodeIfBig			@ if testDecode > 'z' go to label decodeIfBig
	CMP			r2, #1				@ compare testDecode(r2) with 1/letter 'z'
	BLT			decodeIfSmall		@ if testDecode > 'a' go to label decodeIfSmall
	
									@ decodes normally
	ADD			r0, r0, r1			@ messageChar = cipherChar + keyChar
	SUB			r0, r0, #2			@ messageChar = messageChar - 2
	ADD			r0, r0, #96			@ restores ascii format on messageChar
	B			decodeEnd			@ branch to end of function label
	
									@ if testDecode was out of a-z range
	decodeIfBig:					@ decodes then subtracts 26 to restore a-z range
		ADD			r0, r0, r1		@ messageChar = cipherChar + keyChar
		SUB			r0, r0, #28		@ messageChar = messageChar - 2 - 26
		ADD			r0, r0, #96		@ restores ascii format on messageChar
		B			decodeEnd		@ branch to end of function label

									@ if testDecode was out of a-z range
	decodeIfSmall:					@ decodes then adds 26 to restore a-z range
		ADD			r0, r0, r1		@ messageChar = cipherChar + keyChar
		ADD			r0, r0, #24		@ messageChar = messageChar - 2 + 26
		ADD			r0, r0, #96		@ restores ascii format on messageChar
		B			decodeEnd		@ branch to end of function label
		
		
	decodeEnd:						@ end function and return messageChar
		POP {r11, lr}				@ pop lr and alignment register
		BX lr						@ go to lr (next instruction)
	
		



@void fullEncode(int key1Length, int key2Length, char *key1, char *key2) {
@    int counter = 0;
@    int messageChar;
@    while (messageChar = getchar()) {
@        getchar();
@
@        if (messageChar < 97) {
@            messageChar = messageChar + 32;
@        }
@        if (messageChar > 122) {
@            continue;
@        } else if (messageChar < 97) {
@            continue;
@        } else {
@            int temp = key1Length + counter;
@            temp = temp % key1Length;
@            char keyChar = key1[temp];
@            char cipherChar = encode(messageChar, keyChar);
@            temp = key2Length + counter;
@            temp = temp % key2Length;
@            keyChar = key2[temp];
@            cipherChar = encode(cipherChar, keyChar);
@            putchar(cipherChar);
@            counter++;
@        }
@        printf("\n");
@    }
@}

@ void fullEncode(int key1Length(r0), int key2Length(r1), char * key1(r2), char * key2(r3))
@ r4 is getChar char, messageChar. r5 is key1Length. r6 is key1. r7 is key2Length. r8 is key2. r10 is counter.
@ fullEncode loops through getting a messageChar, making the keys the right length, and calling encode. Stops at end character.
fullEncode:
	PUSH		{r11, lr}				@ push lr to stack and unused register for alignment.

	MOV			r5, r0					@ key1Length
	MOV			r6, r2					@ key1
	MOV			r7, r1					@ key2Length
	MOV			r8, r3					@ key2
	
	MOV r10, #0							@ resets counter to 0
	
	fullEncodeLoop:
		BL			getchar				@ use getChar to get the char
		MOV			r4, r0				@ move messageChar into r4 for future use
		
		CMP			r4,#-1				@ compares messageChar to end character
		BEQ			fullEncodeEnd		@ if end character, go to function end
		
		CMP			r4, #97				@ compares messageChar to the letter a (97)
		ADDLT		r4,r4,#32			@ if it's less, add the offset between lower case and capital letters (32). So if it were the letter B, it would become b.

		CMP			r4, #122			@ compares the messageChar to z
		BGT			fullEncodeLoop   	@ if its past z, skip it and start the loop again
		
		CMP			r4,#97				@ compares the messageChar to a
		BLT			fullEncodeLoop		@ if its before a, skip it and start the loop again
	
		ADD			r2, r5, r10			@ key1Length + counter
		MOV			r0, r2				@ moves to r0 for mod
		
		MOV			r1, r5				@ key1Length
		Bl			mod					@ mod(r0) = key1Length+counter % key1Length
		
		LDRB		r1, [r6, r0]		@ r1 = key1[mod(r0)]
		
		MOV			r0, r4				@ r0 = current messageChar
		
		BL			encode				@ encode messageChar(r0) with keyChar(r1)
		MOV			r9, r0				@ Safe spot for half encoded character
		

		ADD			r2, r7, r10			@ key2Length + counter
		MOV			r0, r2				@ moves to r0 for mod
		
		MOV			r1, r7				@ key2Length
		BL mod							@ mod(r0) = key1Length+counter % key1Length
		
		LDRB		r1, [r8, r0]		@ r1 = key2[mod(r0)]
		
		MOV			r0, r9				@ messageChar = half encoded char
		
		BL			encode				@ encodes the half encoded char again with key2
		
		BL			putchar				@ outputs cipherChar 
		
		ADD			r10, r10, #1		@ increments counter
		
		B			fullEncodeLoop		@ label goes to start of loop
		
		fullEncodeEnd:					@ end function
			POP		{r11, lr}			@ pop lr and alignment register
			BX lr						@ go to lr (next instruction)
	
	


	
@void fullDecode(int key1Length, int key2Length, char *key1, char *key2) {
@    int counter = 0;
@    int cipherChar;
@    while (cipherChar = getchar()) {
@        getchar();
@        if (cipherChar < 97) {
@            cipherChar = cipherChar + 32;
@        }
@        if (cipherChar > 122) {
@            continue;
@        } else if (cipherChar < 97) {
@            continue;
@        } else {
@            int temp = key1Length + counter;
@            temp = temp % key1Length;
@            char keyChar = key1[temp];
@            char messageChar = decode(cipherChar, keyChar);
@            temp = key2Length + counter;
@            temp = temp % key2Length;
@            keyChar = key2[temp];
@            messageChar = decode(messageChar, keyChar);
@            putchar(messageChar);
@            counter++;
@        }
@        printf("\n");
@    }
@}

@ void fullDecode(int key1Length(r0), int key2Length(r1), char * key1(r2), char * key2(r3))
@ r4 is getChar char, cipherChar. r5 is key1Length. r6 is key1. r7 is key2Length. r8 is key2. r10 is counter.
@ fullDecode loops through getting a cipherChar, making the keys the right length, and calling decode. Stops at end character.
fullDecode:
	PUSH		{r11, lr}				@ push lr to stack and unused register for alignment.

	MOV			r5, r0					@ key1Length
	MOV			r6, r2					@ key1
	MOV			r7, r1					@ key2Length
	MOV			r8, r3					@ key2
	
	MOV				r10, #0				@ resets counter to 0
	
	fullDecodeLoop:
	
		BL			getchar				@ use getChar to get the char
		MOV			r4, r0				@ move cipherChar into r4 for future use
		
		CMP			r4,#-1				@ compares cipherChar to end character
		BEQ			fullDecodeEnd		@ if end character, go to function end
		
		CMP			r4, #97				@ compares cipherChar to the letter a (97)
		ADDLT		r4,r4,#32			@ if it's less, add the offset between lower case and capital letters (32). So if it were the letter B, it would become b.

		CMP			r4, #122			@ compares the cipherChar to z
		BGT			fullDecodeLoop   	@ if its past z, skip it and start the loop again
		
		CMP			r4,#97				@ compares the cipherChar to a
		BLT			fullDecodeLoop		@ if its before a, skip it and start the loop again

		ADD			r2, r5, r10			@ key1Length + counter
		MOV			r0, r2				@ moves to r0 for mod
		
		MOV			r1, r5				@ key1Length
		BL			mod					@ mod(r0) = key1Length+counter % key1Length

		LDRB		r1, [r6, r0]		@ r1 = key1[mod(r0)]
		
		MOV			r0, r4				@ r0 = current cipherChar
		
		BL decode						@ decode cipherChar(r0) with keyChar(r1)
		MOV			r9, r0				@ Safe spot for half decoded character
		
		
		ADD			r2, r7, r10			@ key2Length + counter	
		MOV			r0, r2				@ moves to r0 for mod
		
		MOV			r1, r7				@ key2Length
		BL			mod					@ mod(r0) = key1Length+counter % key1Length

		LDRB		r1, [r8, r0]		@ r1 = key2[mod(r0)]
		
		MOV			r0, r9				@ cipherChar = half decoded char
		
		BL			decode				@ decodes the half decoded char again with key2
		
		BL			putchar				@ outputs cipherChar 
		
		ADD			r10, r10, #1		@ increments counter
		
		B			fullDecodeLoop		@ label goes to start of loop

		fullDecodeEnd:					@ end function
			POP		{r11, lr}			@ pop lr and alignment register
			BX lr						@ go to lr (next instruction)




.global main



@int main(int argc, char **argv) {
@    char *key1 = argv[2];      // this is key1
@    char *key2 = argv[3];
@    int key1Length = strLength(key1);
@    int key2Length = strLength(key2);
@    int gcdCo = gcd(key1Length, key2Length);
@    int instr = (int) argv[1][0];
@    if (gcdCo == 1) {
@        if (instr == 48) {
@            fullEncode(key1Length, key2Length, key1, key2);
@        }
@        else if(instr == 49){
@            fullDecode(key1Length, key2Length,key1,key2);
@        }
@    } else {
@        printf("%s", "not co-prime...exiting");
@        exit(0);
@    }
@}

@ takes areguments, calls strLength and gcd and tests if keys are coprime. Either exits or continues. Will then determine whether or not to encode or decode.
@ r6 is argv pointer. r4 and r5 are temporary key lengths. r8 is the first argument.
main:
	PUSH		{r11, lr}				@ push lr to stack and unused register for alignment.
	MOV			r6, r1					@ r6 = argv pointer

	
	LDR			r0,[r6,#12]				@ r0 = argument 3 (key 2)
	BL			strLength				@ call strlength, returns length in r0
	MOV			r1,r0					@ r1 = key2Length for gcd
	MOV			r5, r0					@ r1 = key2Length
	
	LDR			r0,[r6,#8]				@ r0 = argument 2 (key 1)
	BL			strLength				@ r0 = key1Length for gcd
	MOV			r4, r0					@ r1 = key1Length
	

	BL			gcd						@ Call gcd, using key1Length(r0) and key2Length(r1). returns gcd in r0.
										@ r0 is the gcd. below is to be done in main, it compares r0 to 1 (if its coprime or not), and will either exit the program entirely, or continue.
	POP			{r11, lr}				@ pop lr and alignment register
	
	CMP			r0, #1					@ checks if gcd is 1

	BEQ			gcdTestSuccess			@ if gcd = 1 / if coprime, branch to success label
	B			gcdTestFail				@ if gcd != 1 / if not coprime, branch to fail label


    gcdTestFail:						@ A label, not a function. Used to print error and exit main.
		PUSH		{r11, lr}			@ continuation of main uses function so PUSH again

		LDR			r0, =euclidError	@ loads error message into r0
		bl			printf				@ prints r0 with printf function

		POP			{r11,lr}			@ end of main, pop lr and alignment register
		BX lr							@ exit main



	gcdTestSuccess:						@ A label, not a function. Determines whether or not to encode or decode, and calls the correct function to do so.
		PUSH		{r11, lr}			@ continuation of main uses function so PUSH again

		MOV			r0, r4				@ r0 = key1Length
		MOV			r1, r5				@ r1 = key2Length
		LDR			r2, [r6, #8]		@ r2 = key1
		LDR			r3, [r6, #12]		@ r3 = key2

		LDR			r8, [r6, #4]		@ r8 = argument 1
		LDRB		r8, [r8]			@ r8 = contents of argument 1 (0 or 1)
		CMP			r8, #48				@ compares to ascii 0
		BEQ			fullEncode			@ if 0, runs encode(r0, r1, r2, r3)
		CMP			r8, #49				@ compares to ascii 1
		BEQ			fullDecode			@ if 1, runs decode(r0, r1, r2, r3)
  
		POP			{r11,lr}			@ end of main, pop lr and alignment register
		BX lr							@ exit main
