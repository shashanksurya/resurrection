.text
        addu    $s0,$zero,$zero         # s0 = 0
        addu    $s1,$zero,$zero         # s1 = 0
        addiu   $v0,$zero,5             # v0 = read value
        syscall
        sw      $v0,n($gp)              # M[n] = v0
L1:     lw      $s2,n($gp)              # s2 = M[n]
        slt     $t0,$s1,$s2             # if s1 >= s2 then
        beq     $t0,$zero,L2            #    goto L2
        addiu   $v0,$zero,5             # v0 = read value
        syscall
        addu    $s0,$s0,$v0             # s0 += v0
        addiu   $s1,$s1,1               # s1 += 1
        j       L1                      # goto L2
L2:     addu    $a0,$s0,$zero           # a0 = s0
        addiu   $v0,$zero,1             # print a0
        syscall
        addiu   $v0,$zero,10            # exit
        syscall
        .data
n:      .word   0
