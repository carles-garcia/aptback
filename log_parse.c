
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
	
	


//for each line
evaluate_line(char *line, NULL);

void evaluate_line(char *line, struct action *current) {
	if (starts_with(line, "Start-Date") { 
		current = struct action *new_action;
		init_action(new_action);
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
	while (*line++ != ' '); //pass space
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
	while (*line++ != ' '); //pass space
	while (1) {
		struct package new_pack;
		char *line_aux = line;
		int c = 0;
		while (*line_aux != ':') {
			++c; //number of chars in name
			++line_aux;
		}
		++line_aux;
		new_pack.name = malloc((c+1) * sizeof(char));
		int i;
		for (i = 0; i < c; ++i) {
			new_pack.name[i] = line[i];
		}
		line = line_aux;
		c = 0;
		while (*line_aux != ' ') {
			++c; //number of chars in arch
			++line_aux;
		}
		++line_aux;
		++line_aux; //now we are at version
		new_pack.arch = malloc((c+1) * sizeof(char));
		for (i = 0; i < c; ++i) {
			new_pack.arch[i] = line[i];
		}
		while (*line_aux != ',' && *line_aux != ')') {
			++c; //number of chars in version
			++line_aux;
		}
		if (*line_aux == ',') new_pack.automatic = true;
		else new_pack.automatic = false;
		new_pack.version = malloc((c+1) * sizeof(char));
		for (i = 0; i < c; ++i) {
			new_pack.version[i] = line[i];
		}
		//package finished
		//add package to action list here
		current->num_pack += 1;
		current->packages = realloc(current->packages, num_pack * sizeof(struct package *)); //maybe it would be more efficient to count "), " + 1
		// which is the number of packages in the line and do only one big malloc
		current->packages[current->num_pack-1] = new_pack;
		
		line = line_aux;
		++line; // now it's either in ' ' or in ',' or end
		while (*line != ',' && *line != '\n') ++line;
		
		if (*line == '\n') break; // exit point: all packages checked 
		else {
			++line;
			++line;
		}
		// now we are in new package name
	}
}

void init_action(struct action *current) {
	current->command = NULL;
	current->packages = malloc(0 * sizeof(struct package *));
	current->num_pack = 0;
}
