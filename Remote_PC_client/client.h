#ifndef CLIENT_H

#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTextEdit>
#include <QLabel>
#include <QFont>
#include <QApplication>
#include <winsock2.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <vector>

class client : public QWidget {
    Q_OBJECT  // Sử dụng nếu bạn dùng signal và slot trong lớp
public:
    client(QWidget* parent = nullptr);

private slots:
    void onButtonClicked();
    void About_Us();
    void How_to_Use();
    void List_Command();
    int initializeSocket(const QString& ipAddress);
    void appendStatus(const QString& message);
    void send_to_server(SOCKET client_socket, std::string accessToken, int intervalSeconds);
    cv::Mat receiveImage(SOCKET server_socket);
    std::vector<std::string> receive_serverIP(SOCKET client_socket);
    std::vector<std::vector<std::string>> ReceiveProcessList(SOCKET client_socket);
    std::vector<std::vector<std::string>> ReceiveServiceList(SOCKET client_socket);

private:
    QLineEdit* lineEdit;
    QPushButton* button_2;
    QPushButton* aboutus_button;
    QPushButton* htu_button;
    QPushButton* listcomd_button;
    QTextEdit* textEdit;  // Thêm khung hiển thị kết quả
    QLabel* statusLabel;
    QLabel* infoLabel;
    QTextEdit* infoEdit;
    
};


#endif