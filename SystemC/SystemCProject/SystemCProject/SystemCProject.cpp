#include <systemc.h>
bool flagTem = true;
bool flagHum = true;
bool flagStop = false;
// Khối Controller
SC_MODULE(Controller) {
    sc_in<double> desiredTemp; // Tín hiệu nhiệt độ mong muốn
    sc_in<double> desiredHumidity; // Tín hiệu độ ẩm mong muốn
    sc_in<double> currentTemp;
    sc_in<double> currentHumidity;
    sc_out<bool> fan; // Tín hiệu bật/tắt quạt
    sc_out<bool> heater; // Tín hiệu bật/tắt bộ sưởi
    sc_out<bool> compressor; // Tín hiệu bật/tắt bộ làm lạnh 
    
    void controlProcess() {
        while (true) {
            cout << flagTem << endl;
            cout << currentTemp.read() << endl;
            cout << desiredTemp.read() << endl;
            if (currentTemp.read() < desiredTemp.read()) {
                compressor.write(false); // Tắt bộ làm lạnh
                heater.write(true); // Bật sưởi
                flagTem = true;
            }
             else if (currentTemp.read() == desiredTemp.read())
            {
                flagStop == true;
            }
            else {
               
                compressor.write(true); // Bật bộ làm lạnh
                heater.write(false); // Tắt sưởi
                flagTem = false;
            }

            if (currentHumidity.read() > desiredHumidity.read()) {
                fan.write(true); // Bật quạt
                flagHum = true;
            }
            else {
                fan.write(false); // Tắt quạt
                flagHum = false;
            }   

            wait(4, SC_SEC); // Đợi 1 giây trước khi kiểm tra lại
        }
    }

    SC_CTOR(Controller) {
        SC_THREAD(controlProcess);
    }
};

// Khối Software
SC_MODULE(Software) {
    sc_in<bool> fan; // Tín hiệu bật/tắt quạt
    sc_in<bool> heater; // Tín hiệu bật/tắt bộ lọc nhiệt
    sc_in<bool> compressor; // Tín hiệu bật/tắt bộ làm lạnh 
    sc_in<double> currentTemp; // Tín hiệu nhiệt độ hiện tại
    sc_in<double> currentHumidity; // Tín hiệu độ ẩm hiện tại
    void softwareProcess() {
        while (true) {
            // Đọc nhiệt độ hiện tại và trạng thái của quạt và bộ lọc nhiệt
            double temp = currentTemp.read();
            double hum = currentHumidity.read();
            bool fanStatus = fan.read();
            bool heaterStatus = heater.read();
            bool compressorStatus = compressor.read();
            // Hiển thị thông tin
            cout << "Current Temperature: " << temp << " C" << endl;
            cout << "Current Humidity: " << hum << " %" << endl;
            cout << "Fan Status: " << (fanStatus ? "ON" : "OFF") << endl;
            cout << "Heater Status: " << (heaterStatus ? "ON" : "OFF") << endl;
            cout << "Compressor Status: " << (compressorStatus ? "ON" : "OFF") << endl;
            cout << "=========================" << endl;
            wait(3, SC_SEC); // Đợi 2 giây trước khi kiểm tra lại
        }
    }

    SC_CTOR(Software) {
        SC_THREAD(softwareProcess);
    }
};

// Khối InputControl
SC_MODULE(InputControl) {
    sc_out<double> desiredTemp; // Tín hiệu nhiệt độ mong muốn
    sc_out<double> desiredHumidity; // Tín hiệu độ ẩm mong muốn
    sc_in<bool> increaseTempButton; // Tín hiệu nút bấm nhiệt độ
    sc_in<bool> increaseHumButton; // Tín hiệu nút bấm độ ẩm
    sc_in<bool> decreaseTempButton; // Tín hiệu nút bấm giảm nhiệt độ
    sc_in<bool> decreaseHumButton; // Tín hiệu nút bấm giảm nhiệt độ
    void inputProcess() {
        double temp = 25.0; // Nhiệt độ mong muốn ban đầu
        double humidity = 50.0; // Độ ẩm mong muốn ban đầu

        while (true) {
            // Kiểm tra nút bấm nhiệt độ
            if (increaseTempButton.read()) {
                temp += 1.0; // Tăng nhiệt độ mong muốn lên 1 đơn vị
                cout << "Desired Temperature: " << temp << " C" << endl;
            }

            if (decreaseTempButton.read()) {
                temp += 1.0; // Tăng nhiệt độ mong muốn lên 1 đơn vị
                cout << "Desired Temperature: " << temp << " C" << endl;
            }
            // Kiểm tra nút bấm độ ẩm
            if (increaseHumButton.read()) {
                humidity += 5.0; // Tăng độ ẩm mong muốn lên 5 đơn vị
                cout << "Desired Humidity: " << humidity << " %" << endl;
            }
            if (decreaseHumButton.read()) {
                humidity -= 5.0; // Tăng độ ẩm mong muốn lên 5 đơn vị
                cout << "Desired Humidity: " << humidity << " %" << endl;
            }
            // Gửi tín hiệu nhiệt độ và độ ẩm mong muốn
            desiredTemp.write(temp);
            desiredHumidity.write(humidity);

            wait(1, SC_SEC); // Đợi 0.1 giây trước khi kiểm tra lại
        }
    }

    SC_CTOR(InputControl) {
        SC_THREAD(inputProcess);
    }
};

// Khối DHT11
SC_MODULE(DHT11) {
    sc_out<double> currentTemp; // Tín hiệu nhiệt độ hiện tại
    sc_out<double> currentHumidity; // Tín hiệu độ ẩm hiện tại
    void readTemperature() {
            double temp = 28;
            double hum = 60;
        while (true) {
            // Đọc dữ liệu từ cảm biến nhiệt độ và độ ẩm DHT11
                  // Gửi tín hiệu nhiệt độ và độ ẩm hiện tại
           
            if (flagHum == true)
            {
                hum -= 0.5;
            }
            if (flagStop == true)
            {
                temp = temp;
            }
            else
            {
                if (flagTem == true)
                {
                    temp += 0.5;
                }
                else
                {
                    temp -= 0.5;
                }

            }
            currentTemp.write(temp);
            currentHumidity.write(hum);
            wait(2, SC_SEC); // Đợi 5 giây trước khi đọc lại dữ liệu
        }
    }

    SC_CTOR(DHT11) {
        SC_THREAD(readTemperature);
    }
};


// Hàm đọc nhiệt độ và độ ẩm từ cảm biến DHT11
void readTemperatureAndHumidityFromDHT11(double& temperature, double& humidity) {
    //DHT11 dht11; // Khởi tạo đối tượng cảm biến DHT11
    //int chk = dht11.read(); // Đọc dữ liệu từ cảm biến

    temperature = 34; // Lấy giá trị nhiệt độ từ đối tượng cảm biến
    humidity = 80; // Lấy giá trị độ ẩm từ đối tượng cảm biến
    //}
    //else {
    //    // Đặt giá trị nhiệt độ và độ ẩm là một giá trị đặc biệt để biểu thị rằng không thể đọc từ cảm biến
    //    temperature = -999.0;
    //    humidity = -999.0;
    //}
}

double readTemperatureFromDHT11() {
    double temperature, humidity;
    readTemperatureAndHumidityFromDHT11(temperature, humidity);
    return temperature;
}

double readHumidityFromDHT11() {
    double temperature, humidity;
    readTemperatureAndHumidityFromDHT11(temperature, humidity);
    return humidity;
}
int sc_main(int argc, char* argv[]) {
    // Tạo tín hiệu và đối tượng
    sc_signal<double> desiredTemp;
    sc_signal<double> desiredHumidity;
    sc_signal<bool> compressor;
    sc_signal<bool> fan;
    sc_signal<bool> heater;
    sc_signal<double> currentTemp;
    sc_signal<double> currentHumidity;
    sc_signal<bool> decreaseTempButton;
    sc_signal<bool> decreaseHumButton;
    sc_signal<bool> increaseTempButton;
    sc_signal<bool> increaseHumButton;
    Controller controller("Controller");
    controller.desiredTemp(desiredTemp);
    controller.desiredHumidity(desiredHumidity);
    controller.currentTemp(currentTemp);
    controller.currentHumidity(currentHumidity);
    controller.fan(fan);
    controller.heater(heater);
    controller.compressor(compressor);

    Software software("Software");
    software.fan(fan);
    software.heater(heater);
    software.compressor(compressor);
    software.currentTemp(currentTemp);
    software.currentHumidity(currentHumidity);
    
    InputControl inputControl("InputControl");
    inputControl.desiredTemp(desiredTemp);
    inputControl.desiredHumidity(desiredHumidity);
    inputControl.increaseTempButton(increaseTempButton);
    inputControl.increaseHumButton(increaseHumButton);
    inputControl.decreaseTempButton(decreaseTempButton);
    inputControl.decreaseHumButton(decreaseHumButton);

    DHT11 dht11("DHT11");
    dht11.currentTemp(currentTemp);
    dht11.currentHumidity(currentHumidity);
    // Chạy mô phỏng
    sc_start(60, SC_SEC); // Chạy mô phỏng trong 30 giây

    return 0;
}