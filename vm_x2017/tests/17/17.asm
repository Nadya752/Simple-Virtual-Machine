FUNC LABEL 0
    MOV STK A VAL 5
    REF REG 0 STK A
    CAL VAL 1
    PRINT STK A
    RET
FUNC LABEL 1
    MOV REG 1 VAL 0
    MOV STK A REG 0
    EQU REG 0
    NOT REG 1
    ADD REG 0 REG 1
    MOV PTR A REG 0
    RET
