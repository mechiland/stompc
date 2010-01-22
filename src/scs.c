#include "scs.h"
            
struct _scs {
	char *str;
	int size;
};
    
scs *scs_create(const char* cstr)
{                                                  
	scs *s = (scs *)malloc(sizeof(*s));
	s->str = (char *)malloc(sizeof(char) * strlen(cstr));
	memcpy(s->str, cstr, strlen(cstr));
	s->size = strlen(cstr);
	
	return s;
}   

void scs_append(scs *s, const char* cstr)
{
	scs_nappend(s, cstr, strlen(cstr));
}

void scs_nappend(scs *s, const char* cstr, int size)
{                                                       
	if (size <= 0)
		return;
		
	s->str = (char *)realloc(s->str, size);
	memcpy(s->str + s->size, cstr, size);
	s->size += size;	
}

char *scs_get_content(scs *s)
{
	return s->str;
}   

int scs_get_size(scs *s) 
{
	return s->size;
}           

void scs_free(scs *s) 
{
	free(s->str);
	free(s);
}
