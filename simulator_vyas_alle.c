/* Name and CSU ID

----------------------
NAME: Amit Vyas
CSU ID: 2828723
----------------------
Name: Sai Kiran Alle
CSU ID: 2823102
-----------------------

*/



/*
The program works correctly and all the instructions R-Type and I-Type have been implemented.
The code worked correclty with basic.out and memory.out. Didn't try with branching and logical shifting.
We have implemented foward braching(adding for new loaction) as well as backward branching(subracting from current PC for new backward loaction).
I have updated data in rd1,rd2 etc but for instruction I am adding/ updating data into register memory directly. 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int cycles =1000;
int PC=512;
int regArray[32];
int inst;
int memArray[256];


//0-128 data segment and 128-256 is text segment 
//***  PC is in bytes so we have to divide it by 4.
// *****   immediate is the 16 bit offset in I type instruction *******


void printResult();
void initialize();
void bin();
void performOperation();
void updateMemory();
void updatePC();
void addi();
void add();
void sub();
void sw();
void lw();
void beq();
void sll();
void srl();
void slt();
void bne();


int aluOut;
int wd;
int wn;
int regDest ;
int aluSrc;
int memToReg;
int regWrite;
int memRead;
int memWrite;
int branch;
int aluOp1;
int aluOp2;
int branchNE;
int jump;
int rd1;
int rd2;
int rd,rs,rt;
int op;
int ctrl;
int clock;
int immediate,funct;
int pc4,offset,zero;
int pc_next,pc_src;


void initialize(char *argv[]){
    // Reg array initialised to 0
    for (int i=0;i<32;i++){
        regArray[i] = 0;
    }

    // Reading file
    FILE *readFile;
    readFile = fopen(argv[1],"rb");
    for (int j=0;j<256;j++){
        fread(&memArray[j], sizeof(int),1,readFile);
    }
    // Closing the file
    fclose(readFile);

    // Taking the 1st instruction
    inst = memArray[PC/sizeof(int)];

}
void performOperation(){
    op = ((inst >> 26) & 63);

    rs = ((inst >> 21) & 31);
    rt = ((inst >> 16) & 31);

    immediate = (inst& 65535);
    funct = (inst & 63);

    // ctrl for R-type instruction
    if ( op == 0){
        ctrl = 1416;
        if (funct == 32){
            aluOut= rs + rt;   //calclutate the ALUOut here
            add();
        
        }
        else if(funct == 34){
            aluOut= rs-rt ;   //calclutate the ALUOut here
            sub();
        }

        else if(funct == 0){
            int aluOut = rs << rt;
            sll();
        }

        else if(funct == 2){
            aluOut = rs >> rt;
            srl();
        }

        else if(funct == 42){
            aluOut = rs - rt;
            slt();
        }

        else if(funct == 12){
            aluOut = 0;
        }        
    }

    // ctrl for addi
    else if (op == 8){
        ctrl = 896;
        addi();
    }
    // ctrl for lw
    else if (op == 35){
        ctrl = 704;
        lw();
    }

    // ctrl for sw
    else if (op == 43){
        ctrl= 544 ;
        aluOut = regArray[rt] + immediate;
        sw();
    }   

    // ctrl for BEQ
    else if (op == 4 ){
        ctrl = 20 ;
        aluOut =0;
        beq();
    }

    // ctrl for BNE
    else if (op == 5 ){
        ctrl = 6;
        aluOut = 0;
        bne();
    }

    if (aluOut == 0){
        zero = 1;
    }
    else{
        zero = 0;
    }

    regDest = (ctrl >> 10 ) & 1;
    aluSrc = (ctrl >> 9 ) & 1;
    memToReg = (ctrl >> 8) & 1;
    regWrite = (ctrl >> 7 ) & 1;
    memRead = (ctrl >> 6 ) & 1;
    memWrite = (ctrl >> 6) & 1;
    branch = (ctrl >> 4 ) & 1;
    aluOp1 = (ctrl >> 3 ) & 1;
    aluOp2 = (ctrl >> 2 ) & 1;
    branchNE = (ctrl >> 1 ) & 1;
    jump = ctrl & 1;
    rd1 = rs;
    rd2 =rt;
    offset = 512;
    pc_src = ((branch & zero) || (branchNE & !zero));
    

    if(regDest == 0){
        wn = 0;
    }
    else{
        wn = 1;
    }

}

 void addi(){ 
    regArray[rt]+= immediate;
 }

 void beq(){

     if ((regArray[rd] - regArray[rs] ) == 0){
        // forward branching
        if(immediate > 0){
            PC = PC + 4 + immediate * 4;
            inst = memArray[PC];
        }
        // Backward branching 
        else{
            PC = PC - 4 - immediate * 4;
            inst = memArray[PC];
        }
        
     }

 }

 void lw(){
    regArray [rs] = memArray [regArray[rt]+immediate];    
 }

 void sw(){
    memArray [regArray[rt]+immediate]= regArray [rs];
}

 void add(){
    rd = ((inst >> 11) & 31);
    regArray[rd]+= regArray[rs] + regArray[rt];

 }

 void sub(){
    regArray[rd] = regArray[rs] - regArray[rt];
  
 }


void updateMemory(){
    //check with memwrite and update memArray[aluOut]
    if(memWrite){
        memArray[aluOut + immediate] = regArray[rs] ;
    }
}

void srl(){

    regArray[rd] = rs >> rt;

}

void sll(){
    regArray[rd] = rs << rt;
}

void slt(){
    if(rs < rt){
        regArray[rd] = 1;
    }
    else{
        regArray[rd] = 0;
    }
}

void bne(){
    if (rs != rt){
        PC = PC +4 + immediate * 4;
        inst = memArray[PC];
    }

}

void updatePC(){
    PC = PC + 4;
}

void printResult(){
    printf("\n");
    printf("Cycle = %d\n", clock);
    printf("PC = %d \n",PC);
    printf("DM: ");
    for (int i =0;i<16;i++){
        printf(" %d ", memArray[i]);
    }
    printf("\n");
    printf("RegFile: ");
    for (int j =0;j<16;j++){
        printf(" %d ", regArray[j]);
    }
    printf("\n---------------------------------------------------------\n")
;}


int main(int argc, char *argv[]){

    while( inst != 12){
        clock++;
        initialize(argv); 
        performOperation();
        updateMemory();
        updatePC();
        printResult();
    }

    return 0;
}