#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/stat.h>
#include <limits.h>
#include "../../include/render.h"
#include "../../include/assetbundler.h"
#include "../../include/render.h"
extern "C" int polar_enginestart(const char* lua_path);
//The C standard library is just better than the C++ library, you cant prove me otherwise because i am right

extern "C" {

FILE* mainlua;
FILE* edit_current;
const float PISS_VERSION = 0.1f;
mode_t permissions = S_IRWXU | S_IRWXG | S_IRWXO;

bool polar_piss_scriptexists(const char* name) {
	FILE* file;
	file = fopen(name, "r");
	if (file == NULL) return false;
	fclose(file);
	return true;
}

void polar_piss_free(char** args) {
	for (int i = 0; args[i]; i++) free(args[i]);
	free(args);
}

char** polar_piss_tokenize(char* cmd) {
	int argc = 0;
	int capacity = 8;
	char** args = (char**)malloc(capacity * sizeof(char*));

	char buf[256];
	int buf_len = 0;

	for (int i = 0; ; i++) {
		char c = cmd[i];
		if (c == ' ' || c == '\0') {
			if (buf_len > 0) {
				if (argc >= capacity - 1) {
					capacity *= 2;
					args = (char**)realloc(args, capacity * sizeof(char*));
				}
				buf[buf_len] = '\0';
				args[argc++] = strdup(buf);
				buf_len = 0;
			}
			if (c == '\0') break;
		} else {
			buf[buf_len++] = c;
		}
	}

	args[argc] = NULL;
	return args;
}

int polar_piss_countargs(char** args) {
	int count = 0;
	for (count = 0; args[count] != NULL; count++);
	
	return count;
}

int polar_piss_run(int argc, char** argv) {
	char* name = argv[0];
	if (strcmp(name, "run") == 0) {
		polar_enginestart("./main.lua");
		return 0;
	}
	if (strcmp(name, "help") == 0) {
		printf("Simple Lua engine\nUses a simple shell to manipulate things. PISS stands for Polar Interactive Shell System, by the way.\n\nCommands:\nrun - Run the engine as if you were playing it. Good for debugging\nhelp - Display this list.\npeek - Basically just cat\ncompile - Compile assets and lua files into a portable game folder. Rename output folder as you please.\nversion - Display the current PISS version.\n");
		return 0;
	}
	if (strcmp(name, "peek") == 0) {
		if (argc < 2) { printf("PISS: peek: missing filename\n"); return 1; }
		FILE* f = fopen(argv[1], "r");
		if (!f) { printf("PISS: peek: could not open %s\n", argv[1]); return 1; }
		char buf[256];
		while (fgets(buf, sizeof(buf), f)) printf("%s", buf);
		fclose(f);
		return 0;
	}
	if (strcmp(name, "compile") == 0) {
		assetbundler_bundle(".", "../output");
		return 0;
	}
	if (strcmp(name, "version") == 0) {
		printf("PISS version %f\n", PISS_VERSION);
		return 0;
	}
	printf("PISS Run Warning: I dont know what %s is.\n", name);
	return 1;
}

void polar_piss_main() {
	char command[256];
	char cwd[PATH_MAX];
	bool running = true;
	while (running) {
		if (getcwd(cwd, sizeof(cwd)) == NULL) {
			printf("PISS Main Error: getcwd returned NULL\n");
			return;
		}
		
		printf("user@piss: %s $ ", cwd);
		fgets(command, sizeof(command), stdin);
		command[strcspn(command, "\n")] = '\0';
		
		char** argv = polar_piss_tokenize(command);
		if (argv[0] == NULL) { polar_piss_free(argv); continue; }
		if (strcmp(argv[0], "quit") == 0) { running = false; continue; }
		
		int argc = polar_piss_countargs(argv);
		int returned = polar_piss_run(argc, argv);
		polar_piss_free(argv);
		if (returned != 0) {
			printf("PISS Main Warning: polar_piss_run() returned non-zero\n");
		}
	}
}

void polar_piss_open() {
	printf("PISS version %f\n", PISS_VERSION);
	
	if (chdir("./data") != 0) {
		printf("PISS Open Error: chdir() returned non-zero, try running ./polar init if there is no polar data folder\n");
		return;
	}
	if (!polar_piss_scriptexists("main.lua")) {
		printf("PISS Open Error: Couldnt find main.lua, try deleting the data folder and running ./polar init\n");
		return;
	}
	mainlua = fopen("main.lua", "r+");
	if (mainlua == NULL) {
		printf("PISS Open Error: Could not open main.lua\n");
		return;
	}
	polar_piss_main();
	return;
}

void polar_initpiss() {
	printf("PISS version %f\n", PISS_VERSION);
	
	if (mkdir("./data", permissions) != 0) {
		printf("PISS Init Error: mkdir() returned non-zero\n");
	}
	if (chdir("./data") != 0) {
		printf("PISS Init Error: chdir() returned non-zero\n");
		return;
	}
	
	if (polar_piss_scriptexists("main.lua")) {
		printf("PISS Init Resign: Found main.lua, try using ./polar open instead of ./polar init\n");
		return;
	}
	mainlua = fopen("main.lua", "w");
	if (mainlua == NULL) {
		printf("PISS Init Error: Could not create main.lua\n");
		return;
	}
	polar_piss_main();
	return;
}

}
