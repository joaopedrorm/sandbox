#include <stdio.h>
#include <sqlite3.h>
#include <string.h>

int main(int argc, char **argv){
  sqlite3 *db;
  char *zErrMsg = 0;
  int rc;

  sqlite3_stmt *stmt;

  if( argc < 3 ){
    fprintf(stderr, "Usage: %s DATABASE PREPARED-SQL-STATEMENT\n", argv[0]);
    return(1);
  }

  rc = sqlite3_open(argv[1], &db);
  if( rc ){
    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
    sqlite3_close(db);
    return(1);
  }

  rc = sqlite3_prepare_v2 (db, argv[2], -1, &stmt, 0);
  if (rc != SQLITE_OK){
    fprintf(stderr, "Prepare error(%d): %s\n", rc, sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return (1);
  }

  if (argc > 3){
    rc = sqlite3_bind_text (stmt, 1, argv[3], strlen(argv[3]), SQLITE_STATIC);
    if (rc != SQLITE_OK){
      fprintf(stderr, "Bind error(%d): %s\n", rc, sqlite3_errmsg(db));
      sqlite3_finalize(stmt);
      sqlite3_close(db);
      return (1);
    }
  }

  int row =0;
  do {

  rc = sqlite3_step(stmt);
  if (rc != SQLITE_DONE && rc != SQLITE_ROW){
    fprintf(stderr, "Step error(%d): %s\n", rc, sqlite3_errmsg(db));
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return (1);
  }

  zErrMsg = sqlite3_column_text (stmt, 0);

  fprintf(stdout, "RC %d, Row %d, Column 0 ='%s'\n", rc, row, zErrMsg);

  } while(rc == SQLITE_ROW);

  rc = sqlite3_finalize(stmt);
  if (rc != SQLITE_OK){
    fprintf(stderr, "Finalize error(%d): %s\n", rc, sqlite3_errmsg(db));
    sqlite3_close(db);
    return (1);
  }

  sqlite3_close(db);
  return 0;
}
