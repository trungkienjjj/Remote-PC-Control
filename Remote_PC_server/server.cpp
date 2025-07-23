#include "server.h"
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <gdiplus.h>
#include <opencv2/opencv.hpp>
#include <tlhelp32.h> 
#include <winsvc.h>
#include <ws2tcpip.h>
#include <thread>
#include <fstream>
#pragma comment(lib, "ws2_32.lib") 
#pragma comment(lib, "gdiplus.lib") 

#define PORT 8000
ULONG_PTR gdiplusToken;

// Constructor của lớp server
server::server(QWidget* parent) : QWidget(parent) {
    // Tạo layout chính
    QVBoxLayout* layout = new QVBoxLayout(this);

    // Tạo nút bấm
    button = new QPushButton("Start Server", this);
    button->setGeometry(800, 0, 100, 50);
    aboutus_button = new QPushButton("About Us", this);
    aboutus_button->setGeometry(0, 450, 100, 50);
    htu_button = new QPushButton("How to Use", this);
    htu_button->setGeometry(100, 450, 100, 50);
    listcomd_button = new QPushButton("List Command", this);
    listcomd_button->setGeometry(200, 450, 100, 50);
    // Tạo nhãn hiển thị "STATUS"
    statusLabel = new QLabel("SERVER STATUS", this);
    statusLabel->setGeometry(300, 1, 200, 50);
    QFont font = statusLabel->font();
    font.setPointSize(20);
    statusLabel->setFont(font);
    //Nhãn INFO
    infoLabel = new QLabel("INFORM", this);
    infoLabel->setGeometry(1, 1, 200, 50);
    font.setPointSize(20);
    infoLabel->setFont(font);

    //Tạo thanh status
    textEdit = new QTextEdit(this);
    textEdit->setReadOnly(true);
    textEdit->setGeometry(300, 50, 600, 450);
    font.setPointSize(14);
    textEdit->setFont(font);
    //Thanh info
    infoEdit = new QTextEdit(this);
    infoEdit->setReadOnly(true);
    infoEdit->setGeometry(0, 50, 300, 400);
    font.setPointSize(14);
    infoEdit->setFont(font);

    // Thiết lập kết nối giữa nút bấm và slot startServerButton
    connect(button, &QPushButton::clicked, this, &server::startServerButton);
    connect(aboutus_button, &QPushButton::clicked, this, &server::About_Us);
    connect(htu_button, &QPushButton::clicked, this, &server::How_to_Use);
    connect(listcomd_button, &QPushButton::clicked, this, &server::List_Command);

    // Thiết lập kích thước cố định cho cửa sổ
    setFixedSize(900, 500);
    setWindowTitle("Remote PC by GMAIL");
}

// Hàm xử lý khi nút bấm được nhấn
void server::startServerButton() {
    std::thread socketThread(&server::initializeSocket, this);
    socketThread.detach();
}
void server::About_Us() {
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
void server::How_to_Use() {
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
void server::List_Command() {
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

void server::appendStatus(const QString& message) {
    QString newText = message + "\n"; // Append thông báo mới
    textEdit->append(newText); // Cập nhật nội dung
}
int server::initializeSocket() {
    WSADATA wsa;
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        appendStatus(QString("Khởi tạo Winsock thất bại. Mã lỗi: %1").arg(WSAGetLastError()));
        return 1;
    }

    //Tạo socket
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == INVALID_SOCKET) {
        appendStatus(QString("Cant initialize Soccket. ERROR: %1").arg(WSAGetLastError()));
        WSACleanup();
        return 1;
    }

    // Lấy tên máy chủ
    char hostName[256];
    gethostname(hostName, sizeof(hostName));

    struct addrinfo hints, * result;
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_protocol = IPPROTO_TCP;

    int iResult = getaddrinfo(hostName, NULL, &hints, &result);
    if (iResult != 0) {
        appendStatus(QString("Erorr getaddrinfo: %1").arg(WSAGetLastError()));
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    // Lấy địa chỉ IP từ kết quả của getaddrinfo
    struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)result->ai_addr;
    char ipStr[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipStr, sizeof(ipStr));

    appendStatus(QString("Server is listening on IP: %1, Port: %2").arg(ipStr).arg(8000));

    //Cấu hình địa chỉ server
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    //Liên kết socket với địa chỉ IP và cổng
    if (bind(server_socket, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        appendStatus(QString("Connect fail. ERROR: %1").arg(WSAGetLastError()));
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    //Lắng nghe kết nối từ client
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        appendStatus(QString("Chap nhan ket noi that bai: Ma loi ").arg(WSAGetLastError()));
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }

    //Chấp nhận kết nối từ client
    sockaddr_in client_address;
    int client_size = sizeof(client_address);
    SOCKET client_socket = accept(server_socket, (sockaddr*)&client_address, &client_size);
    if (client_socket == INVALID_SOCKET) {
        appendStatus(QString("Connect fail. ERROR: %1").arg(WSAGetLastError()));
        closesocket(server_socket);
        WSACleanup();
        return 1;
    }
    appendStatus(QString("Server is connecting to client!"));

    //Nhận dữ liệu từ client
    recieve_from_client(client_socket);

    //Đóng kết nối
    closesocket(client_socket);
    closesocket(server_socket);
    WSACleanup();

    return 0;
}

cv::Mat captureScreen() {// chup anh man hinh
    // Khởi tạo màn hình chính
    HWND hwnd = GetDesktopWindow();
    HDC hwindowDC = GetDC(hwnd);
    HDC hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);

    // Kích thước màn hình
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);

    // Tạo ảnh bitmap và chọn nó vào DC tương thích
    HBITMAP hbwindow = CreateCompatibleBitmap(hwindowDC, screenWidth, screenHeight);
    BITMAPINFOHEADER bi = { sizeof(BITMAPINFOHEADER), screenWidth, -screenHeight, 1, 32, BI_RGB };

    SelectObject(hwindowCompatibleDC, hbwindow);
    BitBlt(hwindowCompatibleDC, 0, 0, screenWidth, screenHeight, hwindowDC, 0, 0, SRCCOPY);

    // Lấy dữ liệu bitmap và chuyển nó thành OpenCV Mat
    cv::Mat src(screenHeight, screenWidth, CV_8UC4);  // CV_8UC4: 8-bit, 4 channels
    GetDIBits(hwindowCompatibleDC, hbwindow, 0, screenHeight, src.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);

    // Giải phóng bộ nhớ
    DeleteObject(hbwindow);
    DeleteDC(hwindowCompatibleDC);
    ReleaseDC(hwnd, hwindowDC);

    return src;
}
bool server::getServerIP(SOCKET client_socket) {  // lay dia chi ip cua may
    // Khởi tạo Winsock
    WSADATA wsa;
    WSAStartup(MAKEWORD(2, 2), &wsa);

    // Lấy tên máy
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == SOCKET_ERROR) {
        return false;
    }

    // Lấy thông tin về host (bao gồm địa chỉ IP)
    struct addrinfo hints = { 0 }, * result = NULL;
    hints.ai_family = AF_INET;  // Chỉ quan tâm đến địa chỉ IPv4

    int ret = getaddrinfo(hostname, NULL, &hints, &result);
    if (ret != 0) {
        return false;
    }

    // Duyệt qua danh sách địa chỉ và lưu địa chỉ IP
    std::vector<std::string> list_IP;
    for (struct addrinfo* ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        struct sockaddr_in* sockaddr_ipv4 = (struct sockaddr_in*)ptr->ai_addr;
        char ipStr[INET_ADDRSTRLEN];

        // Chuyển địa chỉ từ dạng nhị phân sang chuỗi
        inet_ntop(AF_INET, &(sockaddr_ipv4->sin_addr), ipStr, sizeof(ipStr));
        list_IP.push_back(std::string(ipStr));
    }

    // Gửi số lượng địa chỉ IP
    int num_IPs = htonl(list_IP.size());
    if (send(client_socket, reinterpret_cast<char*>(&num_IPs), sizeof(num_IPs), 0) == SOCKET_ERROR) {
        return false;
    }
    // Gửi từng địa chỉ IP
    for (const auto& ip : list_IP) {
        if (send(client_socket, ip.c_str(), ip.size(), 0) == SOCKET_ERROR) {
            return false;
        }

    }
    freeaddrinfo(result);
    WSACleanup();
    return true;
}

bool server::StopService(const char* serviceName) {

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, serviceName, -1, NULL, 0);
    wchar_t* wString = new wchar_t[size_needed];
    MultiByteToWideChar(CP_UTF8, 0, serviceName, -1, wString, size_needed);
    // Mở Service Control Manager


    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCManager == NULL) {
        appendStatus(QString("Can't open Service Control Manager. ERROR: " + QString::number(GetLastError())));
        return false;
    }

    // Mở dịch vụ
    SC_HANDLE hService = OpenService(hSCManager, wString, SERVICE_STOP | SERVICE_QUERY_STATUS);
    if (hService == NULL) {
        appendStatus(QString("Can't open Service ") + QString::fromStdWString(wString) + ". ERROR: " + QString::number(GetLastError()));
        CloseServiceHandle(hSCManager);
        return false;
    }

    // Kiểm tra xem dịch vụ có đang chạy không
    SERVICE_STATUS_PROCESS ssp;
    DWORD dwBytesNeeded;
    if (!QueryServiceStatusEx(
        hService,
        SC_STATUS_PROCESS_INFO,
        (LPBYTE)&ssp,
        sizeof(SERVICE_STATUS_PROCESS),
        &dwBytesNeeded)) {
        appendStatus(QString("Can't get the status of the service, ERROR: " + QString::number(GetLastError())));
        CloseServiceHandle(hService);
        CloseServiceHandle(hSCManager);
        return false;
    }

    // Nếu dịch vụ đang chạy, dừng nó
    if (ssp.dwCurrentState != SERVICE_STOPPED) {
        appendStatus(QString("Stopping service ") + QString::fromUtf8(serviceName) + "...");

        SERVICE_STATUS serviceStatus;
        if (!ControlService(hService, SERVICE_CONTROL_STOP, &serviceStatus)) {
            appendStatus(QString("Can't stop the service. ERROR: " + QString::number(GetLastError())));
            return false;
        }
        else {
            appendStatus(QString("The service has been stopped successfully"));
            return true;
        }
    }
    else {
        appendStatus(QString("Service has stopped before."));
    }


    // Đóng các handle
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return true;
}
bool server::StartServiceByName(const std::wstring& serviceName) {
    // Mở Service Control Manager
    SC_HANDLE hSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);
    if (hSCManager == NULL) {
        appendStatus(QString("Can't open Service Control Manager. ERROR: " + QString::number(GetLastError())));
        return false;
    }

    // Mở dịch vụ
    SC_HANDLE hService = OpenService(hSCManager, serviceName.c_str(), SERVICE_START);
    if (hService == NULL) {
        appendStatus(QString("Can't open service, ERROR: " + QString::number(GetLastError())));
        CloseServiceHandle(hSCManager);
        return false;
    }
    appendStatus(QString("Starting service ") + QString(serviceName) + "...");
    // Bắt đầu dịch vụ
    if (StartService(hService, 0, NULL)) {
        appendStatus(QString("The service has been started successfully"));
        return true;
    }
    else {
        appendStatus(QString("Can't start service. ERROR: " + QString::number(GetLastError())));
        return false;
    }

    // Đóng handle dịch vụ và SCM
    CloseServiceHandle(hService);
    CloseServiceHandle(hSCManager);
    return true;
}
bool IsMainWindow(HWND hwnd) {
    return GetWindow(hwnd, GW_OWNER) == NULL && IsWindowVisible(hwnd);
}
BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    DWORD processID;
    GetWindowThreadProcessId(hwnd, &processID);
    DWORD targetProcessID = *(DWORD*)lParam;

    if (processID == targetProcessID && IsMainWindow(hwnd)) {
        PostMessage(hwnd, WM_CLOSE, 0, 0); // Gửi thông báo WM_CLOSE
        return FALSE; // Dừng việc duyệt cửa sổ
    }
    return TRUE; // Tiếp tục duyệt
}
bool server::CloseApplication(DWORD processID) {
    if (!EnumWindows(EnumWindowsProc, (LPARAM)&processID)) {
        appendStatus(QString("Success stop Process of ID: %1").arg(processID));
        return true;
    }
    else {
        appendStatus(QString("Can't stop Process of ID: %1").arg(processID));
        return false;
    }
}

bool getFile(SOCKET client_socket) {
    char filePath[1024];
    int bytes_received = recv(client_socket, filePath, sizeof(filePath), 0);
    filePath[bytes_received] = '\0';

    // Mở file
    std::ifstream file(filePath, std::ios::binary);
    if (!file) {
        const char* errorMsg = "Can't open file!";
        send(client_socket, errorMsg, strlen(errorMsg), 0);
        return false;
    }

    // Lấy kích thước file
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Gửi kích thước file
    send(client_socket, reinterpret_cast<const char*>(&fileSize), sizeof(fileSize), 0);

    // Gửi nội dung file
    char* buffer = new char[fileSize];
    if (file.read(buffer, fileSize)) {
        send(client_socket, buffer, fileSize, 0);
    }
    else {
        const char* errorMsg = "Can't read file!";
        send(client_socket, errorMsg, strlen(errorMsg), 0);
        return false;
    }

    delete[] buffer;  // Giải phóng bộ nhớ
    file.close();
}
bool deleteFile(SOCKET client_socket) {
    // Nhận kích thước của filePath
    uint32_t filePathSize;
    recv(client_socket, reinterpret_cast<char*>(&filePathSize), sizeof(filePathSize), 0);

    // Nhận filePath
    std::string filePath(filePathSize, '\0'); // Tạo chuỗi đủ kích thước
    recv(client_socket, &filePath[0], filePathSize, 0); // Nhận dữ liệu vào chuỗi

    // Thử xóa file
    if (std::remove(filePath.c_str()) == 0) {
        const char* successMsg = "File delete successfully";
        send(client_socket, successMsg, strlen(successMsg), 0);
        return true;
    }
    else {
        const char* errorMsg = "Can't delete file";
        send(client_socket, errorMsg, strlen(errorMsg), 0);
        return false;
    }
}
bool runProcess(SOCKET client_socket) {
    char processName[256];
    int bytes_received = recv(client_socket, processName, sizeof(processName), 0);
    processName[bytes_received] = '\0';

    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // Khởi động quy trình
    if (CreateProcessA(processName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        // Đợi quy trình kết thúc
        //WaitForSingleObject(pi.hProcess, INFINITE);

        // Đóng handle
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);

        return true;
    }
    else return false;
}

std::string ConvertLPWSTRToString(LPWSTR lpwstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, lpwstr, -1, NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, lpwstr, -1, &strTo[0], size_needed, NULL, NULL);
    return strTo;
}
std::vector<std::vector<std::string>> get_services() {
    std::vector<std::vector<std::string>> list_srevice;
    std::string result;
    SC_HANDLE scManager = OpenSCManager(NULL, NULL, SC_MANAGER_ENUMERATE_SERVICE);
    if (scManager == NULL) {

        return list_srevice;
    }

    DWORD bytesNeeded, servicesReturned, resumeHandle = 0;
    EnumServicesStatusEx(
        scManager,
        SC_ENUM_PROCESS_INFO,
        SERVICE_WIN32,
        SERVICE_STATE_ALL,
        NULL,
        0,
        &bytesNeeded,
        &servicesReturned,
        &resumeHandle,
        NULL
    );

    std::vector<BYTE> buffer(bytesNeeded);
    LPENUM_SERVICE_STATUS_PROCESS services = (LPENUM_SERVICE_STATUS_PROCESS)buffer.data();

    if (!EnumServicesStatusEx(
        scManager,
        SC_ENUM_PROCESS_INFO,
        SERVICE_WIN32,
        SERVICE_STATE_ALL,
        (LPBYTE)services,
        bytesNeeded,
        &bytesNeeded,
        &servicesReturned,
        &resumeHandle,
        NULL
    )) {
        CloseServiceHandle(scManager);
        return list_srevice;
    }

    for (DWORD i = 0; i < servicesReturned; i++) {
        std::vector<std::string> temp;

        std::string serviceName = ConvertLPWSTRToString(services[i].lpServiceName);
        std::string displayName = ConvertLPWSTRToString(services[i].lpDisplayName);
        std::string status;
        if (services[i].ServiceStatusProcess.dwCurrentState == 4) status = "Running";
        else status = "Stopping";
        temp.push_back(serviceName);
        temp.push_back(displayName);
        temp.push_back(status);
        list_srevice.push_back(temp);

    }


    CloseServiceHandle(scManager);
    std::cout << result;
    return list_srevice;
}

void server::recieve_from_client(SOCKET client_socket) {
    while (1) {
        char buffer[1024];
        int bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';

            appendStatus(QString("Receive order from client: " + QString(buffer)));
            if (strcmp(buffer, "Capture Shot") == 0) {
                appendStatus(QString("Camera Shotting ..."));
                cv::Mat image = captureImageFromCamera();
                sendImage(client_socket, image);
                appendStatus(QString("Send Image to Client Succesfully"));
            }
            if (strcmp(buffer, "List Process") == 0) {
                std::vector<std::vector<std::string>> listProcess = ListRunningProcesses();
                int numsProcess = listProcess.size();
                send(client_socket, reinterpret_cast<char*>(&numsProcess), sizeof(numsProcess), 0);
                appendStatus(QString("Sending data to client ..."));
                for (int i = 0; i < numsProcess; i++) {
                    send(client_socket, listProcess[i][0].c_str(), listProcess[i][0].size() + 1, 0);
                    Sleep(200);
                    send(client_socket, listProcess[i][1].c_str(), listProcess[i][1].size() + 1, 0);
                    Sleep(200);
                }
                appendStatus(QString("Send List Process to Client Succesfully"));
            }
            if (strcmp(buffer, "List Service") == 0) {

                std::vector<std::vector<std::string>> services = get_services();
                DWORD service_count = services.size();
                send(client_socket, reinterpret_cast<char*>(&service_count), sizeof(service_count), 0);
                appendStatus(QString("Sending data to client ..."));
                for (const auto& service : services) {
                    send(client_socket, service[0].c_str(), service[0].size() + 1, 0);
                    Sleep(170);
                    send(client_socket, service[1].c_str(), service[1].size() + 1, 0);
                    Sleep(170);
                    send(client_socket, service[2].c_str(), service[2].size() + 1, 0);
                    Sleep(170);
                }
                appendStatus(QString("Send List Service to Client Succesfully"));
            }
            if (strcmp(buffer, "Screen Shot") == 0) {
                cv::Mat image = captureScreen();
                sendImage(client_socket, image);
                appendStatus(QString("Send Image to Client Succesfully"));
            }
            if (strcmp(buffer, "Server IP") == 0) {
                appendStatus(QString("Sending data to client ..."));
                if (getServerIP(client_socket)) appendStatus(QString("Sent Ip Address to Client Successfully"));
                else appendStatus(QString("Send Ip Address to Client Unsuccessfully"));
            }
            if (strcmp(buffer, "Stop Service") == 0) {
                char nameService[1024];
                int byte_receive_nameService = recv(client_socket, nameService, sizeof(nameService), 0);
                nameService[byte_receive_nameService] = '\0';

                if (StopService(nameService)) {
                    std::string result;
                    result = "Service " + std::string(nameService) + " Stoped Successfully";
                    send(client_socket, result.c_str(), result.size(), 0);
                }
                else {
                    std::string result;
                    result = "Stop Service " + std::string(nameService) + " Failed";
                    send(client_socket, result.c_str(), result.size(), 0);
                }
            }
            if (strcmp(buffer, "Run Service") == 0) {
                char nameService[1024];
                int byte_recieve_nameService = recv(client_socket, nameService, sizeof(nameService), 0);
                nameService[byte_recieve_nameService] = '\0';


                //chuyen sang kieu wstring
                int bufferSize = MultiByteToWideChar(CP_UTF8, 0, nameService, -1, NULL, 0);
                std::wstring Service_to_running(bufferSize, L'\0');
                MultiByteToWideChar(CP_UTF8, 0, nameService, -1, &Service_to_running[0], bufferSize);

                if (StartServiceByName(Service_to_running)) {
                    std::string result = "Service " + std::string(nameService) + " is Running";
                    send(client_socket, result.c_str(), result.size(), 0);
                }
                else {
                    std::string result;
                    result = "Run Service " + std::string(nameService) + " Failed";
                    send(client_socket, result.c_str(), result.size(), 0);
                }
            }
            if (strcmp(buffer, "Stop Process") == 0) {
                DWORD Id;
                recv(client_socket, reinterpret_cast<char*>(&Id), sizeof(Id), 0);
                std::string result;
                if (CloseApplication(Id)) {
                    result = "Stop Process of ID " + std::to_string(Id) + " Successfully!";
                    send(client_socket, result.c_str(), result.size(), 0);
                }
                else {
                    result = "Stop Process of ID " + std::to_string(Id) + " Failed!";
                    send(client_socket, result.c_str(), result.size(), 0);
                }

            }
            if (strcmp(buffer, "Run Process") == 0) {
                std::string result;
                if (runProcess(client_socket)) {
                    result = "Run Process Successfully!";
                    appendStatus(QString::fromStdString(result));
                    send(client_socket, result.c_str(), result.size(), 0);
                }
                else {
                    result = "Run Process Failed!";
                    appendStatus(QString::fromStdString(result));
                    send(client_socket, result.c_str(), result.size(), 0);
                }
            }
            if (strcmp(buffer, "Get File") == 0) {
                if (getFile(client_socket)) appendStatus("Get file successfully");
                else appendStatus("Get file failed");
            }
            if (strcmp(buffer, "Delete File") == 0) {
                if (deleteFile(client_socket)) appendStatus("Delete file successfully");
            else appendStatus("Delete file failed");
            }
            if (strcmp(buffer, "Shutdown PC") == 0) {
                system("shutdown /s /t 0");
            }
            if (strcmp(buffer, "Shutdown Socket") == 0) {
                appendStatus("Client disconect to server");
                break;
            }
        }
    }
}
cv::Mat server::captureImageFromCamera() {
    // Mở camera với id 0 (camera mặc định)
    cv::VideoCapture cap(0);
    cv::Mat frame;

    // Chụp một khung hình từ camera
    cap >> frame;

    // Giải phóng camera
    cap.release();
    return frame;
}
void server::sendImage(SOCKET client_socket, const cv::Mat& image) {
    // Mã hóa ảnh dưới dạng JPEG
    std::vector<uchar> buf;
    cv::imencode(".jpg", image, buf);

    // Gửi kích thước của dữ liệu trước
    int imgSize = buf.size();
    send(client_socket, reinterpret_cast<const char*>(&imgSize), sizeof(int), 0);

    // Gửi dữ liệu ảnh
    send(client_socket, reinterpret_cast<const char*>(buf.data()), imgSize, 0);
}
std::string ConvertWCHARToString(WCHAR* wstr) {
    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    std::string strTo(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, &strTo[0], size_needed, NULL, NULL);
    return strTo;
}
std::vector<std::vector<std::string>> server::ListRunningProcesses()
{
    std::vector<std::vector<std::string>> result;
    HANDLE hProcessSnap;
    PROCESSENTRY32 pe32;

    // Lấy snapshot của tất cả các process trong hệ thống
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        appendStatus(QString("CreateToolhelp32Snapshot that bai."));
        return result;
    }

    // Khởi tạo cấu trúc PROCESSENTRY32 để lấy thông tin process đầu tiên
    pe32.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(hProcessSnap, &pe32))
    {
        appendStatus(QString("Process32First thất bại.")); // Không thể lấy thông tin process
        CloseHandle(hProcessSnap);
        return result;
    }
    do {
        std::vector<std::string> temp;
        std::string processName = ConvertWCHARToString(pe32.szExeFile);
        std::string processID = std::to_string(pe32.th32ProcessID);


        temp.push_back(processName);
        temp.push_back(processID);
        result.push_back(temp);
        //QString status = QString("%1   %2").arg(QString::fromStdWString(pe32.szExeFile)).arg(pe32.th32ProcessID);
        //appendStatus(status);
    } while (Process32Next(hProcessSnap, &pe32));

    CloseHandle(hProcessSnap);
    return result;
}
