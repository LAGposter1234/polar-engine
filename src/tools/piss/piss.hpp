#ifndef PISS_HPP
#define PISS_HPP

#ifdef __cplusplus
extern "C" {
#endif

void polar_initpiss();
void polar_piss_open();
void polar_piss_main();
void polar_piss_free(char** args);
char** polar_piss_tokenize(char* cmd);
int polar_piss_countargs(char** args);
int polar_piss_run(int argc, char** argv);
bool polar_piss_scriptexists(char* name);

#ifdef __cplusplus
}
#endif

#endif
