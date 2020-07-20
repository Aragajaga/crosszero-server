#ifndef CZSRV_DATABASE_H_
#define CZSRV_DATABASE_H_

#include <iostream>
#include <string>
#include <sqlite3.h>

struct UserCredentials {
  std::string passwordHash;
  std::string salt;
};

class Database {
public:
  static Database* GetInstance() {
    static Database* ptr = new Database();
    return ptr;
  }

  void Open() {
    sqlite3_stmt* res;
    int rc = sqlite3_open("users.db", &m_db);

    if (rc != SQLITE_OK) {
      std::cout << "Cannot open database: " << sqlite3_errmsg(m_db)
                << std::endl;
      sqlite3_close(m_db);
    } else {
      std::cout << "Database opened!" << std::endl;
    }

    rc = sqlite3_prepare_v2(m_db, "SELECT SQLITE_VERSION()", -1, &res, 0);

    if (rc != SQLITE_OK) {
      std::cout << "Failed to fetch data: " << sqlite3_errmsg(m_db)
                << std::endl;
      sqlite3_close(m_db);
    }

    rc = sqlite3_step(res);

    if (rc == SQLITE_ROW) {
      std::cout << sqlite3_column_text(res, 0) << std::endl;
    }

    sqlite3_finalize(res);
    sqlite3_close(m_db);
  }

private:
  Database(Database&&) = delete;
  Database(const Database&) = delete;
  Database& operator=(Database&&) = delete;
  Database& operator=(const Database&) = delete;

  Database(){};

  void Prepare() {
    int rc = sqlite3_exec(
        m_db,
        "CREATE TABLE users(id INT PRIMARY KEY "
        "AUTOINCREMENT, name TEXT NOT NULL, password TEXT NOT NULL);",
        0, 0, nullptr);
  }

  int InsertUser(std::string name, std::string password) {
    std::string sql = "INSERT INTO users (name,password) VALUES('" + name +
                      "' '" + password + "');";
    int rc = sqlite3_exec(m_db, sql.c_str(), 0, 0, nullptr);
    return sqlite3_last_insert_rowid(m_db);
  }

  UserCredentials GetUser(std::string name) {
    UserCredentials cred;
    sqlite3_stmt* res;

    std::string sql =
        "SELECT password, salt FROM users WHERE name = '" + name + "';";
    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), -1, &res, 0);

    int step = sqlite3_step(res);

    if (step == SQLITE_ROW) {
      cred.passwordHash =
          reinterpret_cast<const char*>(sqlite3_column_text(res, 0));
      cred.salt = reinterpret_cast<const char*>(sqlite3_column_text(res, 1));
    }

    sqlite3_finalize(res);

    return cred;
  }

private:
  sqlite3* m_db;
};

#endif  // CZSRV_DATABASE_
