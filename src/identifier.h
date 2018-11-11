#ifndef IDENTIFIER_H
#define IDENTIFIER_H

typedef char *identifier;

void idCreate(identifier *id, const char *string, const byte_t length){
	id[0] = length;
	strcpy(&id[1], string);
}

#endif
