.globl main
.text




main:
    li s0, 1
    li   a7, 1
    j loop     

loop:
    # Читаем первый ряд ключей (SWITCHES_0)
    lw   t1, SWITCHES_0_BASE

    # Читаем второй ряд ключей (SWITCHES_1)
    lw   t2, SWITCHES_1_BASE

    # Проверяем 9-й ключ (бит 8) на любом из рядов
    andi t5, t1, 0xFF
    andi t6, t2, 0xFF
    add  a0, t5, t6
    
    ecall
    
    andi t3, t1, 0x100
    bnez t3, preend
    andi t3, t2, 0x100
    bnez t3, preend
    
    j loop

preend:
    beqz s0, end
    andi t1, t1, 0xFF
    andi t2, t2, 0xFF
    add  a0, t1, t2   
    ecall
    j end
end:
   j    end                