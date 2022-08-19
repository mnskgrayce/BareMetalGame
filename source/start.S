// ----------------------------------- start.S -------------------------------------
.section ".text.boot" // Linked puts this at the start of kernel image

.global _start // Execution starts here

_start:
    // Check processor ID is zero (executing on main core), else hang
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    
    // Not in main core, hang in infinite wait loop
1:  wfe
    b       1b
    
    // In main core!
2:  ldr     x1, =_start // Set stack to start below our code
    mov     sp, x1

    // Clean BSS section
    ldr     x1, =__bss_start // Start address
    ldr     w2, =__bss_size // Size of section

3:  cbz     w2, 4f // Quit loop if zero
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b // Loop if non-zero

    // Jump to main() routine in C (make sure it doesn't return)
4:  bl      main
    // If main returns, halt the master core
    b       1b

    
