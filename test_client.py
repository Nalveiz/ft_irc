#!/usr/bin/env python3
"""
IRC Client Test Script
Bu script ft_irc sunucusunu test etmek için kullanılır.
"""

import socket
import time
import threading

class IRCTestClient:
    def __init__(self, host='localhost', port=6667, password='test123'):
        self.host = host
        self.port = port
        self.password = password
        self.socket = None
        self.running = False

    def connect(self):
        """Sunucuya bağlan"""
        try:
            self.socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            self.socket.connect((self.host, self.port))
            print(f"✅ Sunucuya bağlandı: {self.host}:{self.port}")
            return True
        except Exception as e:
            print(f"❌ Bağlantı hatası: {e}")
            return False

    def send_command(self, command):
        """IRC komutu gönder"""
        if self.socket:
            message = command + "\r\n"
            self.socket.send(message.encode('utf-8'))
            print(f"📤 Gönderildi: {command}")

    def receive_messages(self):
        """Sunucudan gelen mesajları dinle"""
        buffer = ""
        while self.running:
            try:
                data = self.socket.recv(1024).decode('utf-8')
                if not data:
                    break

                buffer += data
                while '\r\n' in buffer:
                    line, buffer = buffer.split('\r\n', 1)
                    if line:
                        print(f"📥 Alındı: {line}")
            except Exception as e:
                print(f"❌ Alma hatası: {e}")
                break

    def start_receiver(self):
        """Mesaj alma thread'ini başlat"""
        self.running = True
        receiver_thread = threading.Thread(target=self.receive_messages)
        receiver_thread.daemon = True
        receiver_thread.start()

    def test_basic_registration(self, nickname="TestUser"):
        """Temel kayıt işlemini test et"""
        print("\n🧪 === TEMEL KAYIT TESTİ ===")

        # 1. PASS komutu
        self.send_command(f"PASS {self.password}")
        time.sleep(0.5)

        # 2. NICK komutu
        self.send_command(f"NICK {nickname}")
        time.sleep(0.5)

        # 3. USER komutu
        self.send_command(f"USER {nickname} 0 * :Real Name")
        time.sleep(1)

        print("✅ Kayıt işlemi tamamlandı")

    def test_ping_pong(self):
        """PING-PONG testini yap"""
        print("\n🧪 === PING-PONG TESTİ ===")
        self.send_command("PING :test123")
        time.sleep(0.5)

    def test_error_cases(self):
        """Hata durumlarını test et"""
        print("\n🧪 === HATA DURUMLARI TESTİ ===")

        # Eksik parametre
        self.send_command("NICK")
        time.sleep(0.5)

        # Yanlış şifre
        self.send_command("PASS wrongpassword")
        time.sleep(0.5)

        # Eksik USER parametresi
        self.send_command("USER")
        time.sleep(0.5)

    def run_full_test(self):
        """Tam test sürecini çalıştır"""
        if not self.connect():
            return

        self.start_receiver()
        time.sleep(0.5)

        # Test senaryoları
        self.test_basic_registration()
        self.test_ping_pong()
        self.test_error_cases()

        # Çıkış
        print("\n🧪 === ÇIKIŞ TESTİ ===")
        self.send_command("QUIT :Test tamamlandı")
        time.sleep(1)

        self.running = False
        if self.socket:
            self.socket.close()
        print("✅ Test tamamlandı")

def main():
    print("🚀 IRC Server Test Başlatılıyor...")
    print("📋 Test Senaryoları:")
    print("   1. Sunucuya bağlantı")
    print("   2. PASS, NICK, USER kayıt işlemi")
    print("   3. PING-PONG testi")
    print("   4. Hata durumları")
    print("   5. QUIT çıkış")
    print("-" * 50)

    # Sunucu bilgileri (varsayılan)
    host = 'localhost'
    port = 6667
    password = 'test123'

    client = IRCTestClient(host, port, password)
    client.run_full_test()

if __name__ == "__main__":
    main()
