document.addEventListener('DOMContentLoaded', () => {

    // ==========================================================
    // KHỐI LOGIC BỂ CÁ (GIỮ NGUYÊN)
    // ==========================================================
    const aquarium = document.querySelector('.aquarium-container');
    const numFish = 4;
    const fishes = [];

    function createFish() {
        if (!aquarium) return;
        for (let i = 0; i < numFish; i++) {
            const fish = document.createElement('div');
            fish.className = 'fish';
            aquarium.appendChild(fish);
            const bounds = aquarium.getBoundingClientRect();
            const fishWidth = 80;
            const fishHeight = 80;
            fish.x = Math.random() * (bounds.width - fishWidth);
            fish.y = Math.random() * (bounds.height - fishHeight);
            let speedX = (Math.random() - 0.5) * 4;
            fish.speedX = (speedX < 0.5 && speedX > -0.5) ? 1.5 : speedX;
            fish.speedY = (Math.random() - 0.5) * 1;
            fish.scaleX = (fish.speedX > 0) ? 1 : -1;
            fish.style.transform = `translate(${fish.x}px, ${fish.y}px) scaleX(${fish.scaleX})`;
            fishes.push(fish);
        }
    }

    function animateAquarium() {
        if (!aquarium) return;
        const bounds = aquarium.getBoundingClientRect();
        const fishWidth = 80;
        const fishHeight = 80;
        fishes.forEach(fish => {
            fish.x += fish.speedX;
            fish.y += fish.speedY;
            if (fish.x + fishWidth >= bounds.width) {
                fish.speedX *= -1;
                fish.x = bounds.width - fishWidth;
                fish.scaleX = -1;
            } else if (fish.x <= 0) {
                fish.speedX *= -1;
                fish.x = 0;
                fish.scaleX = 1;
            }
            if (fish.y + fishHeight >= bounds.height) {
                fish.speedY *= -1;
                fish.y = bounds.height - fishHeight;
            } else if (fish.y <= 0) {
                fish.speedY *= -1;
                fish.y = 0;
            }
            fish.style.transform = `translate(${fish.x}px, ${fish.y}px) scaleX(${fish.scaleX})`;
        });
        requestAnimationFrame(animateAquarium);
    }
    
    createFish();
    animateAquarium();


    // ==========================================================
    // KHỐI LOGIC MQTT (ĐÃ CẬP NHẬT)
    // ==========================================================

    // --- CẤU HÌNH ---
    const mqtt = window.parent.mqtt;
    const brokerHost = '192.168.137.14';
    const wsPort = 9001;

    // DOM Elements hiện thị dữ liệu (Đã thêm feedVal)
    const tempVal = document.getElementById('temp-val');
    const phVal = document.getElementById('ph-val');
    const doVal = document.getElementById('do-val');
    const motorVal = document.getElementById('motor-val');
    const feedVal = document.getElementById('feed-val'); // <-- 1. THÊM MỚI
    
    // DOM Element nút nguồn
    const powerBtn = document.getElementById('power-button');

    let client = null;
    let isConnected = false; 

    // ==========================================================
    // HÀM 1: KẾT NỐI MQTT (Đã cập nhật)
    // ==========================================================
    function connectMQTT() {
        if (isConnected) return; 

        console.log(`Đang kết nối đến ws://${brokerHost}:${wsPort}...`);
        client = mqtt.connect(`ws://${brokerHost}:${wsPort}`);

        client.on('connect', () => {
            console.log('>> MQTT: Đã kết nối thành công!');
            isConnected = true;
            powerBtn.classList.add('active');

            // Đăng ký các topic (Đã thêm feed_remaining)
            client.subscribe('esp32/data/temp');
            client.subscribe('esp32/data/ph');
            client.subscribe('esp32/data/do');
            client.subscribe('esp32/data/motor');
            client.subscribe('esp32/data/feed_remaining'); // <-- 2. THÊM MỚI
        });

        // --- SỰ KIỆN: NHẬN TIN NHẮN (Đã cập nhật) ---
        client.on('message', (topic, message) => {
            const msg = message.toString();

            switch (topic) {
                case 'esp32/data/temp':
                    if (tempVal) tempVal.innerText = msg;
                    break;
                case 'esp32/data/ph':
                    if (phVal) phVal.innerText = msg;
                    break;
                case 'esp32/data/do':
                    if (doVal) doVal.innerText = msg;
                    break;
                case 'esp32/data/motor':
                    if (motorVal) motorVal.innerText = (msg === 'RUNNING - 50%' ? 'Đang chạy' : 'Đã dừng');
                    break;
                case 'esp32/data/feed_remaining': // <-- 3. THÊM MỚI
                    if (feedVal) feedVal.innerText = msg;
                    break;
            }
        });

        client.on('error', (err) => {
            console.error('MQTT Lỗi:', err);
            disconnectMQTT(); 
        });

        client.on('close', () => {
            console.log('MQTT: Đã ngắt kết nối (Close).');
            handleUI_Disconnect(); 
        });
    }

    // ==========================================================
    // HÀM 2: NGẮT KẾT NỐI MQTT (GIỮ NGUYÊN)
    // ==========================================================
    function disconnectMQTT() {
        if (client) {
            console.log('>> Đang ngắt kết nối chủ động...');
            client.end(); 
            client = null;
        }
        handleUI_Disconnect();
    }

    // Hàm phụ để reset giao diện (Đã cập nhật)
    function handleUI_Disconnect() {
        isConnected = false;
        powerBtn.classList.remove('active');
        
        if(tempVal) tempVal.innerText = "NO VALUE";
        if(phVal) phVal.innerText = "NO VALUE";
        if(doVal) doVal.innerText = "NO VALUE";
        if(motorVal) motorVal.innerText = "STOP";
        if(feedVal) feedVal.innerText = "NO VALUE"; // <-- 4. THÊM MỚI
    }

    // ==========================================================
    // XỬ LÝ SỰ KIỆN CLICK NÚT NGUỒN (GIỮ NGUYÊN)
    // ==========================================================
    powerBtn.addEventListener('click', () => {
        if (isConnected) {
            disconnectMQTT();
        } else {
            connectMQTT();
        }
    });

    // ==========================================================
    // KHỞI CHẠY TỰ ĐỘNG KHI LOAD TRANG (GIỮ NGUYÊN)
    // ==========================================================
    connectMQTT(); 

});