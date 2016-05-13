#include "Utility.h"

char* itoa(long i, char* s, int dummy_radix)
{
	sprintf(s, "%ld", i);

	return s;
}

char *trimwhitespace(char *str)
{
	char *end;
	char * head = (str);

	// Trim leading space
	while (isspace(*head)) str++;

	if (*head == 0)  // All spaces?
		return head;

	// Trim trailing space
	end = head + strlen(head) - 1;
	while (end > head && isspace(*end)) end--;

	// Write new null terminator
	*(end + 1) = 0;

	return  (head);
}
void chomp(char *s) {
	while (*s && *s != '\n' && *s != '\r') s++;

	*s = 0;
}