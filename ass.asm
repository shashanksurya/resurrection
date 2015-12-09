go    ld#  r0,0      register 0 will hold the sum, init it
      ld#  r1,nums   register 1 points to the numbers to add
      ld#  r2,1      register 2 holds the number one.
loop  ldi  r3,r1     get next number into register 3
      jz   r3,done   if its zero we're finished
      add  r0,r3     otherwise add it to the sum
      add  r1,r2     add one to register one (next number to load)
      jmp  loop      go for the next one
done  hlt  00        all done. sum is in register 0
nums  123            the numbers to add
      234
      345
        0            end of the list