#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#define READ 10
#define WRITE 11
#define LOAD 20
#define STORE 21
#define ADD 30
#define SUBTRACT 31
#define DIVIDE 32
#define MULTIPLY 33
#define BRANCH 40
#define BRANCHNEG 41
#define BRANCHZERO 42
#define HALT 43
#define FINISH -99999
#define MEMORY_INSTALLED 100
#define EMPTY 0
#define INTEGER 9999

typedef struct regs
{	
	/*registrador do acumulador*/
	int accumulator;
	
	/*registrar o local na memória que contém a instrução que está sendo executada*/
	int instructionCounter;

	
	int instructionRegister;

	/*indicar a operação que está sendo executada no momento*/
	int operationCode;

	int operand;

}s_regs;


void load_process ();
bool load_process_file (char *name);
void initialize ();
void dump_simpletron();
void dump_simpletron_in_file();

void execute();
void write();
unsigned read();
void load();
void store();
unsigned add();
unsigned subtract();
unsigned multiply();
void branch();
void branchneg();
void branchzero();
unsigned divide();
unsigned halt();



int memory[MEMORY_INSTALLED];
FILE *in = NULL;
FILE *out = NULL;
s_regs r;


int main( int argc, char **argv)
{
	
	initialize();
	
	printf("\n\n*** Bem vindo ao Simpletron! ***\n\n");

	if(argc == 1){	
		printf("*** Favor digitar seu programa, uma instrução ***\n\n");
		printf("*** (ou palavra de dados) por vez. Mostrarei ***\n\n");
		printf("*** o número do local e uma interrogação (?). ***\n\n");
		printf("*** Você, então, deverá digitar a palavra para esse ***\n\n");
		printf("*** local. Digite a sentinela -99999 para ***\n\n");
		printf("*** encerrar a entrada do seu programa.***\n\n");

		load_process();
	}
	else if(argc == 2){
		
		if(!load_process_file(argv[1]))
			return 0;
	}

	printf("\n\n*** Carga do programa concluída***\n\n");
	printf("*** Iniciando execução do programa ***\n\n");
	
	out = fopen("out.txt", "w+");

	execute();

	dump_simpletron();
	
	dump_simpletron_in_file();
    
	return 0;
}


void initialize ()
{
	unsigned i = 0;
 	memset(&r, 0, sizeof(r));
 	for(i = 0;i < MEMORY_INSTALLED;++i) memory[i] = EMPTY;

}

void load_process(){
	
	int instruction = 0;
	int counter = 0;
	int validInstruction = 0;
	

	do{
		do{
			if(instruction >= INTEGER || instruction <= -INTEGER)
				printf("\n***Informe o núemro novamente***\n");

			printf("\n%02d ? ",counter);
			validInstruction = scanf("%d",&instruction);

		}while((instruction >= INTEGER || instruction <= -INTEGER) && instruction != FINISH);
		
		if(instruction != FINISH)
			memory[counter++] = instruction;

	}while(instruction != FINISH && validInstruction == 1);	

}

bool load_process_file (char *name)
{

	if((in = fopen(name,"r")) == NULL)
	{
		printf("Arquivo %s.sml nao encontrado!!\n",name);
		return false;
	}

	fread(&memory, sizeof(int), MEMORY_INSTALLED, in);

	fclose(in);

	return true;
}

void execute()
{
	unsigned done = 1;
	
	do
	{
		if(r.instructionCounter >= 0 && r.instructionCounter < MEMORY_INSTALLED)
		{
			r.instructionRegister = memory[r.instructionCounter];

			r.operationCode = r.instructionRegister/100;
			r.operand = r.instructionRegister % 100;

			
			switch(r.operationCode)
			{
				case READ: 
					if((done = read())) 
						++r.instructionCounter;
				break;

				case WRITE: 
					write(); 
					++r.instructionCounter;
				break;

				case LOAD: 
					load(); 
					++r.instructionCounter;
				break;

				case STORE: 
					store();
					++r.instructionCounter;					
				break;

				case ADD: 
					if((done = add())) 
						++r.instructionCounter;
				break;
				
				case SUBTRACT: 
					if((done = subtract()))
						++r.instructionCounter; 
				break;
				
				case DIVIDE: 
					if((done = divide()))
						++r.instructionCounter; 
				break;
				
				case MULTIPLY: 
					if((done = multiply()))
						++r.instructionCounter;
				break;
				
				case BRANCH: 
					branch(); 
				break;

				case BRANCHNEG: 
					branchneg(); 
				break;
				case BRANCHZERO: 
					branchzero(); 
				break;
				
				case HALT: 
					if(!(done = halt()))
						++r.instructionCounter;
				break;
				
				default:
					printf("\n*** INVALID OPCODE: %d ***\n",r.operationCode);
					done = 0;
				break;
			}
		}
		else
		{
			printf("\nABEND: ***addressability error\n");
			done = 1;
		}

	}while(done != 0);
}

void write()
{
	printf("\n%d\n",memory[r.operand]);
	fprintf(out, "\n%d\n",memory[r.operand]);
}

unsigned read()
{
	int wrd = 0;

	printf("\nEntre com um número inteiro: ");
	scanf("%d",&wrd);

	if(wrd > INTEGER)
	{
		printf("\nreadtoobid.sml ***ABEND: entrada ilegal***\n");
		return 0;
	}

	if(wrd < -INTEGER)
	{
		printf("\nreadtoosmall.sml ***ABEND: ilegal input***\n");
		return 0;
	}

	memory[r.operand] = wrd;


	return 1;

}

void load()
{
	r.accumulator = memory[r.operand];
}

void store()
{
	memory[r.operand] = r.accumulator;
}

unsigned add()
{
	if(r.accumulator+memory[r.operand]< -INTEGER)
	{
		printf("\nunderflow.add.sml ***ABEND: underflow ***\n");
		return 0;
	}
	if(r.accumulator+memory[r.operand] > INTEGER)
	{
		printf("\noverflow.add.sml ***ABEND: overflow ***\n");
		return 0;
	}

	r.accumulator += memory[r.operand];
	return 1;
}


unsigned subtract()
{
	if(r.accumulator - memory[r.operand] < -INTEGER)
	{
		printf("\nunderflow.sub.sml ***ABEND: underflow ***\n");
		return 0;
	}
	if(r.accumulator - memory[r.operand] > INTEGER)
	{
		printf("\noverflow.sub.sml ***ABEND: overflow ***\n");
		return 0;
	}

	r.accumulator = memory[r.operand] - r.accumulator;

	return 1;
}
unsigned multiply()
{
	if(r.accumulator*memory[r.operand] < -INTEGER)
	{
		printf("\nunderflow.mult.sml ***ABEND: underflow ***\n");
		return 0;
	}
	if(r.accumulator*memory[r.operand] > INTEGER)
	{
		printf("\noverflow.mult.sml ***ABEND: overflow ***\n");
		return 0;
	}

	r.accumulator*=memory[r.operand];
	return 1;
}

void branch()
{
	r.instructionCounter = r.operand;
}

void branchneg()
{
	if(r.accumulator < 0) r.instructionCounter = r.operand;
	else r.instructionCounter += 1;
}

void branchzero()
{
	if(r.accumulator == 0) r.instructionCounter = r.operand;
	else r.instructionCounter += 1;
}

unsigned divide()
{

	if(memory[r.operand] == 0)
	{
		printf("\ndiv0.sm ***ABEND: attempted division by 0 ***l\n");
		return 0;
	}

	r.accumulator = memory[r.operand] / r.accumulator;
	return 1;
}

unsigned halt()
{
	printf("\n*** Execução do Simpletron encerrada ***\n");
	return 0;
}


void dump_simpletron()
{
	int i = 0,j = 0;

	printf("\nREGISTERS:");
	printf("\naccumulator:        \t%i",r.accumulator);
	printf("\ninstructionCounter: \t%i",r.instructionCounter);
	printf("\ninstructionRegister:\t%i",r.instructionRegister);
	printf("\noperationCode:      \t%i",r.operationCode);
	printf("\noperand:            \t%i",r.operand);

	printf("\n\nMEMORY:\n\n");

	for(i = 0;i < 10;++i)
		printf("\t%d",i+j);		

	printf("\n");

	for(j = 0;j < MEMORY_INSTALLED;j+=10)
	{	
		printf("%d\t", j);	
		for(i = 0;i < 10;i++)
			printf("%04d\t",memory[i+j]);

		printf("\n\n");
		
	}

}

void dump_simpletron_in_file()
{
	int i = 0,j = 0;    
        
	fprintf(out, "\nREGISTERS:");
	fprintf(out, "\naccumulator: \t%d",r.accumulator);
	fprintf(out, "\ninstructionCounter: \t%d",r.instructionCounter);
	fprintf(out, "\ninstructionRegister:\t%d",r.instructionRegister);
	fprintf(out, "\noperationCode:      \t%d",r.operationCode);
	fprintf(out, "\noperand:            \t%d",r.operand);

	fprintf(out, "\n\nMEMORY:\n\n");

	for(i = 0;i < 10;++i){
		fprintf(out, "\t%d",i+j);
        }		

	fprintf(out, "\n");

	for(j = 0;j < MEMORY_INSTALLED;j+=10)
	{	
		fprintf(out, "%d\t", j);	
		for(i = 0;i < 10;i++)
			fprintf(out, "%04d\t",memory[i+j]);

		fprintf(out, "\n\n");
		
	}

}
