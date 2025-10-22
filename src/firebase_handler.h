#ifndef FIREBASE_HANDLER_H
#define FIREBASE_HANDLER_H

#include <Arduino.h> // Cần cho "String"

// Thêm khai báo từ thư viện FirebaseClient
#include <FirebaseClient.h>

void setupFirebase();
void sendToFirebase();

// --- Các hàm mới cho việc "nghe" và xử lý bất đồng bộ ---

// Bắt đầu luồng "nghe" dữ liệu
void startFanStream();

void uploadFanData();

// Hàm callback chung khi gửi/push/set
void processData(AsyncResult &aResult);

// Hàm callback khi "nghe" (stream) có dữ liệu mới
void processFanData(AsyncResult &aResult);
void uploadAutoFanState();

#endif