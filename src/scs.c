#include "scs.h"
            
struct _scs {
	char *str;
	int size;
};
    
scs *scs_create(const char* cstr)
{                                                  
	return scs_ncreate(cstr, strlen(cstr));
}          

scs *scs_ncreate(const char* buf, int size) 
{
	scs *s = (scs *)malloc(sizeof(*s));
	s->str = (char *)malloc(sizeof(char) * size);
	memcpy(s->str, buf, size);
	s->size = size;
	
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

void scs_clear(scs *s)
{
	s->size = 0;
}