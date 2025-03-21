#include <mysql.h>
#include <iostream>

int main() {
    MYSQL *conn;
    MYSQL_RES *res;
    MYSQL_ROW row;

    // 初始化 MySQL 连接
    conn = mysql_init(NULL);
    if (conn == NULL) {
        std::cerr << "mysql_init failed!" << std::endl;
        return EXIT_FAILURE;
    }

    // 连接到数据库
    if (mysql_real_connect(conn, "localhost", "root", "123456","myprojectdb", 0, NULL, 0) == NULL) {
        std::cerr << "mysql_real_connect failed!" << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // 执行 SQL 查询
    if (mysql_query(conn, "SELECT * FROM users")) {
        std::cerr << "Query failed: " << mysql_error(conn) << std::endl;
        mysql_close(conn);
        return EXIT_FAILURE;
    }

    // 处理查询结果
    res = mysql_store_result(conn);
    if (res == NULL) {
        std::cerr << "mysql_store_result failed!" << mysql_error(conn) << std::endl;
    } else {
        while ((row = mysql_fetch_row(res))) {
            for (int i = 0; i < mysql_num_fields(res); ++i) {
                std::cout << row[i] << " ";
            }
            std::cout << std::endl;
        }
        mysql_free_result(res);
    }

    // 关闭连接
    mysql_close(conn);
    return EXIT_SUCCESS;
}

