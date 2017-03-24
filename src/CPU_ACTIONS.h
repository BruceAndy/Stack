#ifdef CPU_CMDS

CPU_CMD(PUSH, 1)

CPU_CMD(JMP, 3)

CPU_CMD(POP, 1)

CPU_CMD(ADD, 0)

CPU_CMD(MUL, 0)

CPU_CMD(SUB, 0)

CPU_CMD(JUMP, 1)

CPU_CMD(CALL, 1)

CPU_CMD(SIN, 0)

CPU_CMD(COS, 0)

CPU_CMD(OUTR, 1)

CPU_CMD(INR, 1)

CPU_CMD(JG, 3) //takes arguments wich are to be compared and addr to jump

CPU_CMD(JL, 3)

CPU_CMD(JBE, 3)

CPU_CMD(JLE, 3)


#endif
