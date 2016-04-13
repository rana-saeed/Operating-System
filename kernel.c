void printString(char*);
void readString(char*);
void readSector(char*, int);
void readFile(char*, char*);
void executeProgram(char*, int);
void terminate();
void deleteFile(char*);
void writeFile(char*, char*, int);
void handleInterrupt21(int, int, int, int);
int mod(int, int);
int div(int, int);

void main()
{
    /*char* chars = "Hello World\0";
    printString(chars);*/

	/*char line[80];
	printString("Enter a line: \0");
	readString(line);
	printString(line);*/

	/*char buffer[512];
    readSector(buffer, 30);
    printString(buffer);*/

    /*makeInterrupt21();
    interrupt(0x21,0,0,0,0);*/

    /*char line[80];
    makeInterrupt21();
    interrupt(0x21,1,line,0,0);
    interrupt(0x21,0,line,0,0);*/

    /*char buffer[13312]; //this is the maximum size of a file
    makeInterrupt21();
    interrupt(0x21, 3, "messag\0", buffer, 0); //read the file into buffer
    interrupt(0x21, 0, buffer, 0, 0); //print out the file */

    /*makeInterrupt21();
    interrupt(0x21, 4, "tstprg\0", 0x2000, 0);*/

    /*makeInterrupt21();
    interrupt(0x21, 5, "tstpr2\0", 0x2000, 0);*/

    /*char buffer[13312];
    makeInterrupt21();
    interrupt(0x21, 7, "messag\0", 0, 0); //delete messag
    interrupt(0x21, 3, "messag\0", buffer, 0); // try to read messag
    interrupt(0x21, 0, buffer, 0, 0); //print out the contents of buffer*/

    /*int i=0;
    char buffer1[13312];
    char buffer2[13312];
    buffer2[0]='h'; buffer2[1]='e'; buffer2[2]='l';
    buffer2[3]='l';buffer2[4]='o';
    for(i=5; i<13312; i++) buffer2[i]=0x0;
    makeInterrupt21();
    interrupt(0x21,8, "testW\0", buffer2, 1); //write file testW
    interrupt(0x21,3, "testW\0", buffer1, 0); //read file testW
    interrupt(0x21,0, buffer1, 0, 0); // print out contents of testW*/

    makeInterrupt21();
    interrupt(0x21, 4, "shell\0", 0x2000, 0);
    while(1);
}

void printString(char * chars)
{
	int i = 0;
	while(chars[i] != '\0')
	{
		interrupt(0x10,0xE*256+chars[i],0,0,0);
		i = i + 1;
	}
}

void readString(char *chars)
{
	int i = 0;
	while(1)
    {
    	char input = interrupt(0x16,0,0,0,0);

    	//while enter is not pressed
    	if(input != 0xd)
    	{ 
    		//if backspace is pressed 
    	    if(input == 0x8) 
    	    {
    	    	if(i > 0)
    	    	{
    	    		i = i - 1;
 				    interrupt(0x10,0xE*256+0x8,0,0,0); 
    	    	}
            }              
    	    else
    	    {
    	    	chars[i] = input; 
                i = i + 1;
                interrupt(0x10,0xE*256+input,0,0,0);
    	    }
        }
        else
        {
        	 chars[i] = 0xa; 
	 	     i = i + 1; 
	 	     chars[i] = 0x0;
	 	     return;
        }
    }
}

void readSector(char* buffer, int sector)
{
	int relative = mod(sector,18) + 1;
    int head = mod((div(sector,18)),2);
    int track = div(sector,36);
    interrupt(0x13,2*256+1,buffer,track*256+relative,head*256+0);
}

void readFile(char* chars, char* buffer)
{
    char outBuf[512];
    int found = -1;
    int b = 0;
    int i,j;

    readSector(outBuf,2);

    for(i = 0; i < 16; i++)
    {
        if(outBuf[i*32] != 0x0  && outBuf[i*32] != '\r' && outBuf[i*32] != '\n')
        {
            for(j = 0; j < 6; j++)
            {
                if(chars[j] == outBuf[(i*32) + j])
                {
                    b = 1;
                }
                else
                {
                    if (chars[j] == '\r' || chars[j] == '\n' || chars[j] == 0x0)
                    {
                         break;
                    }
                    else
                    {
                        b = 0;
                        break;
                    }
                }
            }
        }
           if(b == 1)
           {
                found = i;
                break;
           }
    }

    if(found != -1)
    {
        int sectorIndex = (found*32) + 6;
        int k;

        for(k = 0; k < 26; k++)
        {
            if(outBuf[sectorIndex + k] != 0x0)
            {
                readSector(buffer, outBuf[sectorIndex + k]);
                buffer += 512;
            }
        } 
    }
    else
    {
        //printString("File not found");
        buffer[0] = 'F';
        buffer[1] = 'i';
        buffer[2] = 'l';
        buffer[3] = 'e';
        buffer[4] = ' ';
        buffer[5] = 'n';
        buffer[6] = 'o';
        buffer[7] = 't';
        buffer[8] = ' ';
        buffer[9] = 'f';
        buffer[10] = 'o';
        buffer[11] = 'u';
        buffer[12] = 'n';
        buffer[13] = 'd';
        buffer[14] = '!';
        return;
    }
}
    
void executeProgram(char* name, int segment)
{
    int i;
    char buffer[13312];
    readFile(name,buffer);

    for(i = 0; i < 13312; i++)
    {
        putInMemory(segment,i,buffer[i]);
    }
    launchProgram(segment);
}

void terminate()
{
    char shell[5];
    shell[0] = 's';
    shell[1] = 'h';
    shell[2] = 'e';
    shell[3] = 'l';
    shell[3] = 'l';
    makeInterrupt21();
    interrupt(0x21, 4, shell, 0x2000, 0);
}

void writeSector(char* buffer, int sector)
{
    int relative = mod(sector,18) + 1;
    int head = mod((div(sector,18)),2);
    int track = div(sector,36);
    interrupt(0x13,3*256+1,buffer,track*256+relative,head*256+0);
}

void deleteFile(char* name)
{
    char dir[512];
    char map[512];

    int i;
    int j;
    int sector;
    int found = -1;

    readSector(dir,2);
    readSector(map,1);

    //looping over dir to find filename
    for(i = 0; i < 16; i++)
    {
        j = 0;
        if(dir[i*32] == name[j])
        {
            for(j = 0; j < 6 && name[j] != '\n'; j++)
            {
                if(dir[(i*32)+j] != name[j])
                {
                    found = -1;
                    break;
                }
                else
                {
                    found = i;
                }
            }
            if(found !=  -1)//found filename
            {
                break;
            }
        }
    }

    if(found != -1)
    {
        //setting the filename to 0x00
        for(j = 0; j < 6; j++)
        {
             dir[(i*32) + j] = 0x00;
        }
       
        j = (i*32) + 6;
        while(j < 32 && dir[j] != 0x0) //setting sectors to 0x00
        {
            sector = dir[j] + 1;
            map[sector] = 0x00;
            j++;
        }

        writeSector(map,1);
        writeSector(dir,2);
    }
    else
    {
        printString("File name not found to delete");
    }




    /*//looping over dir to find file
    for(i = 0; i < 16; i++)
    {
        if(dir[i*32] != 0x0)
        {
            for(j = 0; j < 6; j++)
            {
                if(dir[(i*32) + j] != name[j])
                {
                    b = 0;
                    break;
                }
                else
                {
                    b = 1;
                }
            }
        }

        if(b == 1)
        {
            found = i;
            break;
        }
    }
    if(found != -1)
    {

        for(k = 0; k < 26; k++)
        {
            dir[(found*32)] = 0x00;
        }

        sector = (found*32) + 6;
        k = 0;

        for(k = 0; k < 26; k++)
        {
            if(dir[sector+k] != 0x0)
            {
               dir[sector+k] = 0x00;
               map[sector+k] = 0x00;
            }
        }
        writeSector(map,1);
        writeSector(dir,2);*/

    /*}
    else
    {
        printString("File name not found to delete");
    }*/
}


void writeFile(char* name, char* buffer, int secNum)
{
    char dir[512];
    char map[512];
    char buff[512];
    int sectors[26];

    int dirIndex = -1;
    int mapIndex = -1;
    int secCount = 0;
    int secIndex = 0;
    int bufferIndex = 0;
    int rem;

    int i;
    int j;

    //reading directory and map
    readSector(dir,2);
    readSector(map,1);

    //looping over dir to find 1st empty entry
    for(i = 0; i < 16; i++) 
    {
        if(dir[i*32] == 0x0)
        {
            dirIndex = i*32;
            break;
        }
    }

    //if dir is full
    if(dirIndex == -1)
    {
        printString("No available space in directory!");
        return;
    }
    else
    {
        //wite file name in dir
        for(i = 0; i < 6; i++)
        {
            if(name[i] != 0x0 && name[i] != '\r' && name[i] != '\n' && name[i] != '\0')
            {
                dir[dirIndex + i] = name[i];   
            }
            else
            {
                //fill empty slots in name with 0s
                dir[dirIndex + i] = 0x00;       
            }
        }
        dirIndex += 6;  //getting index to start storing sectors at in dir entry
    }

    //find free sectors in map
    for(i = 0; i < secNum; i++) //looping over number of needed sectors
    {
        for(j = 0; j <26; j++) //looping over all avaialble sectors
        {
            if(map[j] == 0x0) //found empty sector
            {
                sectors[secCount] = j;
                secCount++;
                break;
            }
        }
    }

    //no availabe sectors
    if(secCount < secNum)
    {
        printString("Not enough space in map");
        return;
    }
    else
    {
        //fill empty slots with 0s
        for(i = secCount; i < 26; i++)
        {
            sectors[secCount] = 0x0;
        }
        //store sector numbers in dir
        for(i = 0; i < 32; i++)
        {
            dir[dirIndex + i] = sectors[i];
        } 

        //write sectors
        for(i = 0; i < secCount; i++)
        {  
            for(j = 0; j < 512; j++)
            {
                buff[j] = buffer[bufferIndex];
                bufferIndex++;
            }
            writeSector(buff,sectors[i]);
        }
         writeSector(map,1);
         writeSector(dir, 2);
    }
}


void handleInterrupt21(int ax, int bx, int cx, int dx)
{
	if(ax == 0)
    {
        printString(bx);
    }
    else
    {
        if(ax == 1)
        {
            readString(bx);
        }
        else
        {
            if(ax == 2)
            {
                readSector(bx, cx);
            }
            else
            {
                if(ax == 3)
                {
                    readFile(bx, cx);
                }
                else
                {
                    if(ax == 4)
                    {
                        executeProgram(bx,cx);
                    }              
                    else
                    {
                        if(ax == 5)
                        {
                            terminate();
                        }
                        else
                        {
                            if(ax == 6)
                            {
                                writeSector(bx,cx);
                            }
                            else
                            {
                                if(ax == 7)
                                {
                                    deleteFile(bx);
                                }
                                else
                                {
                                    if(ax == 8)
                                    {
                                        writeFile(bx,cx,dx);
                                    }
                                    else
                                    {
                                        printString("Error: Undefined interrupt!");
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

int mod(int a, int b)
{
   int c = div(a,b);	
   return a - (b * c);
}

int div(int a, int b)
{
	int q = 0;
    while (a >= b) {
        a = a - b; 
        q = q + 1;
    }
    return q;
}




