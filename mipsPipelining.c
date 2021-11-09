
/*
The instruction fetch and decode is working.
Evertyhing is good but not everthing is pipelined yet :(

*/




#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// global variable declarations
int reg[32], memory[256], PC, instIF;
int rs, rt, rd, opcode;
int clock=0;

int cycles =1000;

int regdest, aluop0, aluop1, alusrc, branch, memread, memwrite, regwrite, memtoreg;

int wbidex, midex, exidex;
int wbexmem, mexmem;

int immediate;

// IF/ID pipeline registers 
int ifidInst =0, ifidPC4 =0;

// ID/EX pipeline registers
int idexPC4, idexrd1=0, idexrd2=0, idexrs=0, idexrt=0, idexrd=0, idexCtrl=0, idexExtend=0;

// EX/MEM pipeline registers
int exmembtgt=0, exmemZero=0, exmeAluout=0, exmemrd2=0, exmemRegrd=0, exmemCtrl=0;

// MEM/WB pipeline registers
int memwbMemout=0, memwbAlout=0, memwbRegrd=0, memwbCtrl=0;

int extend;

int regWriteFlag= 0;
int exFlag =0;
int instEx;
int ifFlag=0;


int exefunct;

//Fucntion decalartions
void initialize(char *argv[]);
void readPiplineRegisters();
void carryoutOperations();
void updateMemory();
void updatePipelineRegisters();
void printResult();
void updatePC();


void initialize(char *argv[]){
    // Reg array initialised to 0
    // for (int i=0;i<32;i++){
    //     reg[i] = 0;
    // }

    // Reading file
    FILE *readFile;
    readFile = fopen(argv[1],"rb");
    for (int j=0;j<256;j++){
        fread(&memory[j], sizeof(int),1,readFile);
    }

    //  for(int i=128;i<256;i++){
    //     printf("%i  --  %d \n",i, memory[i]);
    // }

    // Closing the file
    fclose(readFile);
    instIF = memory[PC/4];

}

void readPipelineRegisters(){
    ifidInst = instIF;
    ifidPC4 = PC;

    idexPC4 = 0, idexrd1= 0, idexrd2=0, idexrs=0, idexrt=0, idexrd=0, idexCtrl=0, idexExtend=0;

    exmembtgt=0, exmemZero=0, exmeAluout=0, exmemrd2=0, exmemRegrd=0, exmemCtrl=0;

    memwbMemout=0, memwbAlout=0, memwbRegrd=0, memwbCtrl=0;
}

void carryoutOperations(){

    ifidInst = instIF;
    ifidPC4 = PC+4;

    idexPC4 = 0;

    opcode = ((instIF >> 26) & 63);
    if(opcode == 0)
        exmemCtrl = 1416;
    else if(opcode == 8) //addi
        exmemCtrl = 896;
    else if(opcode == 35)
        exmemCtrl = 704; //lw
    else if(opcode == 43)
        exmemCtrl = 544; //sw
   



    // ID/EXE
        idexPC4 = ifidPC4;
        instEx = instIF;
    

    //if(exFlag == 1){
        idexPC4 += 4;
        opcode = ((instIF >> 26) & 63);

        idexrd = ((instIF >> 11) & 31);
        idexrt = ((instIF >> 16) & 31);
        idexrs = ((instIF >> 21) & 31);

        //immediate = (instIF & 65535);

        exefunct = (instIF & 63);

        //printf("----exefucnt is: %d \n", exefunct);
        if(opcode == 0){
            exmemCtrl = 1416;
            regdest = (exmemCtrl >> 10 ) & 1;
            aluop0 = (exmemCtrl >> 3 ) & 1;
            aluop1= (exmemCtrl >> 2 ) & 1;
            alusrc= (exmemCtrl >> 9 ) & 1;
            branch= (exmemCtrl >> 4 ) & 1;
            memread= (exmemCtrl >> 6 ) & 1;
            memwrite= (exmemCtrl >> 5 ) & 1;
            regwrite= (exmemCtrl >> 7 ) & 1;
            memtoreg= (exmemCtrl >> 8 ) & 1;

            idexrd1 = idexrs;
            idexrd2 = idexrt;
            
            if (exefunct == 32){
                exmeAluout = reg[idexrs] + reg[idexrt];
                reg[idexrd] = exmeAluout;
                if (exmeAluout ==0)
                    exmemZero = 1;
            }

            else if (exefunct == 34){
                exmeAluout = reg[idexrs] - reg[idexrt];
                reg[idexrd] = exmeAluout;
                if (exmeAluout ==0)
                    exmemZero = 1;
            }

            else if (exefunct == 0){
                exmeAluout = reg[idexrs] << reg[idexrt]; //sll
                reg[idexrd] = exmeAluout;
                if (exmeAluout ==0)
                    exmemZero = 1;
            }

            else if (exefunct == 2){
                exmeAluout = reg[idexrs] >> reg[idexrt]; //srl
                reg[idexrd] = exmeAluout;
                if (exmeAluout ==0)
                    exmemZero = 1;
            }

            else if (exefunct == 42){
                exmeAluout = reg[idexrs] < reg[idexrt] ? 1 : 0 ; //ternary opeartion slt
                reg[idexrd] = exmeAluout;
                if (exmeAluout ==0)
                    exmemZero = 1;
            }

            else if (exefunct == 12){
                exmeAluout = 0 ; //halt instruction.
            }

        }
        else if( opcode == 8 ){
            //addi
            exmemCtrl = 896;

            immediate = (instIF & 65535);

            regdest = (exmemCtrl >> 10 ) & 1;
            aluop0 = (exmemCtrl >> 3 ) & 1;
            aluop1= (exmemCtrl >> 2 ) & 1;
            alusrc= (exmemCtrl >> 9 ) & 1;
            branch= (exmemCtrl >> 4 ) & 1;
            memread= (exmemCtrl >> 6 ) & 1;
            memwrite= (exmemCtrl >> 5 ) & 1;
            regwrite= (exmemCtrl >> 7 ) & 1;
            memtoreg= (exmemCtrl >> 8 ) & 1;
            extend = immediate; 
        
            exmeAluout = immediate;
            reg[idexrt] = immediate;
            if (exmeAluout ==0)
                exmemZero = 1; 
            

        }
        else if( opcode == 35){
            //lw
            exmemCtrl = 704;

            regdest = (exmemCtrl >> 10 ) & 1;
            aluop0 = (exmemCtrl >> 3 ) & 1;
            aluop1= (exmemCtrl >> 2 ) & 1;
            alusrc= (exmemCtrl >> 9 ) & 1;
            branch= (exmemCtrl >> 4 ) & 1;
            memread= (exmemCtrl >> 6 ) & 1;
            memwrite= (exmemCtrl >> 5 ) & 1;
            regwrite= (exmemCtrl >> 7 ) & 1;
            memtoreg= (exmemCtrl >> 8 ) & 1;
            reg[rs] = memory[reg[idexrt] + immediate];
            memwbRegrd = rs;
        }
        else if( opcode ==43){
            //sw
            exmemCtrl =544;
            
            regdest = (exmemCtrl >> 10 ) & 1;
            memwbRegrd = regdest;
            aluop0 = (exmemCtrl >> 3 ) & 1;
            aluop1= (exmemCtrl >> 2 ) & 1;
            alusrc= (exmemCtrl >> 9 ) & 1;
            branch= (exmemCtrl >> 4 ) & 1;
            memread= (exmemCtrl >> 6 ) & 1;
            memwrite= (exmemCtrl >> 5 ) & 1;
            regwrite= (exmemCtrl >> 7 ) & 1;
            memtoreg= (exmemCtrl >> 8 ) & 1;
           // memory[reg[idexrt]+immediate] = reg[idexrs];
        }

    exmembtgt = PC + (immediate << 2);

}

void updateMemory(){
    if(exmemCtrl == 544)
    memory[reg[idexrt]+immediate] = reg[idexrs];
       
}

void updatePC(){
    PC+= 4;
}

void printResult(){
    printf("\n");
    printf("Cycle = %d\n", clock);
    printf("PC = %d \n",PC);
    printf("DM: ");
    for (int i =0;i<16;i++){
        printf(" %d ", memory[i]);
    }
    printf("\n");
    printf("RegFile: ");
    for (int j =0;j<16;j++){
        if (clock == 5)
            printf(" %d ", reg[j]);
        else
            printf(" %d ", reg[0]);
    }
    printf("\n");
    printf("IF/ID (pc4, inst) %d %d", ifidPC4, instIF);
    printf("\n");
    printf("ID/EX (pc4, rd1,rd2,extend,rs,rt,rd,ctrl) %d %d %d %d %d %d %d %d", PC+4, idexrd1, idexrd2,extend, idexrs, idexrt,idexrd,exmemCtrl);
    printf("\n");
    printf("EX/MEM (btgt, zero,ALUout, rd2, RegRd, ctrl) %d %d %d %d %d %d", exmembtgt, exmemZero,exmeAluout,exmemrd2,exmemRegrd, exmemCtrl);
    printf("\n");
    printf("MEM/WB (memout, ALUout,RegRd, ctrl) %d %d %d %d", memwbMemout, memwbAlout,memwbRegrd, exmemCtrl);
    printf("\n");

    printf("\n---------------------------------------------------------\n")
;}



int main(int argc, char *argv[]){

   
    PC = 512;
    while( instIF != 12 ){    
        clock++;
        initialize(argv); 
        readPipelineRegisters();
        carryoutOperations();
        updateMemory();
        printResult();
        updatePC();
    }
    if(instIF == 12){
       
        cycles = 3;
        for(int i=0;i<4;i++){
            cycles--;
            initialize(argv); 
            instIF = memory[PC/sizeof(int)];
            readPipelineRegisters();
            carryoutOperations();
            updateMemory();
            clock++;
            if(clock <=5)
            printResult();
            updatePC();
        }
    }

        return 0;
}
