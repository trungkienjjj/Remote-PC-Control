#ifndef REAL_H


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


class server : public QWidget {
    Q_OBJECT  // Sử dụng nếu bạn dùng signal và slot trong lớp
public:
    server(QWidget* parent = nullptr);

private slots:
    void startServerButton();
    void About_Us();
    void How_to_Use();
    void List_Command();
    int initializeSocket();
    void appendStatus(const QString& message);
    void recieve_from_client(SOCKET client_socket);
    cv::Mat captureImageFromCamera();
    void sendImage(SOCKET client_socket, const cv::Mat& image);
    std::vector<std::vector<std::string>> ListRunningProcesses();
    bool StopService(const char* serviceName);
    bool StartServiceByName(const std::wstring& serviceName);
    bool CloseApplication(DWORD processID);
    bool getServerIP(SOCKET client_socket);

private:
    QLineEdit* lineEdit;
    QPushButton* button;
    QPushButton* aboutus_button;
    QPushButton* htu_button;
    QPushButton* listcomd_button;
    QTextEdit* textEdit;  // Thêm khung hiển thị kết quả
    QLabel* statusLabel;
    QLabel* infoLabel;
    QTextEdit* infoEdit;
};

#endif 