#ifndef SCS_H
#define SCS_H
    
#ifdef __cplusplus 
extern "C" { 
#endif

struct _scs;
typedef struct _scs scs;
      
scs *scs_create(const char* cstr);
void scs_append(scs *s, const char* cstr); 
void scs_nappend(scs *s, const char* cstr, int size);
char *scs_get_content(scs *s);
int scs_get_size(scs *s);
void scs_free(scs *s);

#ifdef __cplusplus 
} 
#endif

#endif /* end of include guard: SCS_H */
