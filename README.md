<h1> Simple Calculator — ESP32 + FreeRTOS + Keypad + LCD 16x2</h1>

<p>Dự án máy tính đơn giản được xây dựng trên <strong>ESP32</strong>, sử dụng <strong>FreeRTOS</strong>, <strong>Keypad 4x4</strong>,
<strong>LCD I2C 16x2</strong> và hỗ trợ nhập biểu thức giống máy tính bỏ túi mini.</p>

<p>Repo: <a href="https://github.com/PhiTruong69/Simple-calculator">https://github.com/PhiTruong69/Simple-calculator</a></p>
<table>
  <tr>
    <td align="center">
      <img src="https://github.com/user-attachments/assets/26641423-68ad-4cc3-8b39-4052f88a7a14" width="500"><br>
      May tinh cua em !
    </td>
      <td align="center">
      <img src="https://github.com/user-attachments/assets/37849169-f713-46a8-8a49-c3509116b318" width="500"><br>
      May tinh cua em !
    </td>
  </tr>
</table>



<hr>

<h2> Tính năng nổi bật</h2>

<h3> 1. Nhập biểu thức dạng chuỗi</h3>
<ul>
  <li>Người dùng nhập trực tiếp các ký tự số và toán tử từ Keypad 4x4.</li>
  <li>Biểu thức hiển thị thời gian thực trên LCD.</li>
  <li>Hỗ trợ nhiều toán hạng, không giới hạn 2 giá trị như yêu cầu tối thiểu.</li>
</ul>

<h3> 2. Hỗ trợ 6 phép tính</h3>
<table>
  <tr><th>Phép toán</th><th>Ký tự</th><th>Ghi chú</th></tr>
  <tr><td>Cộng</td><td>+</td><td>Mode 1</td></tr>
  <tr><td>Trừ</td><td>-</td><td>Mode 1</td></tr>
  <tr><td>Nhân</td><td>*</td><td>Mode 1</td></tr>
  <tr><td>Chia</td><td>/</td><td>Mode 2</td></tr>
  <tr><td>Modulo</td><td>m</td><td>Mode 2</td></tr>
  <tr><td>Đổi sang nhị phân</td><td>q</td><td>Mode 2</td></tr>
</table>

<h3> 3. Hai chế độ hoạt động</h3>
<ul>
  <li>Nhấn phím <strong>D</strong> để chuyển Mode.</li>
  <li><strong>LED GPIO 48</strong> bật khi ở Mode 2.</li>
  <li>Mỗi mode sử dụng mapping keypad khác nhau.</li>
</ul>

<h3> 4. Xử lý bất đồng bộ bằng FreeRTOS</h3>
<ul>
  <li><strong>TaskKeypad</strong> – đọc phím liên tục và gửi ký tự vào hàng đợi.</li>
  <li><strong>TaskLogic</strong> – xử lý logic, cập nhật LCD, tính biểu thức.</li>
</ul>

<h3> 5. Tự viết bộ phân tích biểu thức</h3>
<ul>
  <li>Dùng 2 stack: số & toán tử.</li>
  <li>Hỗ trợ: ưu tiên toán tử, số âm, lỗi cú pháp.</li>
  <li>Hiển thị lỗi rõ ràng: <code>Math Error</code>, <code>Syntax Error</code>.</li>
</ul>

<h3> 6. LCD hiển thị trực quan</h3>
<ul>
  <li>Hiển thị biểu thức</li>
  <li>Hiển thị kết quả</li>
  <li>Hiển thị mode</li>
  <li>Hiển thị số nhị phân</li>
</ul>

<hr>

<h2> Phần cứng sử dụng</h2>
<table>
  <tr><th>Thiết bị</th><th>Ghi chú</th></tr>
  <tr><td>Yolo UNO</td><td>Chạy FreeRTOS</td></tr>
  <tr><td>Keypad 4x4</td><td>Nhập số + toán tử</td></tr>
  <tr><td>LCD I2C 16x2 (0x21)</td><td>Hiển thị</td></tr>
  <tr><td>LED GPIO 48</td><td>Báo Mode 2</td></tr>
  <tr><td>I2C SDA 11 / SCL 12</td><td>Kết nối LCD</td></tr>
  <tr><td>Row 5,6,7,8 — Col 21,10,17,18</td><td>Dây keypad</td></tr>
</table>

<hr>

<h2> Cấu trúc mã nguồn</h2>

<pre>
main.cpp
 ├── Cấu hình phần cứng
 ├── Map keypad theo mode
 ├── Eval biểu thức
 ├── Chuyển đổi nhị phân
 ├── TaskKeypad (FreeRTOS)
 ├── TaskLogic   (FreeRTOS)
 ├── Setup LCD, Queue, Task
</pre>

<hr>

<h2> Cách chạy dự án</h2>

<h3>Yêu cầu:</h3>
<ul>
  <li>PlatformIO</li>
  <li>Yolo UNO</li>
  <li>Các thư viện: Keypad, LiquidCrystal_I2C</li>
</ul>

<h2> Hướng dẫn sử dụng</h2>

<ul>
  <li><strong>Nhập biểu thức:</strong> bấm các phím số & toán tử.</li>
  <li><strong>Tính kết quả:</strong> nhấn <code>#</code>.</li>
  <li><strong>Xóa:</strong> nhấn <code>R</code>.</li>
  <li><strong>Đổi mode:</strong> nhấn <code>D</code>.</li>
  <li><strong>Đổi sang nhị phân:</strong> bấm <code>q</code> trong Mode 2.</li>
</ul>

<hr>

<h2>Ví dụ sử dụng</h2>

<table>
  <tr><th>Input</th><th>Output</th></tr>
  <tr><td>12+34=</td><td>46</td></tr>
  <tr><td>50/0=</td><td>Math Error</td></tr>
  <tr><td>5m2=</td><td>1</td></tr>
  <tr><td>10 q</td><td>1010</td></tr>
</table>

<hr>


<h2> Hướng phát triển</h2>
<ul>
  <li>Thêm dấu ngoặc <code>()</code></li>
  <li>Thêm phép toán nâng cao: sin, cos, pow…</li>
  <li>Hỗ trợ lịch sử phép tính</li>
  <li>Kết nối Bluetooth / UART</li>
</ul>

<hr>

<p>MIT License</p>



