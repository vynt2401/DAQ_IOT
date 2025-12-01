// === Lấy các phần tử (elements) trên trang ===

// Lấy các ô nhập liệu
const inputPH = document.getElementById('input-ph');
const inputFood = document.getElementById('input-food');
const inputTemp = document.getElementById('input-temp');
const inputOxy = document.getElementById('input-oxy');

// Lấy các khung (item)
const phItem = document.getElementById('ph-item');
const foodItem = document.getElementById('food-item');
const tempItem = document.getElementById('temp-item');
const oxyItem = document.getElementById('oxy-item');

// Lấy các ô văn bản trạng thái
const phStatusText = document.getElementById('ph-status-text');
const foodStatusText = document.getElementById('food-status-text');
const tempStatusText = document.getElementById('temp-status-text');
const oxyStatusText = document.getElementById('oxy-status-text');


// === Gắn "tai nghe" (Event Listeners) cho các ô input ===
// Thay vì dùng oninput="" trong HTML, chúng ta dùng addEventListener
// 'input' có nghĩa là sự kiện này sẽ kích hoạt mỗi khi người dùng gõ phím
inputPH.addEventListener('input', updatePH);
inputFood.addEventListener('input', updateFood);
inputTemp.addEventListener('input', updateTemp);
inputOxy.addEventListener('input', updateOxy);


// === Các hàm cập nhật ===

// 1. Cập nhật pH
function updatePH() {
    const value = parseFloat(inputPH.value);
    // Xóa các lớp màu cũ
    phItem.classList.remove('status-good', 'status-warning', 'status-danger', 'status-default');
    
    // Ngưỡng: Vàng (6.5 - 8.0), Xanh (7.0 - 7.5), Đỏ (ngoài 6.5-8.0)
    if (value >= 7.0 && value <= 7.5) {
        phItem.classList.add('status-good'); // Ổn (Xanh)
        phStatusText.textContent = 'Trạng thái: Tốt';
    } else if (value >= 6.5 && value <= 8.0) {
        phItem.classList.add('status-warning'); // Bình thường (Vàng)
        phStatusText.textContent = 'Trạng thái: Bình thường';
    } else if (value < 6.5 || value > 8.0) {
        phItem.classList.add('status-danger'); // Nguy hiểm (Đỏ)
        phStatusText.textContent = 'Trạng thái: Nguy hiểm';
    } else {
        // Nếu xóa số, quay về mặc định
        phItem.classList.add('status-default');
        phStatusText.textContent = 'Trạng thái: Chưa xác định';
    }
}

// 2. Cập nhật Thức ăn
function updateFood() {
    const value = parseFloat(inputFood.value);
    foodItem.classList.remove('status-good', 'status-warning', 'status-danger', 'status-default');

    // Ngưỡng (ví dụ: %)
    if (value < 30) {
        foodItem.classList.add('status-danger'); // Thấp (Đỏ)
        foodStatusText.textContent = 'Trạng thái: Thấp (Nguy hiểm)';
    } else if (value <= 70) {
        foodItem.classList.add('status-good'); // Vừa (Xanh)
        foodStatusText.textContent = 'Trạng thái: Vừa đủ (Tốt)';
    } else if (value > 70) {
        foodItem.classList.add('status-warning'); // Nhiều (Vàng)
        foodStatusText.textContent = 'Trạng thái: Dư thừa (Cảnh báo)';
    } else {
        foodItem.classList.add('status-default');
        foodStatusText.textContent = 'Trạng thái: Chưa xác định';
    }
}

// 3. Cập nhật Nhiệt độ
function updateTemp() {
    const value = parseFloat(inputTemp.value);
    tempItem.classList.remove('status-good', 'status-warning', 'status-danger', 'status-default');
    
    // Ngưỡng (ví dụ °C)
    if (value >= 26 && value <= 30) {
        tempItem.classList.add('status-good'); // Tốt (Xanh)
        tempStatusText.textContent = 'Trạng thái: Ổn định (Tốt)';
    } else if ((value >= 24 && value < 26) || (value > 30 && value <= 32)) {
        tempItem.classList.add('status-warning'); // Bình thường (Vàng)
        tempStatusText.textContent = 'Trạng thái: Cảnh báo';
    } else if (value < 24 || value > 32) {
        tempItem.classList.add('status-danger'); // Nguy hiểm (Đỏ)
        tempStatusText.textContent = 'Trạng thái: Nguy hiểm';
    } else {
        tempItem.classList.add('status-default');
        tempStatusText.textContent = 'Trạng thái: Chưa xác định';
    }
}

// 4. Cập nhật Oxy
function updateOxy() {
    const value = parseFloat(inputOxy.value);
    oxyItem.classList.remove('status-good', 'status-warning', 'status-danger', 'status-default');

    // Ngưỡng (ví dụ: mg/L)
    if (value >= 5) {
        oxyItem.classList.add('status-good'); // Tốt (Xanh)
        oxyStatusText.textContent = 'Trạng thái: Tốt';
    } else if (value >= 3 && value < 5) {
        oxyItem.classList.add('status-warning'); // Vừa (Vàng)
        oxyStatusText.textContent = 'Trạng thái: Cảnh báo';
    } else if (value < 3) {
        oxyItem.classList.add('status-danger'); // Thấp (Đỏ)
        oxyStatusText.textContent = 'Trạng thái: Nguy hiểm';
    } else {
        oxyItem.classList.add('status-default');
        oxyStatusText.textContent = 'Trạng thái: Chưa xác định';
    }
}