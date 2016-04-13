int div(int, int);
int mod(int ,int);

void main()
{
	while(1)
	{
		char buffer[512];
		interrupt(0x21,0,"\r\n",0,0);
        interrupt(0x21,0,"Shell>\0",0,0);      
        interrupt(0x21,1,buffer,0,0);

        //view filename
        if(buffer[0] == 'v' && buffer[1] == 'i' && buffer[2] == 'e' && buffer[3] == 'w')
        {
        	int i = 0;
            char filename[7];
            char buffer2[13312];
            while(buffer[5+i] != '\0')
            {
            	filename[i] = buffer[5+i];
            	i++;
            }
            filename[7] = '\0';
            interrupt(0x21,3,filename,buffer2,0);
            interrupt(0x21,0,"\r\n",0,0);
            interrupt(0x21,0,buffer2,0,0);
        }
        else
        {
        	//execute filename + terminate
        	if(buffer[0] == 'e' && buffer[1] == 'x' && buffer[2] == 'e' && buffer[3] == 'c'
        		&& buffer[4] == 'u' && buffer[5] == 't' && buffer[6] == 'e')
        	{
        		int i = 0;
                char filename[7];
                while(buffer[i+8] != '\0')
                {
            	     filename[i] = buffer[i+8];
            	     i++;
                }
                filename[7] = '\0';
                interrupt(0x21, 0, "\r\n", 0, 0);
        	    interrupt(0x21, 4, filename, 0x2000, 0);
        	    interrupt(0x21, 5,0,0,0);
        	}
        	else
        	{
        		//delete filename
        		if(buffer[0] == 'd' && buffer[1] == 'e' && buffer[2] == 'l' && buffer[3] == 'e'
        		&& buffer[4] == 't' && buffer[5] == 'e')
        		{
        			int i = 0;
                    char filename[7];
                    while(buffer[i+7] != '\0')
                    {
            	        filename[i] = buffer[i+7];
            	        i++;
                    }
                    filename[7] = '\0';
                    interrupt(0x21,7,filename,0,0);
        		}
        		else
        		{
        			//copy filename1 filename2
        			if(buffer[0] == 'c' && buffer[1] == 'o' && buffer[2] == 'p' && buffer[3] == 'y')
        			{
        				int i = 0;
        				int j = 0;
        				int sectors = 1;
        				int bufferIndex = 5;
                        char filename1[6];
                        char filename2[6];
                        char buffer2[13312];
                        char sectors1[2];

                        //getting 1st filename
                        while(buffer[bufferIndex] != ' ')
                        {
            	            filename1[i] = buffer[bufferIndex];
            	            i++;
            	            bufferIndex++;
                       }
                       bufferIndex++;

                        //getting 2nd filename
                        while(buffer[bufferIndex] != '\n')
                        {
            	            filename2[j] = buffer[bufferIndex];
            	            j++;
            	            bufferIndex++;
                        }

                        //reading 1st file 
                        interrupt(0x21,3,filename1,buffer2,0);

                        //file name not found
                        if(buffer2[0] == 'F' && buffer2[14] == '!')
                        {
                        	interrupt(0x21,0,buffer2,0,0);
                        }
                        else
                        {
                        	//getting sectors number
                            i = 0;
                            j = 0;
                            while(buffer2[i] != 0x0)
                            {
                                i++;
                                j++;
                                if(j == 512) //filled a sector
                                {
                                    sectors++;
                                    j = 0;
                                }
                            }            	
                            interrupt(0x21,8,filename2,buffer2,sectors);
                        }
        			}
        			else
        			{
        				//dir
        				if(buffer[0] == 'd' && buffer[1] == 'i' && buffer[2] == 'r')
        				{
        					int i;
        					int j;
        					int k;
                            char sec;
        					char dir[512];
        					char filename[7];
        					char sectors[3];

        					interrupt(0x21,2,dir,2,0);
        					interrupt(0x21,0,"\r\n",0,0);
        					for(i = 0; i < 16 && dir[i*32] != 0x0; i++)
        					{
        						j = 0;
                                k = 0;
        						while(dir[(i*32) + j] != 0x0 && j < 6)
        						{
        							filename[j] = dir[(i*32) + j];
        							j++;
        						}
        						filename[j] = '\0';                                
        						interrupt(0x21,0,filename,0,0);
        						interrupt(0x21,0," --> ", 0,0);

                                j = 6;
        						while(dir[(i*32)+j] != 0x0 && j < 32)
        						{
        							k++;
        							j++;
        						}

                                if(k > 9)
                                {
                                    sec = '0' + div(k,10);
                                    sectors[0] = sec;
                                    sec = '0' + mod(k,10);
                                    sectors[1] = sec;
                                    sectors[2] = '\0';
                                }
                                else
                                {
                                    sec = '0' + k;
                                    sectors[0] = sec;
                                    sectors[1] = '\0';
                                }
        						interrupt(0x21,0,sectors,0,0);
        						interrupt(0x21,0," sectors",0,0);
        						interrupt(0x21,0,"\r\n",0,0);
        					}
        				}
        				else
        				{
        					//create filename
        					if(buffer[0] == 'c' && buffer[1] == 'r' && buffer[2] == 'e' && buffer[3] == 'a'
        						&& buffer[4] == 't' && buffer[5] == 'e')
        					{
        						int i = 0;
        						int j = 0;
        						int bufferIndex = 0;
        						int sectors;
                                char filename[7];
                                char line[80];
                                char file[13312];
                                char sec[2];

                               while(i < 6)
                                {
                                    if(buffer[i+7] != '\n')
                                    {
                                        filename[i] = buffer[i+7];
                                    }
                                    else
                                    {
                                        filename[i] = 0x0;
                                    }
                                    i++;
                                }
                                filename[7] = '\0';
                                interrupt(0x21,0,"\r\n",0,0);
                                interrupt(0x21,0,"Enter file: ",0,0);
                                interrupt(0x21,0,"\r\n",0,0);

                                while(1)
                                {
                                	interrupt(0x21,1,line,0,0);
                                	if(line[0] == 0xd || line[0] == ' ' || line[0] == '\n')
                                	{
                                		break;
                                	}
                                	else
                                	{
                                		j = 0;
                                		while(j < 80 && line[j] != 0xd && line[j] != '\n'
                                                     && line[j] != "0x0")
                                		{
                                			file[bufferIndex] = line[j];
                                			bufferIndex++;
                                			j++;
                                		}
                                	}
                                	interrupt(0x21,0,"\r\n",0,0);
                                }

                                j = 0;
                                sectors = 1;
                                while(j < bufferIndex)
                                {
                                    j++;
                                    if(j == 512)
                                    {
                                        sectors++;
                                    }
                                }
                               /* interrupt(0x21,0,"sectors : ",0,0);
                                sec[0] = '0' + sectors;
                                sec[1] = '\0';
                                interrupt(0x21,0,sec,0,0);
                                interrupt(0x21,0,"\r\n",0,0);*/

                                interrupt(0x21,8,filename,file,sectors);
        					}
                            
        					else
        					{
        						interrupt(0x21,0,"\r\n",0,0);
        						interrupt(0x21,0,"Bad command!\r\0",0,0);
        					}
        				}
        			}
        		}
        	}
        }
  
	}
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

int mod(int a, int b)
{
   int c = div(a,b);    
   return a - (b * c);
}
