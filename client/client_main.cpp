#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock 헤더파일 include. WSADATA 들어있음.
#include <WS2tcpip.h>
#include <string>
#include <sstream>
#include <iostream>
#include <thread>
#include <mysql/jdbc.h>
#include <conio.h> 
#include <Windows.h>

#define MAX_SIZE 1024

using std::cout;
using std::cin;
using std::endl;
using std::string;

const string server = "tcp://127.0.0.1:3306";
const string username = "root";
const string password = "1234";

sql::mysql::MySQL_Driver* driver; // 추후 해제하지 않아도 Connector/C++가 자동으로 해제해 줌 
sql::Connection* con;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;
sql::Statement* stmt;

SOCKET client_sock;
string my_nick;



void startMenu() {
    system("cls");
    cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■" << endl;
    cout << "■                                                            ■" << endl;
    cout << "■                       채팅 프로그램                        ■" << endl;
    cout << "■                                                            ■" << endl;
    cout << "■                       1. 로그인                            ■" << endl;
    cout << "■                       2. ID 찾기                           ■" << endl;
    cout << "■                       3. PW 찾기                           ■" << endl;
    cout << "■                       4. 회원가입                          ■" << endl;
    cout << "■                       5. 종료                              ■" << endl;
    cout << "■                                                            ■" << endl;
    cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■" << endl;
}
void searchId() {
    system("cls");
    cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■" << endl;
    cout << "■                                                            ■" << endl;
    cout << "■                       채팅 프로그램                        ■" << endl;
    cout << "■                        아이디 찾기                         ■" << endl;
    cout << "■                       >> 이름 입력                         ■" << endl;
    cout << "■                       >> 번호 입력                         ■" << endl;
    cout << "■                       >> 생년월일(8자리) 입력              ■" << endl;
    cout << "■                       >> 완료시 아이디 출력                ■" << endl;
    cout << "■                       >> 실패시 메인화면                   ■" << endl;
    cout << "■                                                            ■" << endl;
    cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■" << endl;
}
void searchPw() {
    system("cls");
    cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■" << endl;
    cout << "■                                                            ■" << endl;
    cout << "■                       채팅 프로그램                        ■" << endl;
    cout << "■                       비밀번호 찾기                        ■" << endl;
    cout << "■                       >> 아이디 입력                       ■" << endl;
    cout << "■                       >> 비밀번호 입력                     ■" << endl;
    cout << "■                       >> 생년월일(8자리) 입력              ■" << endl;
    cout << "■                       >> 전화번호 입력                     ■" << endl;
    cout << "■                       >> 완료시 메인화면                   ■" << endl;
    cout << "■                                                            ■" << endl;
    cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■" << endl;
}
void createUser() {
    system("cls");
    cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■" << endl;
    cout << "■                                                            ■" << endl;
    cout << "■                       채팅 프로그램                        ■" << endl;
    cout << "■                         회원가입                           ■" << endl;
    cout << "■                       >> 아이디 입력                       ■" << endl;
    cout << "■                       >> 비밀번호 입력                     ■" << endl;
    cout << "■                       >> 이름, 전화번호 입력               ■" << endl;
    cout << "■                       >> 생년월일(8자리) 입력              ■" << endl;
    cout << "■                       >> 완료시 메인화면                   ■" << endl;
    cout << "■                                                            ■" << endl;
    cout << "■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■■" << endl;
}

int chat_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            msg = buf;
            std::stringstream ss(msg);  // 문자열을 스트림화
            string user;
            ss >> user; // 스트림을 통해, 문자열을 공백 분리해 변수에 할당. 보낸 사람의 이름만 user에 저장됨.
            if (user != my_nick) cout << buf << endl; // 내가 보낸 게 아닐 경우에만 출력하도록.
        }
        else {
            cout << "Server Off" << endl;
            return -1;
        }
    }
}

class SQL
{
public:
    string id, pw, name, phone, birth;
    SQL()
    {
        try {
            driver = sql::mysql::get_mysql_driver_instance();
            con = driver->connect(server, username, password);
        }
        catch (sql::SQLException& e) {
            cout << "Could not connect to server. Error message: " << e.what() << endl;
            exit(1);
        }

        con->setSchema("project2");

        // DB 한글 저장을 위한 셋팅
        stmt = con->createStatement();
        stmt->execute("set names euckr");
        if (stmt) { delete stmt; stmt = nullptr; }
    }
    int login()
    {
        cout << ">>아이디 : ";
        cin >> id;
        cout << ">>비밀번호를 입력해주세요 : ";
        char ch = ' ';
        while (ch != 13) { // Enter 키를 누르면 입력 종료
            ch = _getch();
            if (ch == 13) break; // Enter 키를 누르면 입력 종료
            else if (ch == 8) { // Backspace 키인 경우
                if (!pw.empty()) { // 입력된 문자가 있으면
                    pw.pop_back(); // 마지막 문자를 삭제
                    cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤,
                    // 다시 커서 위치를 원래대로 이동시킴
                }
            }
            else {
                pw.push_back(ch);
                cout << '*'; // 별표로 대체하여 출력
            }
        }
        cout << endl;
        pstmt = con->prepareStatement("SELECT id, pw, name FROM user \
            WHERE id=?");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();

        if (result->next())
        { // 쿼리 결과가 있다면
            string db_id = result->getString(1); // 데이터베이스에서 가져온 ID
            string db_pw = result->getString(2); // 데이터베이스에서 가져온 PW

            if (db_id == id && db_pw == pw)
            {
                name = result->getString(3);
                cout << endl << "▶로그인 중입니다. 잠시만 기다려주세요." << endl << endl;

                cout << "▶" << name << "님 환영합니다." << endl;
                return 1;
            }
            else cout << "▶해당하는 정보가 없습니다." << endl;
        }
        else cout << "▶해당하는 정보가 없습니다." << endl;
        pw.clear();
    }
    void searchId()
    {
        cout << ">>이름 : ";
        cin >> name;
        cout << ">>전화번호 : ";
        cin >> phone;
        while (true)
        {
            cout << ">>생년월일 : ";
            cin >> birth;
            if (birth.length() != 8) {
                cout << "▶생년월일은 8자리로 입력해주세요." << endl;
                continue;
            }
            break;
        }

        string year = birth.substr(0, 4);
        string month = birth.substr(4, 2);
        string day = birth.substr(6, 2);
        string DATE = year + "-" + month + "-" + day;

        pstmt = con->prepareStatement("SELECT id, name, phone, birth FROM user WHERE phone=?");
        pstmt->setString(1, phone);
        result = pstmt->executeQuery();

        if (result->next())
        {
            string db_id = result->getString(1);
            string db_name = result->getString(2);
            string db_phone = result->getString(3);
            string db_birth = result->getString(4);

            if (db_name == name && db_phone == phone && db_birth == DATE)
            {
                cout << "▶" << name << "님의 아이디는 " << db_id << "입니다." << endl;
                Sleep(3000);
            }
            else
            {
                cout << "▶해당하는 정보가 없습니다.!" << endl;
                Sleep(500);
            }
        }
        else {
            cout << "▶해당하는 정보가 없습니다.?" << endl;
            Sleep(500);
        }
    }
    void searchPw()
    {
        cout << ">>아이디 : ";
        cin >> id;
        cout << ">>이름 : ";
        cin >> name;
        cout << ">>전화번호 : ";
        cin >> phone;
        while (true)
        {
            cout << ">>생년월일 : ";
            cin >> birth;
            if (birth.length() != 8) {
                cout << "▶생년월일은 8자리로 입력해주세요." << endl;
                continue;
            }
            break;
        }

        string year = birth.substr(0, 4);
        string month = birth.substr(4, 2);
        string day = birth.substr(6, 2);
        string DATE = year + "-" + month + "-" + day;

        pstmt = con->prepareStatement("SELECT id, name, phone, birth FROM user WHERE id=?");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();

        if (result->next())
        {
            string db_id = result->getString(1);
            string db_name = result->getString(2);
            string db_phone = result->getString(3);
            string db_birth = result->getString(4);

            if (db_id == id && db_name == name && db_phone == phone && db_birth == DATE)
            {
                while (1)
                {
                    string new_pw = "";
                    cout << ">>새로운 비밀번호를 입력해주세요 : ";
                    char ch = ' ';
                    while (ch != 13) { // Enter 키를 누르면 입력 종료
                        ch = _getch();
                        if (ch == 13) break; // Enter 키를 누르면 입력 종료
                        if (ch == 8) { // Backspace 키인 경우
                            if (!new_pw.empty()) { // 입력된 문자가 있으면
                                new_pw.pop_back(); // 마지막 문자를 삭제
                                cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                            }
                        }
                        else {
                            new_pw.push_back(ch);
                            cout << '*'; // 별표로 대체하여 출력
                        }
                    }


                    cout << endl;

                    string renew_pw = "";
                    cout << ">>다시 한번 입력해주세요. : ";
                    char rech = ' ';
                    while (rech != 13) { // Enter 키를 누르면 입력 종료
                        rech = _getch();
                        if (rech == 13) break; // Enter 키를 누르면 입력 종료
                        if (rech == 8) { // Backspace 키인 경우
                            if (!renew_pw.empty()) { // 입력된 문자가 있으면
                                renew_pw.pop_back(); // 마지막 문자를 삭제
                                cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                            }
                        }
                        else {
                            renew_pw.push_back(rech);
                            cout << '*'; // 별표로 대체하여 출력
                        }
                    }
                    cout << endl;

                    if (new_pw == renew_pw)
                    {
                        pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?");
                        pstmt->setString(1, new_pw);
                        pstmt->setString(2, id);
                        pstmt->executeQuery();
                        printf("▶새로운 비밀번호로 변경되었습니다.\n");
                        Sleep(500);
                        break;
                    }
                    else {
                        cout << "▶비밀번호가 다릅니다." << endl;
                        Sleep(500);
                    }
                }
            }
            else {
                cout << "▶해당하는 정보가 없습니다." << endl;
                Sleep(500);
            }
        }
        else {
            cout << "▶해당하는 정보가 없습니다." << endl;
            Sleep(500);
        }
    }
    void createUser()
    {
        while (1)
        {
            cout << ">>아이디 : ";
            cin >> id;
            pstmt = con->prepareStatement("SELECT id FROM user WHERE id=?");
            pstmt->setString(1, id);
            result = pstmt->executeQuery();

            if (result->next())
            {
                string db_id = result->getString(1);
                if (db_id == id) {
                    cout << "▶중복된 아이디가 있습니다." << endl;
                    continue;
                }
            }
            else {
                cout << "▶중복체크 완료." << endl;
                break;
            }
        }
        while (1)
        {
            cout << ">>비밀번호를 입력해주세요 : ";
            char ch = ' ';
            while (ch != 13) { // Enter 키를 누르면 입력 종료
                ch = _getch();
                if (ch == 13) break; // Enter 키를 누르면 입력 종료
                if (ch == 8) { // Backspace 키인 경우
                    if (!pw.empty()) { // 입력된 문자가 있으면
                        pw.pop_back(); // 마지막 문자를 삭제
                        cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                    }
                }
                else {
                    pw.push_back(ch);
                    cout << '*'; // 별표로 대체하여 출력
                }
            }
            cout << endl;
            string new_pw = "";
            cout << endl << ">>다시 한번 입력해주세요. : ";
            char rech = ' ';
            while (rech != 13) { // Enter 키를 누르면 입력 종료
                rech = _getch();
                if (rech == 13) break; // Enter 키를 누르면 입력 종료
                else if (rech == 8) { // Backspace 키인 경우
                    if (!new_pw.empty()) { // 입력된 문자가 있으면
                        new_pw.pop_back(); // 마지막 문자를 삭제
                        cout << "\b \b"; // 커서 위치를 왼쪽으로 이동시켜 공백을 출력한 뒤, 다시 커서 위치를 원래대로 이동시킴
                    }
                }
                else {
                    new_pw.push_back(rech);
                    cout << '*'; // 별표로 대체하여 출력
                }

            }
            cout << endl;

            if (pw == new_pw)
            {
                break;
            }
            else
            {
                cout << "▶비밀번호가 다릅니다." << endl;
                pw.clear();
            }
        }

        cout << ">>이름 : ";
        cin >> name;
        cout << ">>전화번호 : ";
        cin >> phone;
        while (true)
        {
            cout << ">>생년월일 : ";
            cin >> birth;
            if (birth.length() != 8) {
                cout << "▶생년월일은 8자리로 입력해주세요." << endl;
                continue;
            }
            break;
        }

        string year = birth.substr(0, 4);
        string month = birth.substr(4, 2);
        string day = birth.substr(6, 2);
        string DATE = year + "-" + month + "-" + day;

        pstmt = con->prepareStatement("INSERT INTO user(id,pw, name, phone, birth) VALUE(?,?, ?, ?, ?)");
        pstmt->setString(1, id);
        pstmt->setString(2, pw);
        pstmt->setString(3, name);
        pstmt->setString(4, phone);
        pstmt->setString(5, DATE);
        pstmt->execute();
        cout << "▶회원가입이 완료되었습니다." << endl;
        pw.clear();
        Sleep(500);
    }
    void beforeChat()
    {
        pstmt = con->prepareStatement("SELECT chatname, time, recv FROM chatting ORDER BY time DESC LIMIT 5");
        result = pstmt->executeQuery();

        string chatname[5];
        string time[5];
        string recv[5];
        string msg[5];
        int num = 0;
        while (result->next())
        {
            chatname[num] = result->getString(1);
            time[num] = result->getString(2);
            recv[num] = result->getString(3);
            num++;
        }
        for (int i = 4; i >= 0; i--) {
            msg[i] += "--------------------------------------------------";
            msg[i] += "\n▷보낸사람 : " + chatname[i] + " " + "▷보낸시간 : " + time[i] + "\n";
            msg[i] += "▷내용 : " + recv[i] + "\n";
            msg[i] += "--------------------------------------------------\n";
            cout << msg[i] << endl;
        }
    }
};

int main() {

    SQL sql;
    bool loginSuccess = false;
    
    while (!loginSuccess)
    {
        startMenu(); //시작 화면
        char startIn = 0;
        cout << "▶ ";
        cin >> startIn;
        switch (startIn)
        {
        case '1': //1. 로그인
            sql.login();
            if (sql.login() == 1) {
                loginSuccess = true;
                break;
            }
            continue;
        case '2': //2. 아이디 찾기
            searchId();
            sql.searchId();
            continue;

        case '3': //3. 비밀번호찾기
            searchPw();
            sql.searchPw();
            continue;

        case '4': //4. 회원가입
            createUser();
            sql.createUser();
            continue;
        case '0': //0. 시스템 종료
            return -1;
        default: //5 ~ 9. 재입력
            cout << "▶잘못된 입력입니다. 다시 입력해주세요." << endl;
            continue;
        }
    }
    WSADATA wsa;
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (!code) {
        cout << "사용할 닉네임 입력 >> ";
        cin >> my_nick;

        client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 

        // 연결할 서버 정보 설정 부분
        SOCKADDR_IN client_addr = {};
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(7777);
        InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);

        while (1) {
            if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) { // 위에 설정한 정보에 해당하는 server로 연결!
                cout << "Server Connect" << endl;
                send(client_sock, my_nick.c_str(), my_nick.length(), 0); // 연결에 성공하면 client 가 입력한 닉네임을 서버로 전송
                break;
            }
            cout << "Connecting..." << endl;
        }

        std::thread th2(chat_recv);

        while (1) {
            string text;
            std::getline(cin, text);
            const char* buffer = text.c_str(); // string형을 char* 타입으로 변환
            send(client_sock, buffer, strlen(buffer), 0);
        }
        th2.join();
        closesocket(client_sock);
    }

    WSACleanup();
    return 0;
 }