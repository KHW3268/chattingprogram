#pragma comment(lib, "ws2_32.lib")

#include <WinSock2.h> //Winsock ������� include. WSADATA �������.
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

sql::mysql::MySQL_Driver* driver; // ���� �������� �ʾƵ� Connector/C++�� �ڵ����� ������ �� 
sql::Connection* con;
sql::PreparedStatement* pstmt;
sql::ResultSet* result;
sql::Statement* stmt;

SOCKET client_sock;
string my_nick;



void startMenu() {
    system("cls");
    cout << "���������������������������������" << endl;
    cout << "��                                                            ��" << endl;
    cout << "��                       ä�� ���α׷�                        ��" << endl;
    cout << "��                                                            ��" << endl;
    cout << "��                       1. �α���                            ��" << endl;
    cout << "��                       2. ID ã��                           ��" << endl;
    cout << "��                       3. PW ã��                           ��" << endl;
    cout << "��                       4. ȸ������                          ��" << endl;
    cout << "��                       5. ����                              ��" << endl;
    cout << "��                                                            ��" << endl;
    cout << "���������������������������������" << endl;
}
void searchId() {
    system("cls");
    cout << "���������������������������������" << endl;
    cout << "��                                                            ��" << endl;
    cout << "��                       ä�� ���α׷�                        ��" << endl;
    cout << "��                        ���̵� ã��                         ��" << endl;
    cout << "��                       >> �̸� �Է�                         ��" << endl;
    cout << "��                       >> ��ȣ �Է�                         ��" << endl;
    cout << "��                       >> �������(8�ڸ�) �Է�              ��" << endl;
    cout << "��                       >> �Ϸ�� ���̵� ���                ��" << endl;
    cout << "��                       >> ���н� ����ȭ��                   ��" << endl;
    cout << "��                                                            ��" << endl;
    cout << "���������������������������������" << endl;
}
void searchPw() {
    system("cls");
    cout << "���������������������������������" << endl;
    cout << "��                                                            ��" << endl;
    cout << "��                       ä�� ���α׷�                        ��" << endl;
    cout << "��                       ��й�ȣ ã��                        ��" << endl;
    cout << "��                       >> ���̵� �Է�                       ��" << endl;
    cout << "��                       >> ��й�ȣ �Է�                     ��" << endl;
    cout << "��                       >> �������(8�ڸ�) �Է�              ��" << endl;
    cout << "��                       >> ��ȭ��ȣ �Է�                     ��" << endl;
    cout << "��                       >> �Ϸ�� ����ȭ��                   ��" << endl;
    cout << "��                                                            ��" << endl;
    cout << "���������������������������������" << endl;
}
void createUser() {
    system("cls");
    cout << "���������������������������������" << endl;
    cout << "��                                                            ��" << endl;
    cout << "��                       ä�� ���α׷�                        ��" << endl;
    cout << "��                         ȸ������                           ��" << endl;
    cout << "��                       >> ���̵� �Է�                       ��" << endl;
    cout << "��                       >> ��й�ȣ �Է�                     ��" << endl;
    cout << "��                       >> �̸�, ��ȭ��ȣ �Է�               ��" << endl;
    cout << "��                       >> �������(8�ڸ�) �Է�              ��" << endl;
    cout << "��                       >> �Ϸ�� ����ȭ��                   ��" << endl;
    cout << "��                                                            ��" << endl;
    cout << "���������������������������������" << endl;
}

int chat_recv() {
    char buf[MAX_SIZE] = { };
    string msg;

    while (1) {
        ZeroMemory(&buf, MAX_SIZE);
        if (recv(client_sock, buf, MAX_SIZE, 0) > 0) {
            msg = buf;
            std::stringstream ss(msg);  // ���ڿ��� ��Ʈ��ȭ
            string user;
            ss >> user; // ��Ʈ���� ����, ���ڿ��� ���� �и��� ������ �Ҵ�. ���� ����� �̸��� user�� �����.
            if (user != my_nick) cout << buf << endl; // ���� ���� �� �ƴ� ��쿡�� ����ϵ���.
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

        // DB �ѱ� ������ ���� ����
        stmt = con->createStatement();
        stmt->execute("set names euckr");
        if (stmt) { delete stmt; stmt = nullptr; }
    }
    int login()
    {
        cout << ">>���̵� : ";
        cin >> id;
        cout << ">>��й�ȣ�� �Է����ּ��� : ";
        char ch = ' ';
        while (ch != 13) { // Enter Ű�� ������ �Է� ����
            ch = _getch();
            if (ch == 13) break; // Enter Ű�� ������ �Է� ����
            else if (ch == 8) { // Backspace Ű�� ���
                if (!pw.empty()) { // �Էµ� ���ڰ� ������
                    pw.pop_back(); // ������ ���ڸ� ����
                    cout << "\b \b"; // Ŀ�� ��ġ�� �������� �̵����� ������ ����� ��,
                    // �ٽ� Ŀ�� ��ġ�� ������� �̵���Ŵ
                }
            }
            else {
                pw.push_back(ch);
                cout << '*'; // ��ǥ�� ��ü�Ͽ� ���
            }
        }
        cout << endl;
        pstmt = con->prepareStatement("SELECT id, pw, name FROM user \
            WHERE id=?");
        pstmt->setString(1, id);
        result = pstmt->executeQuery();

        if (result->next())
        { // ���� ����� �ִٸ�
            string db_id = result->getString(1); // �����ͺ��̽����� ������ ID
            string db_pw = result->getString(2); // �����ͺ��̽����� ������ PW

            if (db_id == id && db_pw == pw)
            {
                name = result->getString(3);
                cout << endl << "���α��� ���Դϴ�. ��ø� ��ٷ��ּ���." << endl << endl;

                cout << "��" << name << "�� ȯ���մϴ�." << endl;
                return 1;
            }
            else cout << "���ش��ϴ� ������ �����ϴ�." << endl;
        }
        else cout << "���ش��ϴ� ������ �����ϴ�." << endl;
        pw.clear();
    }
    void searchId()
    {
        cout << ">>�̸� : ";
        cin >> name;
        cout << ">>��ȭ��ȣ : ";
        cin >> phone;
        while (true)
        {
            cout << ">>������� : ";
            cin >> birth;
            if (birth.length() != 8) {
                cout << "����������� 8�ڸ��� �Է����ּ���." << endl;
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
                cout << "��" << name << "���� ���̵�� " << db_id << "�Դϴ�." << endl;
                Sleep(3000);
            }
            else
            {
                cout << "���ش��ϴ� ������ �����ϴ�.!" << endl;
                Sleep(500);
            }
        }
        else {
            cout << "���ش��ϴ� ������ �����ϴ�.?" << endl;
            Sleep(500);
        }
    }
    void searchPw()
    {
        cout << ">>���̵� : ";
        cin >> id;
        cout << ">>�̸� : ";
        cin >> name;
        cout << ">>��ȭ��ȣ : ";
        cin >> phone;
        while (true)
        {
            cout << ">>������� : ";
            cin >> birth;
            if (birth.length() != 8) {
                cout << "����������� 8�ڸ��� �Է����ּ���." << endl;
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
                    cout << ">>���ο� ��й�ȣ�� �Է����ּ��� : ";
                    char ch = ' ';
                    while (ch != 13) { // Enter Ű�� ������ �Է� ����
                        ch = _getch();
                        if (ch == 13) break; // Enter Ű�� ������ �Է� ����
                        if (ch == 8) { // Backspace Ű�� ���
                            if (!new_pw.empty()) { // �Էµ� ���ڰ� ������
                                new_pw.pop_back(); // ������ ���ڸ� ����
                                cout << "\b \b"; // Ŀ�� ��ġ�� �������� �̵����� ������ ����� ��, �ٽ� Ŀ�� ��ġ�� ������� �̵���Ŵ
                            }
                        }
                        else {
                            new_pw.push_back(ch);
                            cout << '*'; // ��ǥ�� ��ü�Ͽ� ���
                        }
                    }


                    cout << endl;

                    string renew_pw = "";
                    cout << ">>�ٽ� �ѹ� �Է����ּ���. : ";
                    char rech = ' ';
                    while (rech != 13) { // Enter Ű�� ������ �Է� ����
                        rech = _getch();
                        if (rech == 13) break; // Enter Ű�� ������ �Է� ����
                        if (rech == 8) { // Backspace Ű�� ���
                            if (!renew_pw.empty()) { // �Էµ� ���ڰ� ������
                                renew_pw.pop_back(); // ������ ���ڸ� ����
                                cout << "\b \b"; // Ŀ�� ��ġ�� �������� �̵����� ������ ����� ��, �ٽ� Ŀ�� ��ġ�� ������� �̵���Ŵ
                            }
                        }
                        else {
                            renew_pw.push_back(rech);
                            cout << '*'; // ��ǥ�� ��ü�Ͽ� ���
                        }
                    }
                    cout << endl;

                    if (new_pw == renew_pw)
                    {
                        pstmt = con->prepareStatement("UPDATE user SET pw = ? WHERE id = ?");
                        pstmt->setString(1, new_pw);
                        pstmt->setString(2, id);
                        pstmt->executeQuery();
                        printf("�����ο� ��й�ȣ�� ����Ǿ����ϴ�.\n");
                        Sleep(500);
                        break;
                    }
                    else {
                        cout << "����й�ȣ�� �ٸ��ϴ�." << endl;
                        Sleep(500);
                    }
                }
            }
            else {
                cout << "���ش��ϴ� ������ �����ϴ�." << endl;
                Sleep(500);
            }
        }
        else {
            cout << "���ش��ϴ� ������ �����ϴ�." << endl;
            Sleep(500);
        }
    }
    void createUser()
    {
        while (1)
        {
            cout << ">>���̵� : ";
            cin >> id;
            pstmt = con->prepareStatement("SELECT id FROM user WHERE id=?");
            pstmt->setString(1, id);
            result = pstmt->executeQuery();

            if (result->next())
            {
                string db_id = result->getString(1);
                if (db_id == id) {
                    cout << "���ߺ��� ���̵� �ֽ��ϴ�." << endl;
                    continue;
                }
            }
            else {
                cout << "���ߺ�üũ �Ϸ�." << endl;
                break;
            }
        }
        while (1)
        {
            cout << ">>��й�ȣ�� �Է����ּ��� : ";
            char ch = ' ';
            while (ch != 13) { // Enter Ű�� ������ �Է� ����
                ch = _getch();
                if (ch == 13) break; // Enter Ű�� ������ �Է� ����
                if (ch == 8) { // Backspace Ű�� ���
                    if (!pw.empty()) { // �Էµ� ���ڰ� ������
                        pw.pop_back(); // ������ ���ڸ� ����
                        cout << "\b \b"; // Ŀ�� ��ġ�� �������� �̵����� ������ ����� ��, �ٽ� Ŀ�� ��ġ�� ������� �̵���Ŵ
                    }
                }
                else {
                    pw.push_back(ch);
                    cout << '*'; // ��ǥ�� ��ü�Ͽ� ���
                }
            }
            cout << endl;
            string new_pw = "";
            cout << endl << ">>�ٽ� �ѹ� �Է����ּ���. : ";
            char rech = ' ';
            while (rech != 13) { // Enter Ű�� ������ �Է� ����
                rech = _getch();
                if (rech == 13) break; // Enter Ű�� ������ �Է� ����
                else if (rech == 8) { // Backspace Ű�� ���
                    if (!new_pw.empty()) { // �Էµ� ���ڰ� ������
                        new_pw.pop_back(); // ������ ���ڸ� ����
                        cout << "\b \b"; // Ŀ�� ��ġ�� �������� �̵����� ������ ����� ��, �ٽ� Ŀ�� ��ġ�� ������� �̵���Ŵ
                    }
                }
                else {
                    new_pw.push_back(rech);
                    cout << '*'; // ��ǥ�� ��ü�Ͽ� ���
                }

            }
            cout << endl;

            if (pw == new_pw)
            {
                break;
            }
            else
            {
                cout << "����й�ȣ�� �ٸ��ϴ�." << endl;
                pw.clear();
            }
        }

        cout << ">>�̸� : ";
        cin >> name;
        cout << ">>��ȭ��ȣ : ";
        cin >> phone;
        while (true)
        {
            cout << ">>������� : ";
            cin >> birth;
            if (birth.length() != 8) {
                cout << "����������� 8�ڸ��� �Է����ּ���." << endl;
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
        cout << "��ȸ�������� �Ϸ�Ǿ����ϴ�." << endl;
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
            msg[i] += "\n��������� : " + chatname[i] + " " + "�������ð� : " + time[i] + "\n";
            msg[i] += "������ : " + recv[i] + "\n";
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
        startMenu(); //���� ȭ��
        char startIn = 0;
        cout << "�� ";
        cin >> startIn;
        switch (startIn)
        {
        case '1': //1. �α���
            sql.login();
            if (sql.login() == 1) {
                loginSuccess = true;
                break;
            }
            continue;
        case '2': //2. ���̵� ã��
            searchId();
            sql.searchId();
            continue;

        case '3': //3. ��й�ȣã��
            searchPw();
            sql.searchPw();
            continue;

        case '4': //4. ȸ������
            createUser();
            sql.createUser();
            continue;
        case '0': //0. �ý��� ����
            return -1;
        default: //5 ~ 9. ���Է�
            cout << "���߸��� �Է��Դϴ�. �ٽ� �Է����ּ���." << endl;
            continue;
        }
    }
    WSADATA wsa;
    int code = WSAStartup(MAKEWORD(2, 2), &wsa);
    if (!code) {
        cout << "����� �г��� �Է� >> ";
        cin >> my_nick;

        client_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); // 

        // ������ ���� ���� ���� �κ�
        SOCKADDR_IN client_addr = {};
        client_addr.sin_family = AF_INET;
        client_addr.sin_port = htons(7777);
        InetPton(AF_INET, TEXT("127.0.0.1"), &client_addr.sin_addr);

        while (1) {
            if (!connect(client_sock, (SOCKADDR*)&client_addr, sizeof(client_addr))) { // ���� ������ ������ �ش��ϴ� server�� ����!
                cout << "Server Connect" << endl;
                send(client_sock, my_nick.c_str(), my_nick.length(), 0); // ���ῡ �����ϸ� client �� �Է��� �г����� ������ ����
                break;
            }
            cout << "Connecting..." << endl;
        }

        std::thread th2(chat_recv);

        while (1) {
            string text;
            std::getline(cin, text);
            const char* buffer = text.c_str(); // string���� char* Ÿ������ ��ȯ
            send(client_sock, buffer, strlen(buffer), 0);
        }
        th2.join();
        closesocket(client_sock);
    }

    WSACleanup();
    return 0;
 }