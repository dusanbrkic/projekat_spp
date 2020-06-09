.globl main

.data

m1:              .word 6
m2:              .word 5
m3:              .word 0

.text
main:
                  la                  $t0, m1
                  lw                  $t2, 0($t0)
                  la                  $t0, m2
                  lw                  $t0, 0($t0)
                  li                  $t1, 1
                  li                  $t3, 0
lab:
                 add                  $t3, $t3, $t2
                 sub                  $t4, $t1, $t0
                addi                  $t1,$t1, 1
                bltz                  $t4, lab
                  la                  $t0, m3
                  sw                  $t3, 0($t0)
                 nop
