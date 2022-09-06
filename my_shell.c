#include  <stdio.h>
#include  <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <signal.h>

#define MAX_INPUT_SIZE 1024
#define MAX_TOKEN_SIZE 64
#define MAX_NUM_TOKENS 64

int bgchld[64];
int forgchld=0;
int j=0;





///////////////////////////////////////////////////////////////////////signal handler///////////////////////////////////////////////////////
void handle_sigchld(int sig) {
                  
		  if(sig==SIGCHLD)
		  {
			  int saved_errno = errno;
			  while (waitpid((pid_t)(-1), 0, WNOHANG) > 0) {printf("Shell: Background process finished\n");}
			  errno = saved_errno;
		   }
		
                  if(sig==SIGINT)
		    {
			 
			//////////////////////killing foreground processes/////////////////////
                                             if(forgchld>0)
						{
									 //printf("%d\n",forgchld);
									 //printf("%d kill vslue",kill(forgchld,SIGINT));
									 printf("\nShell: Foreground process finished\n");
										//forgchld=0;
						}
                                             else {printf("\nShell : No Foreground process is created till yet\n");}
                         
			}
                 
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////











/* Splits the string by space and returns the array of tokens
*
*/
char **tokenize(char *line)
{
  char **tokens = (char **)malloc(MAX_NUM_TOKENS * sizeof(char *));
  char *token = (char *)malloc(MAX_TOKEN_SIZE * sizeof(char));
  int i, tokenIndex = 0, tokenNo = 0;

  for(i =0; i < strlen(line); i++){

    char readChar = line[i];

	if (readChar == ' ' || readChar == '\n' || readChar == '\t')
	{
	  	token[tokenIndex] = '\0';//first member of token is set to 0
	  	if (tokenIndex != 0){
			tokens[tokenNo] = (char*)malloc(MAX_TOKEN_SIZE*sizeof(char));
			strcpy(tokens[tokenNo++], token);
			tokenIndex = 0; 
		}
	  } 
    else {
      token[tokenIndex++] = readChar;
    }
}
 
  free(token);
  tokens[tokenNo] = NULL ;
  return tokens;
}












int main(int argc, char* argv[]) 
       {
	char  line[MAX_INPUT_SIZE];            
	char  **tokens;              
        char s[100];
	int i;
        int chdir_check;
        int counter=0;
        int background_process=64;
        int status;
        signal(SIGINT,handle_sigchld);

       

	while(1) 
		             {		        
						

                                                struct sigaction sa;
					        sa.sa_handler = &handle_sigchld;
					        sigemptyset(&sa.sa_mask);
					        sa.sa_flags = SA_RESTART | SA_NOCLDSTOP;
					        if (sigaction(SIGCHLD, &sa, 0) == -1)
                                                     {
							       perror(0);
							       exit(1);
                                                               //continue;
						     }
 




                                                shift:

						bzero(line, sizeof(line));//emptyig the variable line with zeroes
				                counter=0;
						printf("$ ");
						scanf("%[^\n]", line);// take input until \n is pressesd
                                                getchar();


				
				
						line[strlen(line)] = '\n'; //terminate with new line
                                                tokens = tokenize(line);
				
				      
						if(tokens[0]==NULL){free(tokens);continue;}






						//////////////////////////////exit to kill background process////////////////////////////////
                                                if(strcmp(tokens[0],"exit")==0)
		                                        {      printf("Shell : All bacground process terminated\n");
                                                               sleep(1);
		                                               for(int k=0;k<=64;k++)
								     {
									kill(bgchld[k],SIGTERM);
									}
		                                           free(tokens);break;
		                                           }










                                                /////////////counter to detect valid background processes/////////////////////////////////
						for(i=0;tokens[i]!=NULL;i++)
                                                        {
							    if (strcmp(tokens[i],"&")==0)
								{               
									counter++;
                                                                        free(tokens[i]);
                                                                        tokens[i]=NULL; }
						        }









                                                
						 ////////////////////////////////////////////////////cd command/////////////////////////////////////////////////////////////////////////////////  
						if (strcmp(tokens[0],"cd")==0)
						      {
							   if(!(tokens[2]==NULL))
								    { 
								       printf("Shell : Incorrect command\n");
								     }
							    else if((tokens[1]==NULL) || (strcmp(tokens[1],"~")==0))
								    { 
								       chdir("/home");
								     }
							    
							    else if(strcmp(tokens[1],"-")==0)
								    {
								      chdir("..");
								    }
							    else
								  { 
									   chdir_check=chdir(tokens[1]);
									   if(chdir_check!=0)
										{ 
										  printf("Shell : Invalid Command\n");
										 }
								   }
							      free(tokens); continue;
						       }
                                                  
                                                




						///////////////////////////////////////differentiating between background and foreground processes//////////////////////////////////
				
					     if(counter==1)
							      {          
									
							  
								               int fb=fork();
                                                                               bgchld[j]=fb;
                                                                               j++;

										if(fb<0)
		                                                                          {
												  fprintf(stderr,"%s\n","unable to create child process\n");
											  }
										else if(fb==0)
		                                                                          {              setpgid(0,0);
													 execvp(tokens[0],tokens);
													 
													 printf("you entered a wrong command\n");
													 _exit(1);
											  }
										 else
		                                                                               {    
												   
											       
											       }
										  
							      }

					      else if(counter==0) 
							      {
								             
									int fc=fork();
                                                                         
									if(fc<0)
		                                                                 {
										   fprintf(stderr,"%s\n","unable to create child process\n");
										  }
									else if(fc==0)
                                                                                 {        //forgchld=getpid();
                                                                                         //signal(SIGINT,handle_sigchld);
											 execvp(tokens[0],tokens);
											 
											 printf("you entered a wrong command\n");
											 _exit(1);
									         }
									 else
                                                                                 {        forgchld =fc;
											  int wc=waitpid(fc,0,0);
										       
									         }
									      
								}
						
							      
					      else 
							      {
								            printf("invalid command for running background process\n");
							      }
							      
		
						free(tokens);

					}
                                        
					return 0;
                            }

