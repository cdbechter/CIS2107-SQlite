#include <stdio.h>
#include <string.h>
#include <sqlite3.h>

#define BUFFSIZE 64 //buff size for file output

int main(int argc, char **argv) {
	/* variables for database and output file */
    sqlite3 *db;                //pointer to database
	sqlite3_stmt *stmt;         //pointer to the prepared statement
    char fileName[BUFFSIZE];    //file output file
    char *mode = "w";           //pointer to output mode "w" = write
	
    /* opening database, assign to db, make sure the db exists */
	sqlite3_open("permissions.db", &db);
	if (db == NULL) {
		printf("Failed to open DB\n");
		return 1;
	}

    /* Asking for the output file name output.txt */
    printf("Please type the filname you wish to output:\n");
    scanf("%s", fileName);
    FILE *fpr;                      //file pointer to the output file
    fpr = fopen(fileName, mode);   //opens file and uses mode

    /* performing query on database using sqlite3_prepare_v2 which opens the databse,
        executes the select query you wish to find and output, uses nbyte (?), while
        the query executes and then stores the resulting results of the query to stmt address.
		------------------------------------------------------------------------------------------
		this is the Assignment 6 parameters, prints all the the tuIDs with their valid permissions
		making sure not to add those resources with expired permissions */
    sqlite3_prepare_v2(db, "SELECT id.tuID, r.name, rr.resource_ID, rrr.name, pr.role_expiration_date FROM person_id id INNER JOIN person_role pr ON id.tuID=pr.tuID INNER JOIN roles r ON pr.role_id=r.role_id INNER JOIN resource_role rr ON rr.role_id=r.role_id INNER JOIN resources rrr ON rrr.resource_id=rr.resource_id WHERE (pr.role_expiration_date) >= 20181127 ORDER BY id.tuID AND r.name;", -1, &stmt, NULL);
	
	/* headers for the output */
	printf("All permissions after todays date. \n\n");
	fprintf(fpr, "All permissions after todays date. \n\n");
	printf("tuID | Role | Resouce ID | Resource Name | Expiration Date |\n\n");
    fprintf(fpr, "tuID | Role | Resouce ID | Resource Name | Expiration Date |\n\n");
	
    /* uses the step function in sqlite3 to find how many columns, pull all the information
        using a for loop for the columns, and sqlite3 switch functions to decipher whether 
        or not the information is text or integer, prints it to a file specified above, 
        then when finished, STEPS to the next row, does the same thing, and finally ends once
        all the infor from the query is found "EOF" esstianlly */
	while (sqlite3_step(stmt) != SQLITE_DONE) {
		int i;
		int num_cols = sqlite3_column_count(stmt); /* find the number of columns */
		
		/* cycle through the columns, pulling out the required information before moving to the next row */
		for (i = 0; i < num_cols; i++) {
			/* switch to find the column type, in the current column i is in, and then find its case and id using stmt (statement) prepare */
			switch (sqlite3_column_type(stmt, i)) {
			/* this utilizes sqlite3 case to find if the column is a text type and has text */
			case (SQLITE3_TEXT):
				fprintf(fpr, "%s | ", sqlite3_column_text(stmt, i));
                printf("%s | ", sqlite3_column_text(stmt, i));
				break;
			/* this utilizes sqlite3 case to find if the column is an int and has an integer number */
			case (SQLITE_INTEGER):
				fprintf(fpr, "%d | ", sqlite3_column_int(stmt, i));
                printf("%s | ", sqlite3_column_text(stmt, i));
				break;
			/* this is just an example, not used in this project, but to show a switch could be used for all data types */
			case (SQLITE_FLOAT):
				fprintf(fpr, "%g | ", sqlite3_column_double(stmt, i));
                printf("%s | ", sqlite3_column_text(stmt, i));
				break;
			default:
				break;
			}
		}
		fprintf(fpr, "\n");
        printf("\n");
	}
    /* finalize is absolutely nesscary to close the current results of the 
        databse query so that there are no memory leaks and that further statements
        could be called for additional querys */
	sqlite3_finalize(stmt);

	/* space out the queries */
	printf("\n\n\n");
    fprintf(fpr, "\n\n\n");

	/* in this prepare statement, I used the SELECT DISTINCT statement for sqlite, it essentially just removes duplicate rows. 
		I also found, depending on what information you are trying to find, aka tuID, resource_ID, and lets say, role_name, 
		will actually throw an error while trying to format the data because they are no longer dupilcates, the rows now have 
		different signifying traits. */
	sqlite3_prepare_v2(db, "SELECT DISTINCT id.tuID, rr.resource_ID, rrr.name FROM person_id id INNER JOIN person_role pr ON id.tuID=pr.tuID INNER JOIN roles r ON pr.role_id=r.role_id INNER JOIN resource_role rr ON rr.role_id=r.role_id INNER JOIN resources rrr ON rrr.resource_id=rr.resource_id WHERE (pr.role_expiration_date) >= 20181127 ORDER BY id.tuID;", -1, &stmt, NULL);

	/* headers for the output */
    printf("tuID | Resouce ID | Resource Name\n\n");
    fprintf(fpr, "tuID | Resouce ID\n\n");

	/* goes column by column until its done, then moves down a row and repeats */
	while (sqlite3_step(stmt) != SQLITE_DONE) {
		int i;
		int num_cols = sqlite3_column_count(stmt);
		
		for (i = 0; i < num_cols; i++) {
			switch (sqlite3_column_type(stmt, i)) {
			case (SQLITE3_TEXT):
				fprintf(fpr, "%s | ", sqlite3_column_text(stmt, i));
                printf("%s | ", sqlite3_column_text(stmt, i));
				break;
			case (SQLITE_INTEGER):
				fprintf(fpr, "%d | ", sqlite3_column_int(stmt, i));
                printf("%s | ", sqlite3_column_text(stmt, i));
				break;
			case (SQLITE_FLOAT):
				fprintf(fpr, "%g | ", sqlite3_column_double(stmt, i));
                printf("%s | ", sqlite3_column_text(stmt, i));
				break;
			default:
				break;
			}
		}
		fprintf(fpr, "\n");
        printf("\n");
	}
	/* end the prepare statement -- needed */
	sqlite3_finalize(stmt);

    /* closes the database and file, end of program */
	sqlite3_close(db);
    fclose(fpr);
	return 0;
}