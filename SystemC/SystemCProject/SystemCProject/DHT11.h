#ifndef DHT11_H
#define DHT11_H


class DHT11 {
public:
    // Phương thức khởi tạo
    DHT11(double pin);

    // Phương thức đọc nhiệt độ và độ ẩm từ cảm biến
    void read();

    // Biến lưu trữ giá trị nhiệt độ và độ ẩm
    float temperature;
    float humidity;

private:
    double _pin;

    // Phương thức đọc dữ liệu từ cảm biến DHT11
    double _readData();
};

#endif