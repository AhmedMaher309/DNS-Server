#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

const char* 
createSQLiteDatabase(const char* dbName) {
    sqlite3* db;
    int rc;

    // Attempt to open the database. If it doesn't exist, it will be created.
    rc = sqlite3_open(dbName, &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return NULL; // Error opening the database
    }

    // Close the database
    sqlite3_close(db);

    return dbName; // Return the name of the database
}



// a function to store the name and the ip address in the database
int 
storeNameIPInDatabase(const char* dbName, const char* name, const char* ipAddress) {
    sqlite3* db;
    char* errMsg = 0;
    int rc;

    // Open the existing SQLite database
    rc = sqlite3_open(dbName, &db);

    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return 1; // Error opening the database
    }

    // Insert the name and IP address into the table
    const char* insertDataSQL = "INSERT INTO NameIPTable (Name, IP) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    rc = sqlite3_prepare_v2(db, insertDataSQL, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1; // Error preparing the SQL statement
    }

    sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, ipAddress, -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        fprintf(stderr, "SQL error: %s\n", sqlite3_errmsg(db));
        return 1; // Error executing the SQL statement
    }

    sqlite3_finalize(stmt);

    // Close the database
    sqlite3_close(db);

    return 0; // Success
}


// search in the database about an ip address of a specific name and returns it, else return NULL
char* 
searchIPByName(const char* dbName, const char* name) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    if (sqlite3_open(dbName, &db) != SQLITE_OK) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return NULL; // Error opening the database
    }

    const char* searchSQL = "SELECT IP FROM NameIPTable WHERE Name = ?";
    if (sqlite3_prepare_v2(db, searchSQL, -1, &stmt, 0) != SQLITE_OK) {
        sqlite3_close(db);
        return NULL; // Error preparing the SQL statement
    }

    if (sqlite3_bind_text(stmt, 1, name, -1, SQLITE_STATIC) != SQLITE_OK) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return NULL; // Error binding parameters
    }

    char* ipAddress = NULL;

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const char* resultIP = (const char*)sqlite3_column_text(stmt, 0);
        ipAddress = strdup(resultIP); // Duplicate the result to manage memory
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return ipAddress;
}


int insertDataIntoDatabase(const char *dbName, const char *ip, const char *hostname) {
    sqlite3 *db;
    char *errMsg = 0;
    int rc;

    // Open the database
    rc = sqlite3_open(dbName, &db);
    if (rc) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        return rc;
    }

    // Create the table (if it doesn't exist)
    char *sql = "CREATE TABLE IF NOT EXISTS hosts (id INTEGER PRIMARY KEY AUTOINCREMENT, ip TEXT, hostname TEXT);";
    rc = sqlite3_exec(db, sql, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return rc;
    }

    // Insert data into the table
    char insertSQL[150];
    sprintf(insertSQL, "INSERT INTO hosts (ip, hostname) VALUES ('%s', '%s');", ip, hostname);
    rc = sqlite3_exec(db, insertSQL, 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\n", errMsg);
        sqlite3_free(errMsg);
        sqlite3_close(db);
        return rc;
    }

    // Close the database
    sqlite3_close(db);

    return SQLITE_OK;
}


