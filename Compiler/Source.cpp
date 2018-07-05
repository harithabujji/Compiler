#define _CRT_SECURE_NO_WARNINGS
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
FILE *fp, *fw;
int AX, BX, CX, DX, EX, FX, GX, HX;
int *stack = (int *)calloc(6,sizeof(int)); int top = -1;
struct symbol
{
	char *name;
	int address;
	int size;
};
struct label
{
	char *blockname;
	int address;
};
struct intermediate
{
	int inNo;
	int opcode;
	int *parameters;
};
int checkDataInstruction(char *instruction)
{
	int noofsymbols = -1;
	char *text = (char*)malloc(sizeof(char) * 50);
	strcpy(text, instruction);
	char *token = strtok(text, " ");
	while (token != NULL)
	{
		printf("%s\n", token);
		token = strtok(NULL, " ");
	}
	if (noofsymbols > 1 || noofsymbols<0)
	{
		return 0;
	}
	return 1;
}
int addToSymbolTable(struct symbol** symboltable, char *instruction, int len, int currentaddr,int *memory)
{
	char *keyword; int i = 0;
	char *operation;
	char *text = (char*)malloc(sizeof(char) * 50);
	strcpy(text, instruction);
	keyword = strtok(text, " ");
	if (strcmp(keyword,"DATA")==0)
	{
		operation = strtok(NULL, " ");
		strtok(operation, "\n");
		if (strlen(operation) == 1)
		{
			symboltable[len]->name = operation;
			symboltable[len]->address = currentaddr;
			symboltable[len]->size = 1;
			return 1;
		}
		else
		{
			int strt = 0, size = 0;
			while (i < strlen(operation))
			{
				if (operation[i] == '[')
				{
					strt = 1;
					i++;
				}
				if (strt == 1)
				{
					while (operation[i] != ']'&&i < strlen(operation))
					{
						size = size * 10 + (operation[i] - '0');
						i++;
					}
					strt = 0;
				}
				else
					i++;

			}
			symboltable[len]->address = currentaddr;
			symboltable[len]->size = size;
			strtok(operation, "[");
			symboltable[len]->name = operation;
			return size;
		}
	}
	else
	{
		int strt = 0,num=0;
		operation = strtok(NULL, " ");
		strtok(operation, "\n");
		while (i < strlen(operation))
		{
			if (operation[i] == '=')
			{
				strt = 1;
				i++;
			}
			if (strt == 1)
			{
				while (i < strlen(operation))
				{
					num = num * 10 + (operation[i] - '0');
					i++;
				}
				strt = 0;
			}
			else
				i++;
		}
		symboltable[len]->address = currentaddr;
		symboltable[len]->size = 0;
		strtok(operation, "=");
		symboltable[len]->name = operation;
		memory[currentaddr] = num;
		return 0;
	}
}
void printSymbolTable(struct symbol **symboltable, int len)
{
	int i = 0;
	for (i = 0; i < len; i++)
	{
		printf("\n%s", symboltable[i]->name);
		printf("\t%d", symboltable[i]->address);
		printf("\t%d\n", symboltable[i]->size);
	}
}
int isRegister(char *reg)
{
	if (strcmp(reg, "AX") == 0 || strcmp(reg, "BX") == 0 || strcmp(reg, "CX") == 0 || strcmp(reg, "DX") == 0 || strcmp(reg, "EX") == 0 || strcmp(reg, "FX") == 0 || strcmp(reg, "GX") == 0 || strcmp(reg, "HX") == 0)
		return 1;
	else
		return 0;
}
int checkRegORVar(char *identifier)
{
	 if (isRegister(identifier) == 1)
		return 1;
	else
		return 0;

}
int getmove(char *expression)
{
	char *text = (char*)malloc(sizeof(char) * 50);
	strcpy(text,expression);
	char *token = strtok(text, ",");
	if (isRegister(token) == 1)
		return 2;
	else
	   return 1;
}
int getOpcode(char *keyword,char *expression)
{
	if (strcmp(keyword, "MOV") == 0)
	{
		return getmove(expression);
	}
	else if (strcmp(keyword, "ADD") == 0)
		return 3;
	else if (strcmp(keyword, "SUB") == 0)
		return 4;
	else if (strcmp(keyword, "MUL") == 0)
		return 5;
	else if (strcmp(keyword, "JUMP") == 0)
		return 6;
	else if (strcmp(keyword, "IF") == 0)
		return 7;
	else if (strcmp(keyword, "EQ") == 0)
		return 8;
	else if (strcmp(keyword, "LT") == 0)
		return 9;
	else if (strcmp(keyword, "GT") == 0)
		return 10;
	else if (strcmp(keyword, "LTEQ") == 0)
		return 11;
	else if (strcmp(keyword, "GTEQ") == 0)
		return 12;
	else if (strcmp(keyword, "PRINT") == 0)
		return 13;
	else if (strcmp(keyword, "READ") == 0)
		return 14;
	else if (strcmp(keyword, "ENDIF") == 0)
		return 15;
	else if (strcmp(keyword, "ELSE") == 0)
		return 16;
	else
		return 17;

}
int getRegisterIndex(char *reg)
{
	if (strcmp(reg, "AX") == 0)
		return 0;
	else if (strcmp(reg, "BX") == 0)
		return 1;
	else if (strcmp(reg, "CX") == 0)
		return 2;
	else if (strcmp(reg, "DX") == 0)
		return 3;
	else if (strcmp(reg, "EX") == 0)
		return 4;
	else if (strcmp(reg, "FX") == 0)
		return 5;
	else if (strcmp(reg, "GH") == 0)
		return 6;
	else
		return 7;
	
}

int getVariableIndex(char *expression, struct symbol **symboltable, int len,int *flag)
{
	int i = 0;
	/*if (expression[1] == '[')
	{
		strtok(expression, "[");
	}*/
	for (i = 0; i < len; i++)
	{
		if (strcmp((symboltable[i]->name), expression) == 0&& symboltable[i]->size==0)
		{
			*flag = 0;
			return symboltable[i]->address;
			
		}

		if (strcmp((symboltable[i]->name), expression) == 0)
		{
			*flag = 1;
			return symboltable[i]->address;
			
		}
		
	}
	return -1;
}
void ifPart(char *instruction, struct intermediate **intermediatetable, int leninter, struct symbol **symboltable, int lensy)
{
	top++;
	int flag = 0;
	stack[top] = leninter + 1;
	char *text = (char*)malloc(sizeof(char) * 50);
	strcpy(text,instruction);
	char *token = strtok(text, " ");
	token = strtok(NULL, " ");
	int i = 0,tokencount=0,index=0;
	while (token != NULL)
	{
		tokencount++;
		if (tokencount == 2)
		{
		   intermediatetable[leninter]->parameters[2] = getOpcode(token,token);
			
		}
		else if (tokencount == 4)
		{
			intermediatetable[leninter]->parameters[3] = 100;
		}
		else
		{
			if (checkRegORVar(token) == 1)
			{
				index = getRegisterIndex(token);
				intermediatetable[leninter]->parameters[i] = index;
				i++;
			}
			else
			{
				index = getVariableIndex(token, symboltable, lensy,&flag);
				if (flag == 0)
				{
					intermediatetable[leninter]->parameters[i] = -1;
					intermediatetable[leninter]->parameters[i + 1] = index;
					i++;
				}
				else
				{
					intermediatetable[leninter]->parameters[i] = index;
					i++;
				}
			}
		}
		
      token = strtok(NULL, " ");
	}
	printf("\n%d", intermediatetable[leninter]->inNo);
	printf("\t%d", intermediatetable[leninter]->opcode);
	int k = 0,j;
	for (j = 0; j < 5; j++)
	{
		k = intermediatetable[leninter]->parameters[j];
		if (k > 100 || k<-100)
			printf("\t\t%s", " ");
		else
			printf("\t%d", k);
	}
}
void intermediategeneration(struct intermediate **intermediatetable, char *instruction,int leninter,struct symbol **symboltable,int lensy,int *memory)
{
	char *keyword; int i = 0, flag = 0;
	char *operation, index=0;
	char *duptoken = (char*)malloc(sizeof(char) * 50);
	char *tok = (char*)malloc(sizeof(char) * 50);
	char *duptext = (char*)malloc(sizeof(char) * 50);
	char *text = (char*)malloc(sizeof(char) * 50);
	strcpy(text, instruction);
	keyword = strtok(text, " ");
	operation = strtok(NULL, " ");
	strtok(operation, "\n");
	int opcode = getOpcode(keyword,operation);
	intermediatetable[leninter]->inNo = leninter + 1;
	intermediatetable[leninter]->opcode = opcode;
	strcpy(duptext, operation);
	tok = strtok(duptext, ",");
	strcpy(duptoken, tok);
	tok = strtok(NULL, ",");
	strtok(duptoken, "[");
	strcpy(text,operation);
	if (opcode!=7)
	{
		char *token = strtok(text, ",");
		while (token != NULL)
		{

			if (token[1] == '[')
			{
				//strcpy(duptoken, token);
				
				int strt = 0, j=0,size = 0;
				printf("\n...%s...\n",token);
				while (j < strlen(token))
				{
					if (token[j] == '[')
					{
						strt = 1;
						j++;
					}
					if (strt == 1)
					{
						while (token[j] != ']'&&i < strlen(token))
						{
							size = size * 10 + (token[j] - '0');
							j++;
						}
						strt = 0;
					}
					else
						j++;

				}
				index = getVariableIndex(duptoken, symboltable, lensy,&flag);
				intermediatetable[leninter]->parameters[i] = index+size;
				i++;
				token = strtok(NULL, ",");
			}
			else if (checkRegORVar(token) == 1)
			{
				index = getRegisterIndex(token);
				intermediatetable[leninter]->parameters[i] = index;
				i++;
				token = strtok(NULL, ",");
			}
			else
			{
				index = getVariableIndex(token, symboltable, lensy,&flag);
				if (flag == 0)
				{
					intermediatetable[leninter]->parameters[i] = -1;
					intermediatetable[leninter]->parameters[i+1] = memory[index];
					i++;
				}
				else
				{
					intermediatetable[leninter]->parameters[i] = index;
					i++;
				}
				token = strtok(NULL, ",");
			}
			
		}
	}
	else
	{
		ifPart(instruction, intermediatetable, leninter,symboltable,lensy);
	}

}
void jumpintermediategeneration(struct intermediate **intermediatetable, char *instruction, int leninter, struct label **labeltable, int lenlabeltable)
{
	char *keyword; int i = 0;
	char *operation, index = 0;
	char *text = (char*)malloc(sizeof(char) * 50);
	strcpy(text, instruction);
	keyword = strtok(text, " ");
	operation = strtok(NULL, " ");
	strtok(operation, "\n");
	int opcode = getOpcode(keyword, operation);
	intermediatetable[leninter]->inNo = leninter + 1;
	intermediatetable[leninter]->opcode = opcode;
	for (i = 0; i < lenlabeltable; i++)
	{
		if (strcmp(operation, labeltable[i]->blockname) == 0)
		{
			intermediatetable[leninter]->parameters[0] = labeltable[i]->address;
			break;
		}
	}
}
void elseintermediategeneration(struct intermediate **intermediatetable, char *instruction, int leninter)
{
	intermediatetable[leninter]->inNo = leninter + 1;
	intermediatetable[leninter]->opcode = 16;
	
	int x = stack[top];
	intermediatetable[x-1]->parameters[3] = leninter + 2;
	intermediatetable[leninter]->parameters[0] = 100;
	stack[top] = leninter + 1;
}
void endifgeneration(struct intermediate **intermediatetable, char *instruction, int leninter)
{
	intermediatetable[leninter]->inNo = leninter + 1;
	intermediatetable[leninter]->opcode = 15;
	int x = stack[top];
	intermediatetable[x - 1]->parameters[3] = leninter + 1;
	top--;
}
void printIntermediateTable(struct intermediate **intermediatetable, int len)
{
	printf("\n\ninNO\tOpCode\t\tParameters\n");
	int i = 0,j=0,k;
	for (i = 0; i < len; i++)
	{
		printf("\n%d", intermediatetable[i]->inNo);
		printf("\t%d", intermediatetable[i]->opcode);
		for (j = 0; j < 5; j++)
		{
			k = intermediatetable[i]->parameters[j];
			if (k > 100 || k<-100)
				printf("\t\t%s", " ");
			else
				printf("\t%d", k);
		}
	}
}
void addtoLabeltable(struct label **labeltable, int len,char *instruction,int l)
{
	char *text = (char*)malloc(sizeof(char) * 50);
	strcpy(text, instruction);
	strtok(text, ":");
	labeltable[len]->blockname = text;
	labeltable[len]->address = l + 1;
}
void printLabelTable(struct label **labeltable, int len)
{
	printf("\n\nBlockName\tAddress\n");
	int i = 0;
	for (i = 0; i < len; i++)
	{
		printf("\n%s", labeltable[i]->blockname);
		printf("\t\t%d", labeltable[i]->address);
	}
}

void interpreter(struct intermediate **intermediatetable,int first, int len, int *memory, int *reg)
{
	int i;
	for (i = first; i < len; i++)
	{
		if (intermediatetable[i]->opcode == 14)
		{
			printf("\n enter value :");
			scanf("%d", &reg[intermediatetable[i]->parameters[0]]);
		}
		else if (intermediatetable[i]->opcode == 1)
		{
			memory[intermediatetable[i]->parameters[0]] = reg[intermediatetable[i]->parameters[1]];
		}
		else if (intermediatetable[i]->opcode == 2)
		{
			if (intermediatetable[i]->parameters[1] == -1)
			{
				reg[intermediatetable[i]->parameters[0]] = intermediatetable[i]->parameters[2];
				//printf("\n mov reg %d %d \n", reg[intermediatetable[i]->parameters[0]], memory[intermediatetable[i]->parameters[2]]);
			}
			else
				reg[intermediatetable[i]->parameters[0]] = memory[intermediatetable[i]->parameters[1]];
		}
		else if (intermediatetable[i]->opcode == 3)
		{
			int c = reg[intermediatetable[i]->parameters[0]];
			reg[intermediatetable[i]->parameters[0]] = reg[intermediatetable[i]->parameters[1]] + reg[intermediatetable[i]->parameters[2]];
			//printf("\ncx %d %d %d %d \n", c, reg[intermediatetable[i]->parameters[0]], reg[intermediatetable[i]->parameters[1]], reg[intermediatetable[i]->parameters[2]]);
		}
		else if (intermediatetable[i]->opcode == 4)
		{
			reg[intermediatetable[i]->parameters[0]] = reg[intermediatetable[i]->parameters[1]] - reg[intermediatetable[i]->parameters[2]];
		}
		else if (intermediatetable[i]->opcode == 5)
		{
			reg[intermediatetable[i]->parameters[0]] = reg[intermediatetable[i]->parameters[1]] * reg[intermediatetable[i]->parameters[2]];
		}
		else if (intermediatetable[i]->opcode == 13)
		{
			if (intermediatetable[i]->parameters[0] == -1)
			{
				printf("\n Value is %d \n:", intermediatetable[i]->parameters[1]);
			}
			else
				printf("\n Value is %d \n:", reg[intermediatetable[i]->parameters[0]]);
		}
		else if (intermediatetable[i]->opcode == 7)
		{
			if (intermediatetable[i]->parameters[2] == 8)
			{
				if (reg[intermediatetable[i]->parameters[0]] == reg[intermediatetable[i]->parameters[1]])
				{
					interpreter(intermediatetable, i + 1, intermediatetable[i]->parameters[3] - 2, memory, reg);
					int x = intermediatetable[i]->parameters[3] - 2;
					i = intermediatetable[x]->parameters[0] - 1;
				}
				else
				{
					int x = intermediatetable[i]->parameters[3] - 2;
					interpreter(intermediatetable, x + 1, intermediatetable[x]->parameters[0] - 1, memory, reg);
					i = intermediatetable[x]->parameters[0] - 1;
				}
			}
			else if (intermediatetable[i]->parameters[2] == 9)
			{
				if (reg[intermediatetable[i]->parameters[0]] < reg[intermediatetable[i]->parameters[1]])
				{
					interpreter(intermediatetable, i + 1, intermediatetable[i]->parameters[3] - 2, memory, reg);
					int x = intermediatetable[i]->parameters[3] - 2;
					i = intermediatetable[x]->parameters[0] - 1;
				}
				else
				{
					int x = intermediatetable[i]->parameters[3] - 2;
					interpreter(intermediatetable, x + 1, intermediatetable[x]->parameters[0] - 1, memory, reg);
					i = intermediatetable[x]->parameters[0] - 1;
				}
			}
			else if (intermediatetable[i]->parameters[2] == 10)
			{
				if (reg[intermediatetable[i]->parameters[0]] > reg[intermediatetable[i]->parameters[1]])
				{
					interpreter(intermediatetable, i + 1, intermediatetable[i]->parameters[3] - 2, memory, reg);
					int x = intermediatetable[i]->parameters[3] - 2;
					i = intermediatetable[x]->parameters[0] - 1;
				}
				else
				{

					int x = intermediatetable[i]->parameters[3] - 2;
					if (x != 98)
					{
						interpreter(intermediatetable, x + 1, intermediatetable[x]->parameters[0] - 1, memory, reg);
						i = intermediatetable[x]->parameters[0] - 1;
					}
				}
			}
			else if (intermediatetable[i]->parameters[2] == 11)
			{
				if (reg[intermediatetable[i]->parameters[0]] <= reg[intermediatetable[i]->parameters[1]])
				{
					interpreter(intermediatetable, i + 1, intermediatetable[i]->parameters[3] - 2, memory, reg);
					int x = intermediatetable[i]->parameters[3] - 2;
					i = intermediatetable[x]->parameters[0] - 1;
				}
				else
				{
					int x = intermediatetable[i]->parameters[3] - 2;
					interpreter(intermediatetable, x + 1, intermediatetable[x]->parameters[0] - 1, memory, reg);
					i = intermediatetable[x]->parameters[0] - 1;
				}
			}
			else if (intermediatetable[i]->parameters[2] == 12)
			{
				if (reg[intermediatetable[i]->parameters[0]] >= reg[intermediatetable[i]->parameters[1]])
				{
					interpreter(intermediatetable, i + 1, intermediatetable[i]->parameters[3] - 2, memory, reg);
					int x = intermediatetable[i]->parameters[3] - 2;
					i = intermediatetable[x]->parameters[0] - 1;
				}
				else
				{
					int x = intermediatetable[i]->parameters[3] - 2;
					interpreter(intermediatetable, x + 1, intermediatetable[x]->parameters[0] - 1, memory, reg);
					i = intermediatetable[x]->parameters[0] - 1;
				}
			}

		}
		else if (intermediatetable[i]->opcode == 6)
		{
			interpreter(intermediatetable, intermediatetable[i]->parameters[0] - 1, i, memory, reg);
		}
	}
}
void createFile(struct intermediate **intermediatetable,int len, char *filename)
{
	int i = 0,j=0;
	FILE *fp;
	fopen_s(&fp, filename, "w");
	if (fp == 0)
	{
		printf("oops...\n");
		printf("enter the correct file name\n");
		exit(1);
	}
	for (i = 0; i < len; i++)
	{
		fprintf(fp, "%d %d",intermediatetable[i]->inNo, intermediatetable[i]->opcode);
		for (j = 0; j < 5; j++)
		{
			fprintf(fp, " %d", intermediatetable[i]->parameters[j]);
		}
		fprintf(fp, "\n");
	}
	fclose(fp);
	return;
}
void main()
{
	int i, memorycurrentaddress = 0;
	struct symbol **symboltable = (struct symbol**)malloc(20 * sizeof(struct symbol*));
	for (i = 0; i < 20; i++)
	{
		symboltable[i] = (struct symbol*)malloc(3*sizeof(struct symbol));
	}
	int lensymboltable=0;
	int lenintermediatetable = 0;
	int lenlabeltable = 0,j=0;
	int *memory = (int *)calloc(40 , sizeof(int));
	int *registercodes = (int *)calloc(8 , sizeof(int));
	struct label **labeltable = (struct label**)malloc(20 * sizeof(struct label*));
	for (i = 0; i < 20; i++)
	{
		labeltable[i] = (struct label*)malloc(sizeof(label));
	}
	struct intermediate **intermediatetable = (struct intermediate**)malloc(30*sizeof(intermediate*));
	for (i = 0; i < 30; i++)
	{
		intermediatetable[i] = (struct intermediate*)malloc(sizeof(intermediate));
	intermediatetable[i]->parameters = (int*)malloc(5 * sizeof(int));
	for (int j = 0; j < 5;j++)
		intermediatetable[i]->parameters[j] = -102;
	}
	fopen_s(&fp,"inst.txt", "r");
	if (fp == 0)
	{
		printf("....INVALID FILE...");
	}
	char *instruction = (char*)malloc(sizeof(char) * 50);
	while (fgets(instruction, 50, fp)!=NULL)
	{
		
		char *keyword = (char *)malloc(sizeof(char) * 30);
		int size;
		char *text = (char*)malloc(sizeof(char) * 50);
		strcpy(text, instruction);
		
		keyword=strtok(text, " ");
		printf("\n.......%s....\n", keyword);


		if (strcmp(keyword, "DATA") == 0 || strcmp(keyword, "CONST") == 0)
		{
			size=addToSymbolTable(symboltable, instruction,lensymboltable,memorycurrentaddress,memory);
			if (size == 0)
			{
				memorycurrentaddress = memorycurrentaddress + 1;
			}
			else
			memorycurrentaddress = memorycurrentaddress + size;
			lensymboltable=lensymboltable+1;
		}
		else if (strcmp(keyword, "START:") == 0)
		{
			continue;
		}
		else if (strcmp(keyword, "JUMP") == 0)
		{
			jumpintermediategeneration(intermediatetable,instruction,lenintermediatetable,labeltable,lenlabeltable);
			lenintermediatetable++;
		}
		else if (strcmp(keyword, "ELSE\n") == 0 || strcmp(keyword, "ELSE") == 0)
		{
			elseintermediategeneration(intermediatetable, instruction, lenintermediatetable);
			lenintermediatetable++;
		}
		else if (strcmp(keyword, "ENDIF\n") == 0 || strcmp(keyword, "ENDIF") == 0)
		{
			endifgeneration(intermediatetable, instruction, lenintermediatetable);
			lenintermediatetable++;
		}
		else if (getOpcode(keyword, "AX") == 17)
		{
			addtoLabeltable(labeltable, lenlabeltable, instruction,lenintermediatetable);
			lenlabeltable++;
		}
		else
		{
			intermediategeneration(intermediatetable,instruction, lenintermediatetable,symboltable,lensymboltable,memory);
			lenintermediatetable++;
		}
	}
	fclose(fp);
	printSymbolTable(symboltable, lensymboltable);
	
	printIntermediateTable(intermediatetable, lenintermediatetable);
	printLabelTable(labeltable, lenlabeltable);
	char *interfilename = (char *)malloc(sizeof(char) * 20);
	printf("\nEnter filename:");
	fflush(stdin);
	gets(interfilename);
	createFile(intermediatetable, lenintermediatetable, interfilename);
	interpreter(intermediatetable, 0,lenintermediatetable, memory, registercodes);
	/*printf("\nMemory\n");
	for (i = 0; i < 40; i++)
	{
		printf(" %d ", memory[i]);
	}
	printf("\nRegister code\n");
	for (i = 0; i < 8; i++)
	{
		printf(" %d ", registercodes[i]);
	}*/
	
	getchar();
}