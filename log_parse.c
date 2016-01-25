
struct date {
	int year;
	int month;
	int day;
	int hour;
	int minute;
	int second;
}

enum action_type { INSTALL, REMOVE };

struct package {
	char *name;
	char *arch;
	char *version;
	int automatic;
}	
	
struct action {
	struct date start_date;
	char *command;
	enum action_type type;
	struct package **packages;
	int num_pack;
	struct date end_date;
}
	
	
/*
Start-Date: 2015-09-09  14:27:34
Commandline: apt-get install bin86
Install: bin86:i386 (0.16.17-3.1ubuntu3)
End-Date: 2015-09-09  14:27:34

Start-Date: 2015-09-09  14:32:30
Commandline: apt-get install bochs
Install: bochs-wx:i386 (2.4.6-6, automatic), bximage:i386 (2.4.6-6, automatic), bochsbios:i386 (2.4.6-6, automatic), libwxbase2.8-0:i386 (2.8.12.1+dfsg-2ubuntu2, automatic), libwxgtk2.8-0:i386 (2.8.12.1+dfsg-2ubuntu2, automatic), vgabios:i386 (0.7a-3ubuntu2, automatic), bochs:i386 (2.4.6-6)
End-Date: 2015-09-09  14:32:32

Start-Date: 2015-09-09  14:48:37
Commandline: apt-get remove bochs
Remove: bochs-wx:i386 (2.4.6-6), bochs:i386 (2.4.6-6)
End-Date: 2015-09-09  14:48:37
*/

//for each line
evaluate_line(char *line, NULL);

void evaluate_line(char *line, struct action *current) {
	if (starts_with(line, "Start-Date") { 
		current = struct action *new_action;
		get_date(line, current->start_date);
	}
	else if (starts_with(line, "Commandline")) { //not all actions have one 
		get_command(line, current);
	}
	else if (starts_with(line, "Install")) { //not all actions have one
		current->type = INSTALL;
		get_packages(line, current);
	}
	else if (starts_with(line, "Remove")) { //not all actions have one
		current->type = REMOVE;
		get_packages(line, current);
	}
	else if (starts_with(line, "End_Date")) {
		get_date(line, current->end_date);
	}
	else if (*line != '\n') exit(EXIT_FAILURE); //ERROR
}

int starts_with(char *line, char *string) {
	// line should never be NULL
	while (1) {
		if (!isalpha(*line)) {
			if (*string == '\0') return 1;
			else return 0;
		}
		else if (!isalpha(*string)) return 0;
		else if (*line++ != *string++) return 0;
	}
}

void get_date(char *line, struct date dat) {
	while (*line++ != ' '); //pass dash -
	char year[4];
	int i = 0;
	while (i < 4) year[i++] = *line++;
	dat.year = atoi(year);
	++line;
	char month[2];
	i = 0;
	while (i < 2) month[i++] = *line++;
	dat.month = atoi(month);
	++line;
	char day[2];
	i = 0;
	while (i < 2) day[i++] = *line++;
	dat.day = atoi(day);
	++line;
	
	while (isspace(*line)) ++line;
	char hour[2];
	i = 0;
	while (i < 2) hour[i++] = *line++;
	dat.hour = atoi(hour);
	++line;
	char minute[2];
	i = 0;
	while (i < 2) minute[i++] = *line++;
	dat.minute = atoi(minute);
	++line;
	char second[2];
	i = 0;
	while (i < 2) second[i++] = *line++;
	dat.second = atoi(second);
	
}
		
void get_command(char *line, struct action *current) {
	while (!isspace(*line++)); 
	while (isspace(*line)) ++line;
	// now we have the command
	current->command = malloc((strlen(line)+1) * sizeof(char)); //will free when exiting program
	strcpy(current->command, line); 
}

void get_packages(line, current) {
	
}

