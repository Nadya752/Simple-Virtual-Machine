FUNC LABEL 0
    MOV STK A VAL 0
    MOV STK B VAL 1
    MOV PTR A VAL 7
    PRINT STK A
    MOV PTR A PTR B
    PRINT STK A
    MOV REG 0 VAL 8
    MOV PTR A REG 0
    PRINT STK A
    MOV PTR A STK B
    PRINT STK A
    RET
