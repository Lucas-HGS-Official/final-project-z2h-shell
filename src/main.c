#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
    printf("Usage: %s -n -f <database file>\n", argv[0]);
    printf("\t -n  - Create new database file\n");
    printf("\t -f  - (Required) path to database file\n");

    return;
}

int main(int argc, char *argv[]) {

	int c = 0, dbfd = -1;
    bool newfile = false, list = false;
    char *filepath = NULL, *addstring = NULL;
    struct dbheader_t *dbhdr = NULL;
    struct employee_t *employees = NULL;

    while ((c = getopt(argc, argv,"nf:a:l")) != -1) {
        switch (c) {
            case 'n':
                newfile = true;
                break;
            case 'f':
                filepath = optarg;
                break;
            case 'a':
                addstring = optarg;
                break;
            case 'l':
                list = true;
                break;
            case '?':
                printf("Unknown option -%c\n", c);
                break;
            
            default:
                return -1;
                break;
        }
    }

    if (filepath == NULL) {
        printf("Filepath is a required argument\n");
        print_usage(argv);

        return 0;
    }

    if (newfile) {
        dbfd = create_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to create database file\n");
            return STATUS_ERROR;
        }

        if (create_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
            printf("Malloc failed to create DB header\n");
            return STATUS_ERROR;
        }
    } else {
        dbfd = open_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to open database file\n");
            return STATUS_ERROR;
        }

        if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
            printf("Failed to validade DB header\n");
            return STATUS_ERROR;
        }
    }

    if (read_employees(dbfd, dbhdr, &employees) != STATUS_SUCCESS) {
        printf("Failed to read the employees\n");
        return 0;
    }

    if (addstring) {
        dbhdr->count++;
        employees = realloc(employees, dbhdr->count*(sizeof(struct employee_t)));
        add_employee(dbhdr,employees, addstring);
    }

    if (list) {
        list_employees(dbhdr, employees);
    }

    output_file(dbfd, dbhdr, employees);

    return 0;
}
