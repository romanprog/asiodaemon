#include <random>
#include <iostream>
#include <functional>
#include <memory>
#include <map>
#include <algorithm>

// friend class
#include <iostream>
#include "AsyncEvent/BufferBase/BuffAbstract.hpp"
#include "AsyncEvent/BufferBase/ParsingBuffAbstract.hpp"
#include "HUtils/HStrings.hpp"
#include <mysql.h>
#include <asio.hpp>

int i = 0;
asio::io_service io;
asio::ip::tcp::socket _socket(io);
MYSQL *conn;

void reader()
{
    if (i > 1)
        return;

    ++i;
    std::vector<char> result_test;
    result_test.resize(100);
    result_test.resize(1000);

    _socket.async_read_some(asio::buffer(result_test.data(), 0),
                            [&](asio::error_code ec, size_t sz)
    {
        if (!mysql_read_query_result(conn))
        {
            MYSQL_RES *res = mysql_store_result(conn);
            MYSQL_ROW row;

            printf("MySQL Tables in mysql database:\n");
            while ((row = mysql_fetch_row(res)) != NULL)
               printf("%s \n", row[0]);
            mysql_free_result(res);
            reader();

        } else
        {
            printf("mysql_read_query_result() failed for fd = %d: %s (%d)\n",
                            conn->net.fd,
                            mysql_error(conn),
                            mysql_errno(conn));
        }
    }
                            );
}

int main () {

    char *server = "localhost";
    char *user = "root";
    char *password = "kor011sg8"; /* set me first */
    char *database = "mysql";

    conn = mysql_init(NULL);
    /* Connect to database */
    if (!mysql_real_connect(conn, server,
          user, password, database, 0, NULL, 0)) {
       fprintf(stderr, "%s\n", mysql_error(conn));
       exit(1);
    }
    _socket.assign(asio::ip::tcp::v4(), conn->net.fd);

    std::string query("select sleep(1)");
    /* send SQL query */
    if (mysql_send_query(conn, query.c_str(), query.length())) {
       fprintf(stderr, "%s\n", mysql_error(conn));
       exit(1);
    }


    reader();

    std::cout << "Waiting mysql..." << std::endl;
    io.run();


    mysql_close(conn);
}

