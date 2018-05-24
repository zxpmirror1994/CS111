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

struct command_stream
{
  command_t curr;
  command_stream_t next;

};

int word_size = 0;
char *wordout = NULL;
int wordout_size = 0;
int numOfLine = 1;
int isComment = 0;
command_t comminput = NULL;
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
judgeWhite(char a)
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
removeWhite(void *stream)
{
  char a;
  for(;;)
    {
      a = getc(stream);
      if (a == EOF)
	{
	  ungetc(a, stream);
	  break;
	}

      if (judgeWhite(a) && a != '\n')
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
removeSpa(void *stream)
{
  char a;
  for(;;)
    {
      a = getc(stream);
      if (a == EOF)
	{
	  ungetc(a, stream);
	  break;
	}

      if (judgeWhite(a) && a != '\n')
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
typedef struct operatorStack
{
  int index;
  int total;
  int empty;
  int *entry;
} operatorStack;

//Structure of the command stack
typedef struct commandStack
{
  int index;
  int total;
  int empty;
  command_t *entry;
} commandStack;

//Initialize the operator stack
operatorStack *
beginOperatorStack()
{
  operatorStack *ops = (operatorStack *)malloc(sizeof(operatorStack));
  ops->index = 0;
  ops->total = 20;
  ops->empty = 1;
  ops->entry = (int *)malloc(sizeof(int) * 20);
  return ops;
}

//Initialize the command stack
commandStack *
beginCommandStack()
{
  commandStack *cmds = (commandStack *)malloc(sizeof(commandStack));
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
op_push(operatorStack *ops, char *str1)
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
  typetest2 = judgetype(str1);
  int index1;
  index1 = ops->index;
  ops->entry[index1] = typetest2;
  ops->index++;

}

//Push the command onto the command stack
void
cmd_push(commandStack *cmds, command_t comminput)
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
  cmds->entry[result] = comminput;

}


//Check if the operator stack is empty
int ifOpStackEmpty(operatorStack *ops)
{
  if(ops->empty)
    return 1;
  return 0;
}

//Check if the command stack is empty
int ifCmdStackEmpty(commandStack *cmds)
{
  if(cmds->empty)
    return 1;
  return 0;
}


//Return the top operator on the operator stack
int op_top(operatorStack *ops)
{
  if (ifOpStackEmpty(ops))
    return 8;
  int result;
  result = ops->index;
  result--;
  return ops->entry[result];
}

//Return the top command on the command stack
command_t
cmd_top(commandStack *cmds)
{
  if (ifCmdStackEmpty(cmds))
    return NULL;
  int result = cmds->index;
  result--;
  return cmds->entry[result];
}

//Pop the top operator off the operator stack
int op_pop(operatorStack *ops)
{
  if (ifOpStackEmpty(ops))
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
cmd_pop(commandStack *cmds)
{
  if (ifCmdStackEmpty(cmds))
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
  if (comminput != NULL)
    {
      int indexSize = word_size;
      word_size = 0;
      comminput->u.word = (char **)checked_realloc(comminput->u.word, sizeof(char *) * (1 + indexSize));
      comminput->u.word[indexSize] = '\0';
      comminput = NULL;
    }
}

//construct a simple command, add the simple command on the global variable
void
proSimple(commandStack *cmdStack)
{
  if (wordout != NULL)
    {
      if (comminput == NULL)
	{
	  comminput = (command_t)checked_malloc(sizeof(struct command));
	  comminput->status = -1;
	  comminput->type = SIMPLE_COMMAND;
	  comminput->u.word = NULL;
	  comminput->input = comminput->output = NULL;
	  cmd_push(cmdStack, comminput);
	}
      int indexSize1 = word_size;
      int indexSize2 = wordout_size;
      wordout_size = 0;
      word_size++;
      wordout = (char *)checked_realloc(wordout, sizeof(char) * (1 + indexSize2));
      wordout[indexSize2] = '\0'; 
      comminput->u.word = (char **)checked_realloc(comminput->u.word, sizeof(char *) * (1 + indexSize1));
      comminput->u.word[indexSize1] = wordout;
      wordout = NULL;
    }
}

//combine 2 commands and an operator into a new command, push onto the command stack
void
processDoubleCommand(operatorStack *ops, commandStack *cmds)
{
  int topoperator = op_pop(ops);

  command_t testcommand = (command_t)checked_malloc(sizeof(struct command));
  if (cmds->index == 1 || ifCmdStackEmpty(cmds))
    {
      fprintf(stderr, "%d: processDoubleCommand mistake\n", numOfLine);
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
      fprintf(stderr, "%d: combining mistake\n", numOfLine);           
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
workStacks(operatorStack *ops, commandStack *cmds)
{
  proSimple(cmds);

  for (;!(ifOpStackEmpty(ops));processDoubleCommand(ops, cmds))
    {}
  if (ifCmdStackEmpty(cmds) || cmds->index > 1)
    {
      
      fprintf(stderr, "%d: workStacks mistake\n", numOfLine);
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
  removeSpa(stream);
  int index = 0;
  char a;
  for(;;)
    { 
      a = getc(stream);
      if ( a == EOF)
	{ break;}
      if (!(judgeWhite(a)) && !(judgeSpe(a)))
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
	  if (index != 0)
	    {
	      *stringlist = (char *)checked_realloc(*stringlist, sizeof(char) * (1 + index));
	      (*stringlist)[index] = '\0';
	      return;
	    }
	  else 
	    {
	      fprintf(stderr, "%d: syntax mistake\n", numOfLine);
	      exit(1);
	    }
	}
      
    }
  ungetc(a, stream);
}

//Judge if the character belongs to comment
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
  FILE *teststre = (FILE *)get_next_byte_argument;
  removeWhite(teststre);
  int isOperator = 0;

  operatorStack *ops = beginOperatorStack();
  commandStack *cmds = beginCommandStack();
 
  char a;
  for(;;)  
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
      if (!(judgeVal(a) || judgeWhite(a) || judgeSpe(a) ))
	{
	  fprintf(stderr, "%d: syntax mistake in the line\n", numOfLine);
	  exit(1);
	}
                
      if (a != '\n' && judgeWhite(a))
	{
	  proSimple(cmds);
	  continue;
	}

      if (a == '\n')
	{
	  numOfLine++;
	
	  // &&, ||
	  if (isOperator)
	    {
	      removeWhite(teststre);
	      isOperator = 0;
	      continue;
	    }
	 SWIFT:
	  // sequence command
          a = (*get_next_byte)(teststre);	  
	  if (a == EOF)
	    break;
	  else
	    {
	      proSimple(cmds);
	      removeCommand();

	      if (a == ';' || a == '|' || a == '&' || a == ')')
		{
		  fprintf(stderr, "%d: invalid special token\n", numOfLine);
		  exit(1);
		}
	      else if (a == '\n') 
		{
		  workStacks(ops, cmds);
		  numOfLine++;
		}
	      else
		{
		  if (!ifCmdStackEmpty(cmds))
		    op_push(ops, ";");
		  ungetc(a, teststre);
		}
	    }
	  continue;
	}

      if (judgeSpe(a))
	{
	  proSimple(cmds);
	  removeCommand();  
	  int temp = isOperator || ifCmdStackEmpty(cmds);
	  if (temp && a != '(' && a != '>' && a != '<')
	    {
	      fprintf(stderr, "%d: invalid special token\n", numOfLine);
	      exit(1);
	    }
	  
	  //redirection token
	  if (a == '>' || a == '<')
	    {
	      command_t comm = cmd_top(cmds);
	      if (comm != NULL)
		{
		  if (a == '>')
		    {
		      workRedirect(&comm->output, teststre);
		    }		  
		  else
		    {
		      workRedirect(&comm->input, teststre);
		    }
		  continue;
		}
	      fprintf(stderr, "%d: empty command stack\n", numOfLine);
	      exit(1);
	    }

	  //subshell token
	  else if (a == ')')
	    {
	      command_t commandSubs = (command_t)checked_malloc(sizeof(struct command));
	      commandSubs->status = -1;
	      commandSubs->type = SUBSHELL_COMMAND;
	      commandSubs->input = commandSubs->output = NULL;
	      
	      for (;(!ifOpStackEmpty(ops)) && op_top(ops) != 5;processDoubleCommand(ops, cmds))
		{}
	      if (!ifOpStackEmpty(ops))
		{	
		  commandSubs->u.subshell_command = cmd_pop(cmds);
		  op_pop(ops);
		  cmd_push(cmds, commandSubs);
		  continue;
		}
		  fprintf(stderr, "%d: empty operator stack\n", numOfLine);
		  exit(1);
	    }

	  if (a == '|' || a == '&')
	    {
	      char b;
	      b = (*get_next_byte)(teststre);
	      if (b != EOF)
		{
		  isOperator = 1;

		  if (a == '|' && b != '|')
		    {
		      for (;(!ifOpStackEmpty(ops)) && op_top(ops) == 4; processDoubleCommand(ops, cmds))
			{}
		      op_push(ops, "|");
		      ungetc(b, teststre);
		    }
		  else if (( a == '|' && b == '|') || ( a == '&' && b == '&'))
		    {
		      for (;(!ifOpStackEmpty(ops)) && op_top(ops) != 1; processDoubleCommand(ops, cmds))
			{}
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
		      fprintf(stderr, "%d: invalid special token\n", numOfLine);
		      exit(1);
		    }
		}
	      else
		break;
	      continue;
	    }

	  if (a == ';')
	    {
	      removeSpa(teststre);
	      a = (*get_next_byte)(teststre);
	      if (a == EOF)
		break;
	      else
		{
		  if (a != '\n')
		    {
		      for (;(!ifOpStackEmpty(ops));processDoubleCommand(ops, cmds))
			{}
		      op_push(ops, ";");
		      ungetc(a, teststre);
		    }
	          else
		    {
		      numOfLine++;
		      goto SWIFT;
		    }
		  continue;
	       }
	    }

	  if (comminput == NULL)
	    {
	      op_push(ops, "(");
	      continue;
	    }
	  fprintf(stderr, "%d: comminput is not empty\n", numOfLine);
          exit(1);
	}

      isOperator = 0;
      int indexSize = wordout_size;
      wordout_size++;
      wordout = (char *)checked_realloc(wordout, sizeof(char) * (1 + indexSize));
      wordout[indexSize] = a;
    }

  proSimple(cmds);
  removeCommand();
  if ((!ifOpStackEmpty(ops)) || (!ifCmdStackEmpty(cmds)))
    workStacks(ops, cmds);
  return head1;
}

command_t
read_command_stream (command_stream_t s)
{
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
