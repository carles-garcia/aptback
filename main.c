/*
Copyright (C) 2016  Carles Garcia Cabot  (github.com/carles-garcia)
This file is part of aptback, a tool to search, install, remove and upgrade
packages logged by apt. Released under the GNU GPLv3 (see COPYING.txt)
*/
#include <stdio.h>
#include <stdlib.h>
#include <argp.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/wait.h>

#include "log_parse.h"
#include "debug.h"
#include "selection.h"
#include "darray.h"
#include "print_search.h"
#include "argp_aux.h"

const char *argp_program_version = "aptback v1.1.0beta";
const char *argp_program_bug_address = "https://github.com/carles-garcia/aptback/issues";
static char doc[] =
    "aptback -- a tool to search, install, remove and upgrade packages logged by apt";

static char usage[] =
    "Usage: aptback [-s {OPTIONS}] -d DATE [-u DATE] [-m|-a] [-e|-v] [-t]\n\
       aptback install -s {OPTIONS} -d DATE [-u DATE] [-m|-a] [-y]\n\
       aptback remove -s {OPTIONS} -d DATE [-u DATE] [-m|-a] [-y]\n";

static char args_doc[] = "";

static struct argp_option options[] = {
    {"date",	'd', "DATE", 0, "Select packages matching a valid date. Format: yyyy-mm-dd-hh-mm-ss. Year is mandatory" },
    {"until",	'u', "DATE", 0, "If date specified, select packages in the interval [date,until] (both included)" },
    {"select",   	's', "OPTIONS", 0, "Select packages that were installed, removed, purged and/or upgraded. OPTIONS must include at least one of the following, separated by comma: {installed,removed,purged,upgraded}" },
    {"yes",	'y',	0,	0, "With 'install' and 'remove': always call apt-get, assume Yes to all queries and do not prompt" },
    {"automatic",	'a',	0,	0, "Select only automatically installed packages"},
    {"manual",	'm',	0,	0, "Select only manually installed packages (exclude automatic)"},
    {"export",	'e',	0,	0, "Print only package names separated by a single space. This is useful to call apt-get with the selected packages if advanced options are needed."},
    {"export-version",	'v',	0,	0, "Print only package names and versions separated by a single space. If the selected package was upgraded, print old version. This is useful to downgrade packages with apt-get."},
    {"statistics",	't',	0,	0, "Print statistics"},
    {"user",	'r',	0,	0, "Print user that requested action"},
    {"help",	-1,	0,	OPTION_HIDDEN, "Print help message"},
    {"usage",	-1,	0,	OPTION_HIDDEN|OPTION_ALIAS, 0},
    { 0 }
};

static void help(struct argp_state *state) {
    fprintf(stderr, "%s\n\n%s\n", doc, usage);
    argp_state_help(state, stderr, ARGP_HELP_LONG|ARGP_HELP_BUG_ADDR);
    exit(EXIT_FAILURE);
}

static int select_opt = 0;
static error_t parse_opt(int key, char *arg, struct argp_state *state) {
    struct arguments *arguments = state->input;
    switch (key) {
    case 'd':
        if (!parse_date(arg, &arguments->dat)) help(state);
        break;
    case 'u':
        if (!parse_date(arg, &arguments->until)) help(state);
        break;
    case 's':
        if (!parse_select(arg, arguments)) help(state);
        select_opt = 1;
        break;
    case 'y':
        arguments->yes = 1;
        break;
    case 'a':
        arguments->automatic = 1;
        break;
    case 'm':
        arguments->manual= 1;
        break;
    case 'e':
        arguments->exp = 1;
        break;
    case 'v':
        arguments->exp = 1;
        arguments->version = 1;
        break;
    case 't':
        arguments->stats = 1;
        break;
    case 'r':
        arguments->user = 1;
        break;
    case -1:
        help(state);
        break;
    case ARGP_KEY_ARG:
        if (strcmp(arg, "remove") == 0) arguments->option = REMOVE;
        else if (strcmp(arg, "install") == 0) arguments->option = INSTALL;
        else help(state);
        break;
    case ARGP_KEY_END:
        if (state->arg_num == 0) {
            arguments->option = SEARCH;
            if (!select_opt) {
                arguments->installed = 1;
                arguments->removed = 1;
                arguments->upgraded = 1;
                arguments->purged = 1;
            }
        }
        else if (state->arg_num != 1) help(state);
        else if (!select_opt) help(state);
        if (arguments->dat.year == -1) help(state);
        if (arguments->manual && arguments->automatic) {
            fprintf(stderr, "Options -a and -m are mutually exclusive\n");
            exit(EXIT_FAILURE);
        }
        break;
    default:
        return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = {options, parse_opt, args_doc, doc};

static void init_args(struct arguments *args) {
    *args = (struct arguments){0};
    args->until.year = args->until.month = args->until.day = args->until.hour = args->until.minute = args->until.second = -1;
    args->dat.year = args->dat.month = args->dat.day = args->dat.hour = args->dat.minute = args->dat.second = -1;
    args->option = UNDEFINED;
}

int main(int argc, char *argv[]) {
    const char *apt_path = "/var/log/apt/";

    /* Input arguments processing */
    struct arguments args;
    init_args(&args);
    argp_parse(&argp, argc, argv, 0, 0, &args);

    /* Apt-log search and processing */
    struct darray actions;
    init_darray(&actions);
    DIR *apt_dir = NULL;
    struct dirent *in_file = NULL;
    FILE *log_file = NULL;

    if ((apt_dir = opendir(apt_path)) == NULL) eperror("Failed to open log directory");
    while ((in_file = readdir(apt_dir))) {
        int pid = -1;
        if (starts_with(in_file->d_name, "history.log.")) {
            int fildes[2];
            if (pipe(fildes) == -1) eperror("Failed to create pipe");
            pid = fork();
            if (pid == 0) {
                if (close(fildes[0]) == -1) eperror("Failed to close read fildes");
                if (dup2(fildes[1], 1) == -1) eperror("Failed to dup2 pipe");
                if (close(fildes[1]) == -1) eperror("Failed to close fildes[1] after dup2");
                char path[strlen(apt_path) + strlen(in_file->d_name)];
                sprintf(path, "%s%s", apt_path, in_file->d_name);
                if (execlp("zcat", "zcat", path, NULL) == -1)
                    eperror("Failed to exec to zcat");
            }
            else if (pid == -1) eperror("Failed to fork process to execute zcat");
            else {
                if (close(fildes[1]) == -1) eperror("Failed to close write fildes");
                log_file = fdopen(fildes[0], "r");
                if (log_file == NULL) eperror("Failed to fdopen pipe to read");
            }
        }
        else if (strcmp(in_file->d_name, "history.log") == 0) {
            char path[strlen(apt_path) + strlen(in_file->d_name)];
            sprintf(path, "%s%s", apt_path, in_file->d_name);
            log_file = fopen(path, "r");
            if (log_file == NULL) eperror("Failed to open log_file to read");
        }
        else continue; // other irrelevant files

        struct action *current = NULL;
        char *line = NULL;
        size_t n = 0;
        while (getline(&line, &n, log_file) > 0) {
            evaluate_line(line, &current, &actions, &args);
            free(line);
            line = NULL;
            n = 0;
        }
        free(line);
        if (fclose(log_file) != 0) eperror("Failed to close log_file");
        if (pid != -1) {
            int status;
            if (waitpid(pid, &status, 0) == -1) eperror("waitpid failed");
            if (WIFEXITED(status) == 0) {
                fprintf(stderr, "\nzcat finished abnormally\n");
                exit(EXIT_FAILURE);
            }
        }
    }
    if (closedir(apt_dir) == -1) eperror("Failed to close log directory");

    /* Actions selection based on input */
    struct darray selected;
    init_darray(&selected);
    struct statistics stats = {0};
    int user_len = 0;
    selection(&args, &actions, &selected, &stats, &user_len);
    if (stats.num_selected > 0) {
        if (args.stats) {
            print_stats(&stats);
        }
        if (args.option == SEARCH) {
            qsort(selected.array, selected.size, sizeof(struct action *), actioncmp);
            if (args.exp) print_export(&selected, args.version);
            else print_search(&selected, args.user, user_len);
        }
        else {
            if (!args.yes) {
                printf("%d packages selected:\n", stats.num_selected);
                print_preview(&selected);
                char input = 0;
                do {
                    if (input != '\n') printf("Call apt-get? [y/n]\n");
                    input = getchar();
                    if (input == 'n') exit(EXIT_FAILURE);
                }
                while (input != 'y');
            }

            /* Apt-get call */ // should ask for confirmation before calling apt if action is install
            int argv_size = stats.num_selected + 2 + 1;  // +2 for the first 2, +1 for the last NULL
            if (args.yes) ++argv_size;
            char *apt_argv[argv_size];
            int num = 0;
            apt_argv[num++] = "apt-get";
            if (args.option == INSTALL) apt_argv[num++] = "install";
            else if (args.option == REMOVE) apt_argv[num++] = "remove";
            if (args.yes) apt_argv[num++] = "--yes";
            apt_argv[argv_size-1] = NULL;
            for (int k = 0; k < selected.size; ++k) {
                for (int l = 0; l < darray_get(&selected, k)->packages.size; ++l) {
                    apt_argv[num++] = darray_pack_get(&(darray_get(&selected, k)->packages), l)->name;
                }
            }
            int pid = fork();
            if (pid == 0) {
                if (execvp(apt_argv[0], apt_argv) == -1) eperror("Failed to exec to apt-get"); // if sudo is needed it will tell
            }
            else if (pid == -1) eperror("Failed to fork process to execute apt-get");
            else {
                int status;
                if (waitpid(pid, &status, 0) == -1) eperror("waitpid for apt-get failed");
                if (WIFEXITED(status) == 0) {
                    fprintf(stderr, "\napt-get finished abnormally\n");
                    exit(EXIT_FAILURE);
                }
            }
        }
    }
    else printf("No packages match the arguments\n");

    /* Free allocated memory left */
    for (int i = 0; i < actions.size; ++i)
        free_action(darray_get(&actions, i));
    free_darray(&actions);
    free_darray(&selected);

    return 0;
}
