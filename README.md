# Remote PC Control via Email (C++)

Đây là đồ án môn Mạng máy tính, xây dựng một hệ thống cho phép điều khiển máy tính từ xa bằng cách gửi các câu lệnh qua email, được lập trình hoàn toàn bằng C++.

---

## 🎬 Video Demo

*(GHI CHÚ: Đây là phần quan trọng nhất! Hãy dán link video demo/thuyết trình của bạn vào đây.)*

**Link video:** [https://www.youtube.com/watch?v=rWUEGhtDJEY]

---

## 📜 Giới thiệu dự án (About The Project)

Dự án này mô phỏng một kỹ thuật kiểm soát máy tính từ xa ở mức độ cơ bản. Một chương trình client chạy ẩn trên máy tính mục tiêu sẽ liên tục kiểm tra một hòm thư email được chỉ định. Khi có email mới chứa câu lệnh hợp lệ được gửi đến, chương trình sẽ thực thi câu lệnh đó và gửi kết quả (nếu có) về lại email của người điều khiển.

### Sơ đồ kiến trúc

graph TD;
    subgraph "Người Điều Khiển (Controller)";
        A["📱 Gửi email chứa lệnh (vd: 'SCREENSHOT')"];
        I["📥 Nhận email chứa kết quả"];
    end

    subgraph "Server Email (Gmail, Outlook...)";
        B[("📧")];
    end

    subgraph "Máy Tính Mục Tiêu (Target PC)";
        C["💻 Chương trình Client (C++)"];
        D["🖥️ Hệ Điều Hành (OS)"];
    end

    A -- Gửi Lệnh --> B;
    C -- "1. Kiểm tra email định kỳ (IMAP)" --> B;
    B -- "2. Có email mới chứa lệnh" --> C;
    C -- "3. Phân tích và thực thi lệnh" --> D;
    D -- "4. Trả kết quả (vd: tạo file ảnh)" --> C;
    C -- "5. Gửi email chứa kết quả (SMTP)" --> B;
    B -- "6. Chuyển tiếp email kết quả" --> I;

---

## ✨ Các câu lệnh hỗ trợ (Supported Commands)

Chương trình có khả năng thực thi các câu lệnh sau được gửi từ email:

Chào bạn, tôi đã đọc lại file báo cáo đồ án Mạng máy tính (MMT_Project.pdf) của bạn để lấy danh sách đầy đủ các câu lệnh mà chương trình hỗ trợ.

Dưới đây là nội dung hoàn chỉnh cho mục "Các câu lệnh hỗ trợ" mà bạn có thể sao chép và dán vào file README.md của mình.

✨ Các câu lệnh hỗ trợ (Supported Commands)
Chương trình có khả năng thực thi các câu lệnh sau được gửi từ email:

SCREENSHOT: Chụp lại toàn bộ màn hình và gửi file ảnh về email.

KEYLOG [start/stop/dump]:

start: Bắt đầu ghi lại các phím được bấm.

stop: Dừng việc ghi phím.

dump: Gửi file log chứa các phím đã ghi được về email.

PROCESSLIST: Lấy danh sách các tiến trình đang chạy và gửi về email.

APPSTART: Lấy danh sách các ứng dụng đang chạy và gửi về email.

SHUTDOWN: Ra lệnh cho máy tính tự động tắt nguồn.

QUIT: Ra lệnh cho chương trình client tự tắt.

---

## 🛠️ Công nghệ sử dụng (Tech Stack)

* **Ngôn ngữ:** C++
* **Khái niệm cốt lõi:** Lập trình Socket, xử lý giao thức mạng (IMAP/SMTP), lập trình đa luồng (multi-threading).
* **Thư viện (có thể):** OpenSSL, Boost.Asio, hoặc các thư viện socket tùy chỉnh.

---

## 🚀 Bắt đầu (Getting Started)

### Yêu cầu

* Một trình biên dịch C++ (ví dụ: g++, MinGW).
* Thư viện OpenSSL để xử lý kết nối SSL/TLS với server email.

### Hướng dẫn cài đặt & Cấu hình

1.  **Clone repository:**
    ```sh
    git clone [Dán link repo Remote PC Control của bạn vào đây]
    ```
2.  **Cài đặt các thư viện phụ thuộc.**
3.  **Cấu hình thông tin nhạy cảm:**
    * Tạo một file `credentials.json` theo mẫu.
    * Điền thông tin email và mật khẩu ứng dụng vào file này.
    * **Lưu ý:** File `credentials.json` đã được thêm vào `.gitignore` và sẽ không được đưa lên repository.
4.  **Biên dịch và chạy chương trình.**

---

## 👥 Đội ngũ phát triển (Team)

Dự án này là sản phẩm hợp tác của nhóm sinh viên trong khuôn khổ môn học.
