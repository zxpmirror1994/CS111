// UCLA Fall 2015 CS 111 Lab 1a 

#include "command.h"
#include "command-internals.h"

#include <error.h>

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include "alloc.h"
#include <string.h> 
#include <stdlib.h>

int word_size = 0;
char *wordout = NULL;
int wordout_size = 0;
int numOfLine = 1;
int isComment = 0;
command_t cmd = NULL;
command_stream_t head1 = NULL;
command_stream_t current1 = NULL;
command_stream_t next1 = NULL;

//Judge if the character is an ordinary token
int
judgeVal(char a)
{
  int result;
  if ( (a == '!')
    || (a == '%')|| (a == '+')|| (a == ',')
    || (a == '-')|| (a == '.')|| (a == '/')
    || (a == ':')|| (a == '@')|| (a == '^')
       || (a == '_')||isalpha(a)||isdigit(a))
    {
      result = 1;
    }
  else
    {
      result = 0;
    }
  return result;
}

//Judge if the character is a special token
int
judgeSpe(char a)
{
  int result;
  if( (a == ')')|| (a == '<')|| (a == '>')||
      (a == '|')|| (a == '&')|| (a == '(') ||(a == ';'))
    {
      result = 1;
    }
  else
    {
      result = 0;
    }
  
  return result;
}

//Judge if the character is a space, tab or newline 
int
judgewhite(char a)
{
  int result;
  if ((a == ' ')|| (a == '\n')|| (a == '\t'))
    {
      result = 1;
    }
  else
    {
      result = 0;
    }
  return result;
}

//Remove all whitespaces in a stream, while keeping track of the current line number
void
removewhite(void *stream)
{
  char a;
  while(1)
    {
      a = getc(stream);
      if (a == EOF)
	{
	  ungetc(a, stream);
	  break;
	}

      if (judgewhite(a) && a != '\n')
	{
          continue;
	}
      else if (a == '\n')
	{
	  numOfLine++;
	  continue;
	}
      ungetc(a, stream);
      return;
    }

}

//Remove all whitespaces, excluding newlines
void
removespa(void *stream)
{
  char a;
  while(1)
    {
      a = getc(stream);
      if (a == EOF)
	{
	  ungetc(a, stream);
	  break;
	}

      if (judgewhite(a) && a != '\n')
	continue;
      ungetc(a, stream);
      break;
    }

}

//Enumerate all possible operator types
enum typeoperator
  {
    SEMICOLON,
    AND,
    OR,
    PIPE,
    PARENTHESIS,
    LEFT,
    RIGHT,
    ERR
  };

//Convert the operater types into corresponding integers for convenience
int convert(enum typeoperator op)
{
  switch(op)
  {
  case SEMICOLON:
    return 1;
  case AND:
    return 2;
  case OR:
    return 3;
  case PIPE:
    return 4;
  case PARENTHESIS:
    return 5;
  case LEFT:
    return 6;
  case RIGHT:
    return 7;
  case ERR:
    return 8;
  default:
    break;
  }
  return -1;
}

//Structure of the operator stack
typedef struct operatorstack
{
  int index;
  int total;
  int empty;
  int *entry;
} operatorstack;

//Structure of the command stack
typedef struct commandstack
{
  int index;
  int total;
  int empty;
  command_t *entry;
} commandstack;

//Initialize the operator stack
operatorstack *
beginOperatorstack()
{
  operatorstack *ops = (operatorstack *)malloc(sizeof(operatorstack));
  ops->index = 0;
  ops->total = 20;
  ops->empty = 1;
  ops->entry = (int *)malloc(sizeof(int) * 20);
  return ops;
}

//Initialize the command stack
commandstack *
beginCommandstack()
{
  commandstack *cmds = (commandstack *)malloc(sizeof(commandstack));
  cmds->index = 0;
  cmds->total = 20;
  cmds->empty = 1;
  cmds->entry = (command_t *)malloc(sizeof(command_t) * 20);
  return cmds;
}

//Judge the type of the operator
int judgetype(char *str)
{
  int typetest;
  if (strcmp(str, ";") == 0)
    typetest = convert(SEMICOLON);
  else if (strcmp(str, "&&") == 0)
    typetest = convert(AND);
  else if (strcmp(str, "||") == 0)
    typetest = convert(OR);
  else if (strcmp(str, "|") == 0)
    typetest = convert(PIPE);
  else if (strcmp(str, "(") == 0)
    typetest = convert(PARENTHESIS);
  else
    typetest = convert(ERR);
  return typetest;
}

//Push the operator onto the operator stack
void
op_push(operatorstack *ops, char *str)
{
  if ((ops->index - ops->total)== 0)
    {
      ops->total *=3;
      ops->entry = (int *)checked_realloc(ops->entry, sizeof(int) * ops->total);
    }
 
  if (ops->empty)
    {
      ops->empty = 0;
    }
  int typetest2;
  typetest2 = judgetype(str);
  int index1;
  index1 = ops->index;
  ops->entry[index1] = typetest2;
  ops->index++;

}

//Push the command onto the command stack
void
cmd_push(commandstack *cmds, command_t cmd)
{
  if ((cmds->index - cmds->total)== 0)
    {
      cmds->total *= 3;
      cmds->entry = (command_t *)checked_realloc(cmds->entry, sizeof(command_t) * cmds->total);
    }
  if(cmds->empty)
    {
      cmds->empty = 0;
    }
  int result = cmds->index;
  cmds->index++;
  cmds->entry[result] = cmd;

}


//Check if the operator stack is empty
int ifopstackempty(operatorstack *ops)
{
  if(ops->empty)
    return 1;
  return 0;
}

//Check if the command stack is empty
int ifcmdstackempty(commandstack *cmds)
{
  if(cmds->empty)
    return 1;
  return 0;
}


//Return the top operator on the operator stack
int op_top(operatorstack *ops)
{
  if (ifopstackempty(ops))
    return 8;
  int result;
  result = ops->index;
  result--;
  return ops->entry[result];
}

//Return the top command on the command stack
command_t
cmd_top(commandstack *cmds)
{
  if (ifcmdstackempty(cmds))
    return NULL;
  int result = cmds->index;
  result--;
  return cmds->entry[result];
}

//Pop the top operator off the operator stack
int op_pop(operatorstack *ops)
{
  if (ifopstackempty(ops))
    return 8;
  if(ops->index == 1)
    {
      ops->empty = 1;
    }
  ops->index--;
  int result = ops->index;
  return ops->entry[result];
}

//Pop the top command off the command stack
command_t
cmd_pop(commandstack *cmds)
{
  if (ifcmdstackempty(cmds))
    return NULL;
  if (cmds->index == 1)
    {
      cmds->empty = 1;
    }
  int result;
  cmds->index--;
  result = cmds->index;
  return cmds->entry[result];
}

//Clear "cmd"
void
removeCommand()
{
  if (cmd != NULL)
    {
      int indexsize = word_size;
      word_size = 0;
      cmd->u.word = (char **)checked_realloc(cmd->u.word, sizeof(char *) * (1 + indexsize));
      cmd->u.word[indexsize] = '\0';
      cmd = NULL;
    }
}

//construct a simple command, add the simple command on the global variable
void
prosimple(commandstack *cmdStack)
{
  if (wordout != NULL)
    {
      if (cmd == NULL)
	{
	  cmd = (command_t)checked_malloc(sizeof(struct command));
	  cmd->status = -1;
	  cmd->type = SIMPLE_COMMAND;
	  cmd->u.word = NULL;
	  cmd->input = cmd->output = NULL;
	  cmd_push(cmdStack, cmd);
	}
      int indexsize1 = word_size;
      int indexsize2 = wordout_size;
      wordout_size = 0;
      word_size++;
      wordout = (char *)checked_realloc(wordout, sizeof(char) * (1 + indexsize2));
      wordout[indexsize2] = '\0'; 
      cmd->u.word = (char **)checked_realloc(cmd->u.word, sizeof(char *) * (1 + indexsize1));
      cmd->u.word[indexsize1] = wordout;
      wordout = NULL;
    }
}

//combine 2 commands and an operator into a new command, push onto the command stack
void
processdoubleCommand(operatorstack *ops, commandstack *cmds)
{
  int topoperator = op_pop(ops);

  command_t testcommand = (command_t)checked_malloc(sizeof(struct command));
  if (cmds->index <= 1)
    {
      fprintf(stderr, "%d: processdoubleCommand error\n", numOfLine);
      exit(1);
    }
  command_t rcommand = cmd_pop(cmds);
  command_t lcommand = cmd_pop(cmds);

  if (topoperator == 1)                                                       
    testcommand->type = SEQUENCE_COMMAND;                                           
  else if ( topoperator == 3)                                                           
    testcommand->type = OR_COMMAND;                                                 
  else if (topoperator == 2)                                                          
    testcommand->type = AND_COMMAND;                                                  
  else if (topoperator == 4)                                                          
    testcommand->type = PIPE_COMMAND;                                                
  else                                                                          
    {                                                                           
      fprintf(stderr, "%d:combining mistake\n", numOfLine);           
      exit(1);                                                                  
    }      
  testcommand->status = -1;                                                    
  testcommand->input = testcommand->output = NULL;
  testcommand->u.command[1] = rcommand;                                        
  testcommand->u.command[0] = lcommand;
  cmd_push(cmds, testcommand);
}

//make simple command, create the node and push it on the linked list
void
workStacks(commandstack *cmds, operatorstack *ops)
{
  prosimple(cmds);

  while (!(ifopstackempty(ops)))
    processdoubleCommand(ops, cmds);

  if (ifcmdstackempty(cmds) || cmds->index > 1)
    {
      
      fprintf(stderr, "%d: workStacks 2 error\n", numOfLine);
      exit(1);
    }

  command_stream_t cmd_point = checked_malloc(sizeof(struct command_stream));
  cmd_point->next = NULL;
  cmd_point->curr = cmd_pop(cmds);
  if (head1 != NULL)
    {
      current1->next = cmd_point;
      current1 = cmd_point;
    }
  else
    {
      head1 = cmd_point;
      current1 = cmd_point;
    }
}



void
workRedirect(char **stringlist, void *stream)
{
  removespa(stream);
  int index = 0;
  char a;
  while(1)
    { 
      a = getc(stream);
      if ( a == EOF)
	{ break;}
      if (!(judgewhite(a)) && !(judgeSpe(a)))
	{
	  *stringlist = (char *)checked_realloc(*stringlist, sizeof(char) * (1 + index));
	  int result;
	  result = index;
	  index++;
	  (*stringlist)[result] = a;
	}
       else
	{
	  ungetc(a, stream);
	  if (index != 0){
	    *stringlist = (char *)checked_realloc(*stringlist, sizeof(char) * (1 + index));
	    (*stringlist)[index] = '\0';
	    return;}
	  else 
	    {
	      fprintf(stderr, "%d: syntax error\n", numOfLine);
	      exit(1);
	    }
	}
      
    }
  ungetc(a, stream);
}

int judgeComment(char a)
{   
    if (a == '#' || isComment)       
   {
            isComment = 1;   
            if (a == '\n')
            {
	      isComment = 0;   
              numOfLine++;
                
            }
	    return isComment;
        }   
	return 0;
    }




command_stream_t
make_command_stream (int (*get_next_byte) (void *),
		     void *get_next_byte_argument)
{
  /* FIXME: Replace this with your implementation.  You may need to
     add auxiliary functions and otherwise modify the source code.
     You can also use external functions defined in the GNU C Library.  */
  FILE *teststre = (FILE *)get_next_byte_argument;
  removewhite(teststre);
  int judgecomm = 0;
  int isoperator = 0;

  operatorstack *ops = beginOperatorstack();
  commandstack *cmds = beginCommandstack();
 
  char a;
  while(1)  
  {
      a = (*get_next_byte)(teststre);
      if (a == EOF)
	{
	  break;
	}
      if(judgeComment(a))
	{
	  continue;
	  }
      /*	if (judgecomm || a == '#') //if it is a command lien
	{
	  judgecomm = 1;
	  if (a == '\n')
	    {
              judgecomm = 0;
	      numOfLine++;
	    }
	  continue;
	  }
      */   
      if (!(judgeVal(a) || judgewhite(a) || judgeSpe(a) ))
	{
	  fprintf(stderr, "%d: syntax mistake in the line \n", numOfLine);
	  exit(1);
	}
                
      if (a == ' ' || a == '\t')
	{
	  prosimple(cmds);
	  continue; // consume spaces
	}

      if (a == '\n')
	{
	  numOfLine++;
	  // case 1: bi-operators
	  if (isoperator)
	    {
	      // e.g. A &&\n\n\n B
	      removewhite(teststre); // This includes all the following newlines
	      isoperator = false;
	      continue;
	    }
	SWIFT:
	  // case 2: sequence command
          a = (*get_next_byte)(teststre);	  
	  if (a == EOF)
	    break;
	  else
	    {
	      prosimple(cmds);
	      removeCommand();

	      if (judgeSpe(a) && a != '(' && a != '>' && a != '<')
		{
		  // Except for <, >, and (, there expect to be a valid string before the special token
		  fprintf(stderr, "%d: syntax error\n", numOfLine);
		  exit(1);
		}
	      else if (a == '\n') // case 3: the end of a command tree
		{
		  workStacks(cmds, ops);
		  numOfLine++;
		}
	      else
		{
		  if (cmds->index > 0)
		    op_push(ops, ";");
		  ungetc(a, teststre);
		}
	    }

	  continue;
	}

      if (judgeSpe(a))
	{
	  prosimple(cmds);
	  removeCommand();

	  // Except for <, >, and (, special token cannot immediately follow &&, ||, or |
	  //                         and there expect to be a valid string before the special token
	  
	  if (a != '<' && a != '>' && a != '(' && (isoperator == 1 || cmds->index == 0))
	    {
	      fprintf(stderr, "%d: syntax error\n", numOfLine);
	      exit(1);
	    }
	  
	  // case 1: redirection
	  if (a == '>' || a == '<')
	    {
	      command_t tmp = cmd_top(cmds);
	      if (tmp == NULL)
		{
		  fprintf(stderr, "%d: syntax error\n", numOfLine);
		  exit(1);
		}
	      else
		{
		  if (a == '>')
		    workRedirect(&tmp->output, teststre);
		  else
		    workRedirect(&tmp->input, teststre);
		}
	
		
	      continue;
	    }

	  // case 2: subshell
	  else if (a == ')')
	    {
	      command_t commandsubs = (command_t)checked_malloc(sizeof(struct command));
	      commandsubs->status = -1;
	      commandsubs->type = SUBSHELL_COMMAND;
	      commandsubs->input = commandsubs->output = NULL;
	      
	      while (ops->index != 0 && op_top(ops) != PARENTHESIS)
		processdoubleCommand(ops, cmds);

	      if (ops->index == 0)
		{
		  fprintf(stderr, "%d: workStacks error\n", numOfLine);
		  exit(1);
		}
	      commandsubs->u.subshell_command = cmd_pop(cmds);
	      op_pop(ops);
	      cmd_push(cmds, commandsubs);
	      continue;
	    }

	  if (a == '|' || a == '&')
	    {
	      char b;
	      b = (*get_next_byte)(teststre);
	      if (b != EOF)
		{
		  isoperator = 1;

		  if (a == '|' && b != '|') // OR
		     // PIPE
		    {
		      while (ops->index != 0 && op_top(ops) == PIPE) // precedence rule
			processdoubleCommand(ops, cmds);
		      op_push(ops, "|");
		      ungetc(b, teststre);
		    }
		  else if (( a == '|' && b == '|') || ( a == '&' && b == '&'))
		    {
		      while (ops->index != 0 && op_top(ops) != SEMICOLON) // precedence rule
			processdoubleCommand(ops, cmds);
		      if (a == '|')
			{
			  op_push(ops, "||");
			}
		      else
			{
			  op_push(ops, "&&");
			}
		    }
		  else if ( a == '&' && b != '&')
		    {
		      fprintf(stderr, "%d: AND error\n", numOfLine);
		      exit(1);
		    }
		}
	      else
		break;
	      continue;
	    }

	  // case 4: ';' may be ignored before multiple newlines
	  if (a == ';')
	    {
	      removespa(teststre);
	      a = (*get_next_byte)(teststre);
	      if (a != EOF)
		{
		  if (a != '\n')
		    {
		      while (ops->index != 0) // precedence rule
			processdoubleCommand(ops, cmds);
		      op_push(ops, ";");
		      ungetc(a, teststre);
		    }
		  else
		    numOfLine++;
		    goto SWIFT;
		}
	      else
		break;
	      continue;
	    }

	  // '('
	  if (cmd != NULL)
	    {
	      fprintf(stderr, "%d: syntax error\n", numOfLine);
	      exit(1);
	    }
	  op_push(ops, "(");
	  continue;
	}

      isoperator = 0;
      wordout = (char *)checked_realloc(wordout, sizeof(char) * (1 + wordout_size));
      wordout[wordout_size++] = a;

    }

  prosimple(cmds);
  removeCommand();
  if (cmds->index != 0 || ops->index != 0)
    workStacks( cmds, ops);
  return head1;


}

command_t
read_command_stream (command_stream_t s)
{
  /* FIXME: Replace this with your implementation too.  */
  if (s == NULL)
    return NULL;
  if (s->curr == NULL)
    return NULL;
 
  command_t a = s->curr;

  if (s->next == NULL)
    s->curr = NULL;
  else
    {
      command_stream_t next1 = s->next;
      s->curr = s->next->curr;
      s->next = s->next->next;
    }
  return a;


}
