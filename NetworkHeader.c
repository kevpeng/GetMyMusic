#include "NetworkHeader.h"

void DieWithError(char *errorMessage) { perror(errorMessage); exit(1); } /*Error handling function */

void DieWithSystemMessage(char *msg)
{
/*Error handling function */
  perror(msg);
  exit(1);
}

void DieWithUserMessage(const char *msg, const char *detail)
{
  fputs(msg, stderr);
  fputs(": ", stderr);
  fputs(detail, stderr);
  fputc('\n', stderr);
	exit(1);
}
