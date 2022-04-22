#include "include/actor.hpp"
#include "include/handler.hpp"

using namespace std;


int main(){
    // sql::Driver *dr = get_driver_instance();
    // sql::Connection *conn = dr->connect("localhost", "abhinav", "1141");
    // sql::Statement *stmt = conn->createStatement();

    // unsigned char ibuf[] = "1234";
    // unsigned char obuf[SHA256_DIGEST_LENGTH];
    // string s;

    // SHA256(ibuf, 5, obuf);
    // cout << obuf << endl;


    Admin admin;
    admin.reset();
    admin.insert_default_data();
    cout << 1 << endl;
    cout << admin.get_num_cols() << endl;
    cout << 1 << endl;

    admin.exit();

    cout << get_current_date() << endl;

    return 0;
}