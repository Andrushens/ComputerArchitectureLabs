
#ifndef RISCV_SIM_EXECUTOR_H
#define RISCV_SIM_EXECUTOR_H

#include "Instruction.h"

class Executor
{
public:
    void Execute(InstructionPtr& instr, Word ip)
    {
        /* YOUR CODE HERE */
        ProcessAlu(instr, ip);
        SetNextIp(instr, ip);
    }

private:
    /* YOUR CODE HERE */
    void ProcessAlu(InstructionPtr& instr, Word ip){
        Word a = instr->_src1Val;
        Word b = instr->_imm.value_or(instr->_src2Val);
        Word result;

        switch(instr->_type){
            case IType::Csrr:
                instr->_data = instr->_csrVal;
                return;

            case IType::Csrw:
                instr->_data = instr->_src1Val;
                return;

            case IType::St:
                instr->_data = instr->_src2Val;
                break;

            case IType::J:

            case IType::Jr:
                instr->_data = ip + 4;
                return;

            case IType::Auipc:
                instr->_data = ip + instr->_imm.value();   
                return;
        }

        switch (instr->_aluFunc){
            case AluFunc::Add:
                result = a + b;
                break;

            case AluFunc::Sub:
                result = a - b;
                break;

            case AluFunc::And:
                result = a & b;
                break;

            case AluFunc::Or:
                result = a | b;
                break;

            case AluFunc::Xor:
                result = a ^ b;
                break;

            case AluFunc::Slt:
                result = (SignedWord)a < (SignedWord)b;
                break;

            case AluFunc::Sltu:
                result = a < b;
                break;

            case AluFunc::Sll:
                result = a << b % 32;
                break;

            case AluFunc::Srl:
                result = a >> b % 32;
                break;

            case AluFunc::Sra:
                result = (SignedWord)a >> (SignedWord)b % 32;
                break;
        }

        if (instr->_type == IType::Ld || instr->_type == IType::St){
            instr->_addr = result;
        }
        else{
            instr->_data = result;
        }
    }

    void SetNextIp(InstructionPtr& instr, Word ip){
        if (CheckCondition(instr)){
            switch(instr->_type){
                case IType::Br:
            
                case IType::J:
                    instr->_nextIp = ip + instr->_imm.value();
                    return;

                case IType::Jr:
                    instr->_nextIp = instr->_src1Val + instr->_imm.value();
                    return;
            }
        }

        instr->_nextIp = ip + 4;
    }

    bool CheckCondition(InstructionPtr& instr){
        Word a = instr->_src1Val;
        Word b = instr->_src2Val;

        switch(instr->_brFunc){
            case BrFunc::Eq:
                return a == b;
                
            case BrFunc::Neq:
                return a != b;
                
            case BrFunc::Lt:
                return (SignedWord)a < (SignedWord)b;
                
            case BrFunc::Ltu:
                return a < b;
                
            case BrFunc::Ge:
                return (SignedWord)a >= (SignedWord)b;
                
            case BrFunc::Geu:
                return a >= b;
                
            case BrFunc::AT:
                return true;
                
            case BrFunc::NT:
                return false;
        }

        return false;
    }
    
};

#endif // RISCV_SIM_EXECUTOR_H
