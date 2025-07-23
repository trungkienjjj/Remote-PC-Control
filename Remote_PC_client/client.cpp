#include "client.h"
#include "mailAPI.h"
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <gdiplus.h>
#include <opencv2/opencv.hpp>
#include <tlhelp32.h> 
#include <winsvc.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <direct.h>
#include <QDir>
#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "gdiplus.lib") 

#define PORT 8000
ULONG_PTR gdiplusToken;

// Constructor của lớp client
client::client(QWidget* parent) : QWidget(parent) {
    // Tạo nhãn hiển thị "STATUS"
    statusLabel = new QLabel("CLIENT STATUS", this);
    statusLabel->setGeometry(300, 0, 200, 50);
    QFont font = statusLabel->font();
    font.setPointSize(20);  // Đặt kích thước chữ lớn hơn
    statusLabel->setFont(font);
    //Nhãn INFO
    infoLabel = new QLabel("INFORM", this);
    infoLabel->setGeometry(1, 1, 200, 50);
    font.setPointSize(20);
    infoLabel->setFont(font);
    //Tạo thanh status
    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setGeometry(300, 50, 600, 400);
    font.setPointSize(14);
    textEdit->setFont(font);
    setFixedSize(900, 500);
    //Thanh info
    infoEdit = new QTextEdit(this);
    infoEdit->setReadOnly(true);
    infoEdit->setGeometry(0, 50, 300, 400);
    font.setPointSize(14);
    infoEdit->setFont(font);

    // Tạo ô nhập văn bản
    lineEdit = new QLineEdit(this);
    lineEdit->setPlaceholderText("Type server ip address ...");
	lineEdit->setGeometry(300, 450, 500, 50);
    font.setPointSize(14);
    lineEdit->setFont(font);
    // Tạo nút bấm
    button_2 = new QPushButton("Connect", this);
    button_2->setGeometry(800, 450, 100, 50);
    aboutus_button = new QPushButton("About Us", this);
    aboutus_button->setGeometry(0, 450, 100, 50);
    htu_button = new QPushButton("How to Use", this);
    htu_button->setGeometry(100, 450, 100, 50);
    listcomd_button = new QPushButton("List Command", this);
    listcomd_button->setGeometry(200, 450, 100, 50);
   

    // Thiết lập kết nối giữa nút bấm và slot onButtonClicked
    connect(button_2, &QPushButton::clicked, this, &client::onButtonClicked);
    connect(aboutus_button, &QPushButton::clicked, this, &client::About_Us);
    connect(htu_button, &QPushButton::clicked, this, &client::How_to_Use);
    connect(listcomd_button, &QPushButton::clicked, this, &client::List_Command);

    //setLayout(layout);
    setWindowTitle("Remote PC by GMAIL");
}

// Hàm xử lý khi nút bấm được nhấn
void client::onButtonClicked() {
    QString text = lineEdit->text();
    std::thread socketThread(&client::initializeSocket, this, text);
    socketThread.detach();
}
void client::About_Us() {
    QString aboutText =
        "<h3>Thông tin nhóm </h3>"
        "<p><b>Vũ Nguyễn Trung Hiếu</b></p>"
        "<p>MSSV: 23122028</p>"
        "<p><b>Châu Văn Minh Khoa</b></p>"
        "<p>MSSV: 23122035</p>"
        "<p><b>Nguyễn Trần Trung Kiên</b></p>"
        "<p>MSSV: 23122038</p>"
        "<p>Giáo viên hướng dẫn:"
        "<p><b>Đỗ Hoàng Cường</b></p>"

        "<p>Nhóm: 14</p>"
        "<p>Mô tả: Nhóm phát triển phần mềm giúp điều khiển máy tính từ xa thông qua gmail</p>";
    infoEdit->setText(aboutText);
}
void client::How_to_Use() {
    QString aboutText =
        "<h3>Cách sử dụng phần mềm </h3>"
        "<p>Khởi động Server và Client, sau đó dùng gmail để gửi lệnh yêu cầu điều khiển máy server</p>"
        "<p><b>Địa chỉ mail host</b></p>"
        "<p>khoa00927@gmail.com</p>"
        "<p><b>Header</b></p>"
        "<p>Lệnh yêu cầu - xem ở phần list command</p>"
        "<p><b>Body</b></p>"
        "<p>Tên file bạn muốn lưu lại hoặc tên dịch vụ, tiến trình bạn muốn thao tác</p>"
        "<p><b>Note: Hãy chạy server với vai trò admin bởi vì 1 số chức năng sẽ không hoạt động khi chạy bình thường</b></p>";

    infoEdit->setText(aboutText);
}
void client::List_Command() {
    QString aboutText =
        "<h3>Các lệnh yêu cầu</h3>"
        "<p>Đăng kí email sử dụng"
        "<p><b>Register</b></p>"
        "<p>Danh sách các tiến trình"
        "<p><b>List Process</b></p>"
        "<p>Danh sách các dịch vụ"
        "<p><b>List Service</b></p>"
        "<p>Chụp ảnh bằng camera"
        "<p><b>Capture Shot</b></p>"
        "<p>Chụp ảnh màn hình"
        "<p><b>Screen Shot</b></p>"
        "<p>Lấy địa chỉ IP của server"
        "<p><b>Server IP</b></p>"
        "<p>Chạy 1 dịch vụ"
        "<p><b>Run Service</b></p>"
        "<p>Dừng 1 dịch vụ"
        "<p><b>Stop Service</b></p>"
        "<p>Chạy 1 tiến trình"
        "<p><b>Run Process</b></p>"
        "<p>Dừng 1 tiến trình"
        "<p><b>Stop Process</b></p>"
        "<p>Lấy thông tin 1 file"
        "<p><b>Get File</b></p>"
        "<p>Xóa 1 file"
        "<p><b>Delete File</b></p>"
        "<p>Shutdown máy Server"
        "<p><b>Shutdown PC</b></p>"
        "<p>Ngắt kết nối"
        "<p><b>Shutdown Socket</b></p>"
        "<p>Lấy lịch sử toàn bộ các lệnh đã yêu cầu"
        "<p><b>My History</b></p>";

    infoEdit->setText(aboutText);
}

void client::appendStatus(const QString& message) {
    QString newText = message + "\n"; // Append thông báo mới
    textEdit->append(newText); // Cập nhật nội dung
}

int client ::initializeSocket(const QString& ipAddress) {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        appendStatus(QString("Initialize Winsock Fail. ERROR: %0").arg(WSAGetLastError()));
        return 1;
    }

    // Tạo socket
    SOCKET client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        appendStatus(QString("Can't create socket. ERROR: %0").arg(WSAGetLastError()));
        WSACleanup();
        return 1;
    }

    // Cấu hình địa chỉ server
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    InetPton(AF_INET, ipAddress.toStdWString().c_str(), &server_address.sin_addr);  // Địa chỉ IP của server

    // Kết nối tới server
    if (::connect(client_socket, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        appendStatus(QString("Connect to server fail. ERROR: %0").arg(WSAGetLastError()));
        closesocket(client_socket);
        WSACleanup();
        return 1;
    }
    else {
        appendStatus(QString("Connect to Server"));
    }
    std::string access_token = readJson("mailapi.json");
    // Gửi dữ liệu tới server
    send_to_server(client_socket, access_token, 1);
    // Nhận phản hồi từ server


    // Đóng kết nối
    closesocket(client_socket);
    WSACleanup();

    return 0;
}

// Hàm đọc danh sách user_mail từ file database
std::vector<std::string> GetRegisteredUsers(const std::string& filePath) {
    std::ifstream file(filePath);
    std::vector<std::string> registeredUsers;

    if (!file.is_open()) {
        return registeredUsers;
    }

    json data;
    file >> data;
    file.close();

    for (const auto& user : data["users"]) {
        registeredUsers.push_back(user["user_mail"]);
    }
    return registeredUsers;
}

json ReadJsonFile(const std::string& filePath) {
    std::ifstream file(filePath);
    json data;

    if (file.is_open()) {
        file >> data;
        file.close();
    }

    return data;
}
void WriteJsonFile(const std::string& filePath, const json& data) {
    std::ofstream file(filePath);
    if (file.is_open()) {
        file << data.dump(4); // Ghi file với định dạng đẹp (4 space)
        file.close();
    }
}
bool AddUser(const std::string& filePath, const std::string& newUser) {
    json data = ReadJsonFile(filePath);

    // Kiểm tra nếu user đã tồn tại
    for (const auto& user : data["users"]) {
        if (user["user_mail"] == newUser) {
            return false;
        }
    }

    // Thêm user mới
    json newUserJson = { {"user_mail", newUser}, {"commands", json::array()} };
    data["users"].push_back(newUserJson);

    // Ghi lại vào file
    WriteJsonFile(filePath, data);
    return true;
}
void AddCommandToUser(const std::string& filePath, const std::string& username, const std::string& command) {
    json data = ReadJsonFile(filePath);

    // Tìm user trong danh sách
    bool userFound = false;
    for (auto& user : data["users"]) {
        if (user["user_mail"] == username) {
            user["commands"].push_back(command); // Thêm command mới
            userFound = true;
            break;
        }
    }
    // Ghi lại file JSON
    WriteJsonFile(filePath, data);
}
bool IsUserRegistered(const std::string& filePath, const std::string& username) {
    json data = ReadJsonFile(filePath);

    for (const auto& user : data["users"]) {
        if (user["user_mail"] == username) {
            return true;
        }
    }

    return false;
}
std::vector<std::string> GetCommandsOfUser(const std::string& filePath, const std::string& username) {
    // Hàm lấy toàn bộ command của user
    json data = ReadJsonFile(filePath);
    std::vector<std::string> commands;

    if (!data.contains("users") || !data["users"].is_array()) {
        return commands;
    }

    for (const auto& user : data["users"]) {
        if (user.contains("user_mail") && user["user_mail"] == username) {
            if (user.contains("commands") && user["commands"].is_array()) {
                for (const auto& cmd : user["commands"]) {
                    commands.push_back(cmd.get<std::string>());
                }
            }
            break;
        }
    }

    return commands;
}

cv::Mat client::receiveImage(SOCKET client_socket) {
    // Nhận kích thước ảnh
    int imgSize;
    recv(client_socket, reinterpret_cast<char*>(&imgSize), sizeof(int), 0);
    // Nhận dữ liệu ảnh
    std::vector<uchar> buf(imgSize);
    recv(client_socket, reinterpret_cast<char*>(buf.data()), imgSize, 0);
    // Giải mã ảnh từ buffer
    cv::Mat img = cv::imdecode(buf, cv::IMREAD_COLOR);

    if (!img.empty()) {
        appendStatus(QString("Client has received the image from server"));
    }
    else {
        appendStatus(QString("ERROR: Can't decode image."));
    }
    return img;
}

std::vector<std::vector<std::string>>client:: ReceiveProcessList(SOCKET client_socket) {
    int received_number = 0;
    if (recv(client_socket, reinterpret_cast<char*>(&received_number), sizeof(received_number), 0) <= 0 || received_number <= 0) {
        // Xử lý lỗi
        appendStatus(QString("Error while recieve nums of process "));
        return {};
    }

    size_t maxProcessName = 0;
    size_t maxId = 0;
    std::vector<std::vector<std::string>> listProcess;
    appendStatus(QString("Receiving data from the server ..."));
    for (int i = 0; i < received_number; i++) {
        char Process_Name[4096];
        char Id[4096];
        int bytes_received_process = recv(client_socket, Process_Name, sizeof(Process_Name), 0);
        if (bytes_received_process <= 0) {
            // Xử lý lỗi khi nhận dữ liệu
            continue;
        }
        Process_Name[bytes_received_process] = '\0';
        int bytes_received_ID = recv(client_socket, Id, sizeof(Id), 0);
        if (bytes_received_ID <= 0) {
            // Xử lý lỗi khi nhận dữ liệu
            continue;
        }
        Id[bytes_received_ID] = '\0';

        std::vector<std::string> temp;
        temp.push_back(Process_Name);
        temp.push_back(Id);
        listProcess.push_back(temp);
        if (strlen(Process_Name) > maxProcessName) maxProcessName = strlen(Process_Name);
        if (strlen(Id) > maxId) maxId = strlen(Id);
    }
    return listProcess;
}

bool saveToTXT(const std::vector<std::vector<std::string>>& data, const std::string& filename) {
    std::ofstream file(filename);
    bool flag;
    if (file.is_open()) {
        // Tính chiều dài lớn nhất của mỗi cột
        std::vector<size_t> colWidths(data[0].size(), 0);
        for (const auto& row : data) {
            for (size_t col = 0; col < row.size(); ++col) {
                colWidths[col] = std::max(colWidths[col], row[col].length());
            }
        }

        // Ghi đường viền đầu bảng
        file << "+";
        for (const auto& width : colWidths) {
            file << std::string(width + 2, '-') << "+";
        }
        file << "\n";

        // Ghi dữ liệu với định dạng
        for (const auto& row : data) {
            file << "|";
            for (size_t col = 0; col < row.size(); ++col) {
                file << " " << std::setw(colWidths[col]) << std::left << row[col] << " |";
            }
            file << "\n";

            // Ghi đường viền ngăn cách giữa các hàng
            file << "+";
            for (const auto& width : colWidths) {
                file << std::string(width + 2, '-') << "+";
            }
            file << "\n";
        }

        file.close();
        flag = true;
    }
    else {
        flag = false;
    }
    return flag;
}

std::vector<std::vector<std::string>> client:: ReceiveServiceList(SOCKET client_socket) {
    DWORD received_number;
    recv(client_socket, reinterpret_cast<char*>(&received_number), sizeof(received_number), 0);

    size_t maxServiceNameLength = 0;
    size_t maxDisplayNameLength = 0;
    size_t maxStatus = 8;
    std::vector<std::vector<std::string>> services;
    appendStatus(QString("Receiving data from the server ..."));
    for (DWORD i = 0; i < received_number; i++) {
        char Service_name[8182];
        char Display_name[4096];
        char status[1024];
        std::vector<std::string> temp;
        int bytes_received_service = recv(client_socket, Service_name, sizeof(Service_name), 0);
        Service_name[bytes_received_service] = '\0';
        int bytes_received_name = recv(client_socket, Display_name, sizeof(Display_name), 0);
        Display_name[bytes_received_name] = '\0';
        int byte_status = recv(client_socket, status, sizeof(status), 0);
        status[byte_status] = '\0';


        temp.push_back(Service_name);
        temp.push_back(Display_name);
        temp.push_back(status);
        services.push_back(temp);

        if (strlen(Service_name) > maxServiceNameLength) maxServiceNameLength = strlen(Service_name);
        if (strlen(Display_name) > maxDisplayNameLength) maxDisplayNameLength = strlen(Display_name);
    }
    return services;
}

std::vector<std::string> client::receive_serverIP(SOCKET client_socket) {
    int received_number = 0;

    // Nhận số lượng IP từ server
    if (recv(client_socket, reinterpret_cast<char*>(&received_number), sizeof(received_number), 0) <= 0 || received_number <= 0) {
        appendStatus(QString("Error while receiving number of IPs"));
        return {};
    }

    // Chuyển từ định dạng mạng sang định dạng máy (nếu cần)
    received_number = ntohl(received_number);
    appendStatus(QString("Receiving data from the server ..."));

    std::vector<std::string> ListIP;
    for (int i = 0; i < received_number; i++) {
        char IP[128] = { 0 };  // Khởi tạo mảng để tránh dữ liệu cũ còn sót
        int bytes_received_IP = recv(client_socket, IP, sizeof(IP), 0); 
        IP[bytes_received_IP] = '\0';
        if (bytes_received_IP <= 0) {
            appendStatus(QString("Error while receiving IP"));
            break;
        }

        IP[bytes_received_IP] = '\0';  // Kết thúc chuỗi an toàn
        ListIP.push_back(IP);
    }
    return ListIP;
}

void client::send_to_server(SOCKET client_socket, std::string accessToken, int intervalSeconds) {
    std::unordered_set<std::string> previousEmailIds;
    nlohmann::json emailList = getEmailIds(accessToken);
    for (const auto& message : emailList["messages"]) {
        std::string emailid = message["id"];
        previousEmailIds.insert(emailid);
    }
    int flag = 0;
    while (true) {
        // Lấy danh sách email ID hiện tại
        if (flag == 0) {
            appendStatus(QString("Waiting mail ..."));
            flag = 1;
        }
        std::this_thread::sleep_for(std::chrono::seconds(intervalSeconds));
        nlohmann::json emailListAfter = getEmailIds(accessToken);

        if (!emailListAfter.is_null()) {
            // Lưu các email ID hiện tại vào tập hợp
            std::string emailIdAfter = emailListAfter["messages"][0]["id"];

            // Kiem  tra xem email này có trong danh sách trước đó không
            if (previousEmailIds.find(emailIdAfter) == previousEmailIds.end()) {
                // Nếu không có, nghĩa là đây là email mới
                appendStatus(QString("New email arrived! Message ID: " + QString::fromStdString(emailIdAfter)));
                previousEmailIds.insert(emailIdAfter);
                std::string receive = GetSenderEmail(accessToken, emailIdAfter);
                appendStatus(QString("Mail from: " + QString::fromStdString(receive)));
                std::string message = getEmailSubject(accessToken, emailIdAfter);
                appendStatus(QString("Mail order: " + QString::fromStdString(message)));
                std::string dataPath = "database.json";
                if (message != "Register" && (IsUserRegistered(dataPath, receive) == false)) {
                    appendStatus(QString("This Email is not registered"));

                    std::string body = "You have not registered for this service, Please register to use Remote PC";
                    std::string recipient = receive;
                    std::string subject = "Remote PC";

                    send_Text_email(accessToken, recipient, subject, body);
                    flag = 0;
                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    continue;
                }
                AddCommandToUser(dataPath, receive, message);

                std::string filesave = "C:/RemotePC";
                DWORD dwAttrib = GetFileAttributesA(filesave.c_str());
                if (dwAttrib == INVALID_FILE_ATTRIBUTES || !(dwAttrib & FILE_ATTRIBUTE_DIRECTORY)) {
                    CreateDirectoryA(filesave.c_str(), NULL);
                }

                if (message == "Register") {
                    std::string tmp;
                    if (AddUser(dataPath, receive)) {
                        appendStatus(QString::fromStdString("Register success"));
                        tmp = "You have successfully registered";
                    }
                    else {
                        appendStatus(QString::fromStdString("Register unsuccess"));
                        tmp = "Register Failed";
                    }

                    std::string body;
                    body = tmp;
                    std::string recipient = receive;
                    std::string subject = "Register Remote PC account";

                    if (send_Text_email(accessToken, recipient, subject, body)) appendStatus(QString("Email sent Successfully!"));
                    else appendStatus(QString("Email sent Failed"));
                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
                else if (message == "Capture Shot") {
                    send(client_socket, message.c_str(), message.size(), 0);
                    std::string filename = getEmailBody(accessToken, emailIdAfter);
                    filename.erase(std::remove(filename.begin(), filename.end(), '\n'), filename.end());
                    filename.erase(std::remove(filename.begin(), filename.end(), '\r'), filename.end());       
                    appendStatus(QString("The file name has been set: ") + QString::fromStdString(filename));
                    filename = filesave + "/" + filename + ".jpg";
                    cv::Mat image_capture = receiveImage(client_socket);
                    cv::imwrite(filename, image_capture);

                    std::string recipient = receive;
                    std::string subject = "ScreenShot send from Mail API";
                    std::string bodyText = "This is image:";
                    if (sendImagelWithGmailAPI(recipient, subject, bodyText, image_capture, accessToken)) {
                        appendStatus(QString("The email has been sent successfully!"));
                    }
                    else {
                        appendStatus(QString("Email sent Failed"));
                    }
                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }   
                else if (message == "List Process") {

                    send(client_socket, message.c_str(), message.size(), 0);
                    std::vector<std::vector<std::string>> ProcessList = ReceiveProcessList(client_socket);
                    
                    std::string filename = getEmailBody(accessToken, emailIdAfter);
                    filename.erase(std::remove(filename.begin(), filename.end(), '\n'), filename.end());
                    filename.erase(std::remove(filename.begin(), filename.end(), '\r'), filename.end());
                    appendStatus(QString("The file name has been set: ") + QString::fromStdString(filename));
                    filename = filesave + "/" + filename + ".txt";

                    if(saveToTXT(ProcessList, filename)) appendStatus("File saved successfully at: " + QString::fromStdString(filename));
                    else appendStatus("Error opening file for writing");
                    std::string recipient = receive;
                    std::string subject = "file TXT of List Process send from Mail API";
                    std::string bodyText = "This List of Process:";
                    sendTXT_Email(accessToken, recipient, subject, bodyText, filename);
                    appendStatus(QString("The email has been sent successfully!"));
                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
                else if (message == "List Service") {
                    send(client_socket, message.c_str(), message.size(), 0);    
                    std::vector<std::vector<std::string>> List_service = ReceiveServiceList(client_socket);

                    std::string filename = getEmailBody(accessToken, emailIdAfter);
                    filename.erase(std::remove(filename.begin(), filename.end(), '\n'), filename.end());
                    filename.erase(std::remove(filename.begin(), filename.end(), '\r'), filename.end());
                    appendStatus(QString("The file name has been set: ") + QString::fromStdString(filename));
                    filename = filesave + "/" + filename + ".txt";
                    
                    if (saveToTXT(List_service, filename)) appendStatus("File saved successfully at: " + QString::fromStdString(filename));
                    else appendStatus("Error opening file for writing");
                    std::string recipient = receive;
                    std::string subject = "file TXT of List Service send from Mail API";
                    std::string bodyText = "This List of Service:";
                    sendTXT_Email(accessToken, recipient, subject, bodyText, filename);
                    appendStatus(QString("The email has been sent successfully!"));
                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
                else if (message == "Screen Shot") {
                    send(client_socket, message.c_str(), message.size(), 0);
                    std::string filename = getEmailBody(accessToken, emailIdAfter);
                    filename.erase(std::remove(filename.begin(), filename.end(), '\n'), filename.end());
                    filename.erase(std::remove(filename.begin(), filename.end(), '\r'), filename.end());
                    appendStatus(QString("The file name has been set: ") + QString::fromStdString(filename));
                    filename = filesave + "/" + filename + ".jpg";

                    cv::Mat image_ScreenShot = receiveImage(client_socket);
                    cv::imwrite(filename, image_ScreenShot);
                    std::string recipient = receive;
                    std::string subject = "CaptureShot send from Mail API";
                    std::string bodyText = "This is image:";
                    if (sendImagelWithGmailAPI(recipient, subject, bodyText, image_ScreenShot, accessToken)) {
                        appendStatus(QString("The email has been sent successfully!"));
                    }
                    else {
                        appendStatus(QString("The email has been sent failed"));
                    }
                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
                else if (message == "Server IP") {

                    send(client_socket, message.c_str(), message.size(), 0);
                    std::vector<std::string> ListIP = receive_serverIP(client_socket);
                    std::string body;
                    for (int i = 0; i < ListIP.size(); i++) {
                        if (i == 0) body = body + "Private IP: " + ListIP[0] + "\n";
                        else if (i == 1) body = body + "Public IP: " + ListIP[1] + "\n";
                        else body = body + "IPv4 address: " + ListIP[2];
                    }

                    std::string recipient = receive;
                    std::string subject = "Server IP";
                   
                    if(send_Text_email(accessToken, recipient, subject, body)) appendStatus(QString("Email sent Successfully!"));
                    else appendStatus(QString("Email sent Failed"));
                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
                else if (message == "Stop Service") {
                    send(client_socket, message.c_str(), message.size(), 0);
                    std::string nameService;

                    nameService = getEmailBody(accessToken, emailIdAfter);
                    nameService.erase(std::remove(nameService.begin(), nameService.end(), '\n'), nameService.end());
                    nameService.erase(std::remove(nameService.begin(), nameService.end(), '\r'), nameService.end());
                    appendStatus(QString("Stoping Service: ") + QString::fromStdString(nameService));

                    send(client_socket, nameService.c_str(), nameService.size(), 0);
                    char result[1024];
                    int byte_recv = recv(client_socket, result, sizeof(result), 0);
                    result[byte_recv] = '\0';
                    appendStatus(QString("Message from server: ") + QString(result));
                    std::string recipient = receive;
                    std::string subject = "Stop Service";
                    std::string bodyText = result;
                    send_Text_email(accessToken, recipient, subject, bodyText);

                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
                else if (message == "Run Service") {
                    send(client_socket, message.c_str(), message.size(), 0);
                    std::string nameService;

                    nameService = getEmailBody(accessToken, emailIdAfter);
                    nameService.erase(std::remove(nameService.begin(), nameService.end(), '\n'), nameService.end());
                    nameService.erase(std::remove(nameService.begin(), nameService.end(), '\r'), nameService.end());
                    appendStatus(QString("Running Service: ") + QString::fromStdString(nameService));
     
                    send(client_socket, nameService.c_str(), nameService.size(), 0);
                    char result[1024];
                    int byte_recv = recv(client_socket, result, sizeof(result), 0);
                    result[byte_recv] = '\0';
                    appendStatus(QString("Message from server: ") + QString(result));
                    std::string recipient = receive;
                    std::string subject = "Run Service";
                    std::string bodyText = result;
                    send_Text_email(accessToken, recipient, subject, bodyText);

                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
                else if (message == "Stop Process") {
                    send(client_socket, message.c_str(), message.size(), 0);
                    std::string process;
                    process = getEmailBody(accessToken, emailIdAfter);
                    process.erase(std::remove(process.begin(), process.end(), '\n'), process.end());
                    process.erase(std::remove(process.begin(), process.end(), '\r'), process.end());
                    appendStatus(QString("Stoping Process of ID: ") + QString::fromStdString(process));
                    DWORD Id = static_cast<DWORD>(std::stoul(process));

                    send(client_socket, reinterpret_cast<char*>(&Id), sizeof(Id), 0);
                    char result[1024];
                    int byte_result = recv(client_socket, result, sizeof(result), 0);
                    result[byte_result] = '\0';
                    appendStatus(QString("Message from server: ") + QString(result));
                    std::string recipient = receive;
                    std::string subject = "Stop Process";
                    std::string bodyText = result;
                    send_Text_email(accessToken, recipient, subject, bodyText);

                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
                else if (message == "Run Process") {
                    send(client_socket, message.c_str(), message.size(), 0);
                    std::string processName;
                    processName = getEmailBody(accessToken, emailIdAfter);
                    processName.erase(std::remove(processName.begin(), processName.end(), '\n'), processName.end());
                    processName.erase(std::remove(processName.begin(), processName.end(), '\r'), processName.end());
                    appendStatus(QString("Running Process: ") + QString::fromStdString(processName));

                    send(client_socket, processName.c_str(), processName.size(), 0);
                    char result[1024];
                    int byte_recv = recv(client_socket, result, sizeof(result), 0);
                    result[byte_recv] = '\0';
                    appendStatus(QString("Message from server: ") + QString(result));
                    std::string recipient = receive;
                    std::string subject = "Run Process";
                    std::string bodyText = result;
                    send_Text_email(accessToken, recipient, subject, bodyText);

                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
                else if (message == "Get File") {
                    send(client_socket, message.c_str(), message.size(), 0);

                    std::string filePath;
                    filePath = getEmailBody(accessToken, emailIdAfter);
                    filePath.erase(std::remove(filePath.begin(), filePath.end(), '\n'), filePath.end());
                    filePath.erase(std::remove(filePath.begin(), filePath.end(), '\r'), filePath.end());
                    appendStatus(QString("Getting File: ") + QString::fromStdString(filePath));
                    send(client_socket, filePath.c_str(), filePath.size(), 0);

                    // Nhận kích thước file từ server
                    uint64_t fileSize;
                    int received = recv(client_socket, reinterpret_cast<char*>(&fileSize), sizeof(fileSize), 0);
                    if (received <= 0) {
                        appendStatus(QString("Can't get size of file from server."));
                        return;
                    }

                    // Tạo một bộ đệm để chứa dữ liệu file
                    std::vector<char> buffer(fileSize);
                    received = recv(client_socket, buffer.data(), fileSize, 0);
                    if (received <= 0) {
                        appendStatus(QString("Can't get data from server."));
                        return;
                    }

                    // Lưu file
                    std::string savedFilePath = filesave + "/" + filePath.substr(filePath.find_last_of("/\\") + 1); // Lấy tên file gốc
                    std::ofstream outFile(savedFilePath, std::ios::binary);
                    outFile.write(buffer.data(), fileSize);
                    outFile.close();

                    appendStatus(QString("file has been retrieved and saved at ") + QString::fromStdString(savedFilePath));

                    // Gửi file đã lưu qua email
                    std::string recipient = receive;
                    std::string subject = "Get File from server";
                    std::string bodyText = "File you get from server.";

                    // Đảm bảo rằng accessToken hợp lệ trước khi gọi hàm gửi email
                    if (accessToken.empty()) {
                        appendStatus(QString("The access token is invalid. Please ensure you have granted access."));
                        return;
                    }

                    if (sendFileWithGmailAPI(accessToken, recipient, subject, bodyText, savedFilePath)) {
                        appendStatus(QString("Sent email successfully."));
                    }
                    else {
                        appendStatus(QString("Sent email Failed."));
                    }
                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
                else if (message == "Delete File") {
                    send(client_socket, message.c_str(), message.size(), 0);
                    std::string filePath;
                    filePath = getEmailBody(accessToken, emailIdAfter);
                    filePath.erase(std::remove(filePath.begin(), filePath.end(), '\n'), filePath.end());
                    filePath.erase(std::remove(filePath.begin(), filePath.end(), '\r'), filePath.end());
                    appendStatus(QString("Delete File: ") + QString::fromStdString(filePath));

                    // Gửi kích thước filePath
                    uint32_t filePathSize = static_cast<uint32_t>(filePath.size());
                    send(client_socket, reinterpret_cast<char*>(&filePathSize), sizeof(filePathSize), 0);

                    // Gửi filePath
                    send(client_socket, filePath.c_str(), filePathSize, 0);

                    char result[1024];
                    int byte_recv = recv(client_socket, result, sizeof(result), 0);
                    result[byte_recv] = '\0';
                    appendStatus(QString("Message from server: ") + QString(result));

                    std::string recipient = receive;
                    std::string subject = "Delete File";
                    std::string bodyText = result;
                    send_Text_email(accessToken, recipient, subject, bodyText);

                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
                else if (message == "Shutdown PC") {
                    send(client_socket, message.c_str(), message.size(), 0);
                    
                    std::string recipient = receive;
                    std::string subject = "Shutdown PC";
                    std::string bodyText = "Shutdown PC successfully";
                    send_Text_email(accessToken, recipient, subject, bodyText);
                    appendStatus(QString("Shutdown PC done!"));

                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
                else if (message == "Shutdown Socket") {
                    send(client_socket, message.c_str(), message.size(), 0);
                    appendStatus("Disconect to Server");
                    break;
                }
                else if (message == "My History") {
                    std::vector<std::string> commands = GetCommandsOfUser(dataPath, receive);
                    std::string body = "Your history request:\n";
                    for (const auto& cmd : commands) {
                        body = body + cmd + "\n";
                    }
                    std::string recipient = receive;
                    std::string subject = "Your Command in Remote PC";

                    if (send_Text_email(accessToken, recipient, subject, body)) appendStatus(QString("Email sent Successfully!"));
                    else appendStatus(QString("Email sent Failed"));

                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
                else {
                    appendStatus("No function like this !");
                    std::string body;
                    body = "Your request doesn't match any command.\n";
                    body = body + "Our Service are:\n";
                    body = body + "Register\n";
                    body = body + "Capture Shot\n";
                    body = body + "List Process\n";
                    body = body + "List Service\n";
                    body = body + "Screen Shot\n";
                    body = body + "Server IP\n";
                    body = body + "Stop Service\n";
                    body = body + "Run Service\n";
                    body = body + "Stop Process\n";
                    body = body + "Run Process\n";
                    body = body + "Get File\n";
                    body = body + "Delete File\n";
                    body = body + "Shutdown Socket\n";
                    body = body + "My History\n";

                    std::string recipient = receive;
                    std::string subject = "Request Failed";

                    if (send_Text_email(accessToken, recipient, subject, body)) appendStatus(QString("Email sent Successfully!"));
                    else appendStatus(QString("Email sent Failed"));
                    emailList = getEmailIds(accessToken);
                    for (const auto& message : emailList["messages"]) {
                        std::string emailid = message["id"];
                        previousEmailIds.insert(emailid);
                    }
                    flag = 0;
                }
            }
            
        }
    }
}

