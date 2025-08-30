# FT_IRC - IRC Server Implementation

## 📋 Proje Açıklaması

Bu proje, C++98 standardında yazılmış bir IRC (Internet Relay Chat) sunucusudur. 42 School projesi olarak geliştirilmiştir.

## 🏗️ Proje Yapısı

```
ft_irc/
├── main.cpp              # Ana program giriş noktası
├── Makefile              # Derleme dosyası
├── includes/             # Header dosyaları
│   ├── Server.hpp        # Server sınıfı tanımları
│   ├── Client.hpp        # Client sınıfı tanımları
│   └── Commands.hpp      # IRC komutları ve parser
├── src/                  # Kaynak dosyalar
│   ├── Server.cpp        # Server implementasyonu
│   ├── Client.cpp        # Client implementasyonu
│   └── Commands.cpp      # IRC komut işleyicileri
└── test_client.py        # Test script'i
```

## 🔧 Kurulum ve Çalıştırma

### Derleme
```bash
make clean  # Önceki derleme dosyalarını temizle
make        # Projeyi derle
```

### Çalıştırma
```bash
./ft_irc <port> <password>
```

**Örnek:**
```bash
./ft_irc 6667 mypassword123
```

### Test Etme
```bash
python3 test_client.py
```

## 📚 Kod Analizi

### 1. main.cpp - Program Giriş Noktası

**Ana İşlevler:**
- Komut satırı argümanlarını kontrol eder (port ve şifre)
- Port numarasının geçerliliğini doğrular (1024-65535 arası)
- Server nesnesini oluşturur ve başlatır
- Hata yönetimi yapar

**Kritik Kod Blokları:**
```cpp
int checkPort(const std::string &portStr)  // Port doğrulama
Server server(port, password);             // Server oluşturma
server.bindAndListen();                     // Socket bağlama
server.runServer();                         // Ana döngü
```

### 2. Server.hpp/cpp - Ana Sunucu Sınıfı

**Önemli Özellikler:**
- **Multi-client desteği**: `poll()` sistemi ile birden fazla istemci
- **Non-blocking I/O**: Asenkron bağlantı yönetimi
- **Client yönetimi**: std::map ile istemci takibi
- **Command parsing**: IRC protokol ayrıştırması

**Ana Metodlar:**
```cpp
Server(int &port, const std::string &password);  // Constructor
void bindAndListen();                             // Socket kurulumu
void runServer();                                 // Ana poll döngüsü
void addClient(int client_fd);                    // Yeni istemci ekleme
void removeClient(int client_fd);                 // İstemci çıkarma
void handleClientData(int client_fd);             // Veri işleme
```

**Poll Döngüsü Mantığı:**
```cpp
while (true) {
    int poll_count = poll(&poll_fds[0], poll_fds.size(), -1);

    for (size_t i = 0; i < poll_fds.size(); ++i) {
        if (poll_fds[i].revents & POLLIN) {
            if (poll_fds[i].fd == serverSocket) {
                // Yeni bağlantı kabul et
                addClient(accept(serverSocket, NULL, NULL));
            } else {
                // Mevcut istemci verisi işle
                handleClientData(poll_fds[i].fd);
            }
        }
    }
}
```

### 3. Client.hpp/cpp - İstemci Durumu

**İstemci Özellikleri:**
- **Kayıt durumu**: `_isRegistered`, `_hasPassword`, `_hasNick`, `_hasUser`
- **Buffer yönetimi**: `_readBuffer`, `_sendBuffer`
- **Kimlik bilgileri**: `_nickname`, `_username`, `_realname`

**Kayıt Sürecini:**
```cpp
void updateRegistrationStatus() {
    if (_hasPassword && _hasNick && _hasUser && !_isRegistered) {
        _isRegistered = true;
        // İstemci artık tam kayıtlı
    }
}
```

### 4. Commands.hpp/cpp - IRC Protokol İşlemcisi

**Desteklenen Komutlar:**
- **PASS**: Şifre doğrulama
- **NICK**: Takma ad belirleme
- **USER**: Kullanıcı bilgileri
- **PING**: Sunucu yanıt testi
- **QUIT**: Bağlantı sonlandırma

**Mesaj Ayrıştırma:**
```cpp
IRCMessage CommandParser::parseMessage(const std::string& message)
```

IRC formatı: `[PREFIX] COMMAND [params] [:trailing]`

**IRC Numeric Replies:**
```cpp
#define RPL_WELCOME(nick)     ":localhost 001 " + nick + " :Welcome..."
#define ERR_NEEDMOREPARAMS(nick, cmd) ":localhost 461 " + nick + " " + cmd + " :Not enough parameters"
```

## 🧪 Test Senaryoları

### Test Client Kullanımı:

1. **Bağlantı Testi**
   ```
   telnet localhost 6667
   ```

2. **Tam Kayıt Süreci**
   ```
   PASS mypassword123
   NICK TestUser
   USER TestUser 0 * :Real Name
   ```

3. **Ping Test**
   ```
   PING :test
   ```

4. **Hata Testleri**
   ```
   NICK           # Eksik parametre
   PASS wrong     # Yanlış şifre
   ```

### Python Test Script:

```bash
python3 test_client.py
```

Bu script otomatik olarak:
- Sunucuya bağlanır
- Kayıt işlemini tamamlar
- Ping-pong testi yapar
- Hata durumlarını test eder
- Temiz çıkış yapar

## 🔍 Hata Ayıklama

### Yaygın Sorunlar:

1. **Port zaten kullanımda**
   ```bash
   lsof -i :6667        # Port kontrolü
   kill -9 <PID>        # İşlem sonlandırma
   ```

2. **Bağlantı reddedildi**
   - Firewall ayarlarını kontrol edin
   - Port numarasının doğru olduğundan emin olun

3. **Derleme hataları**
   ```bash
   make clean && make   # Temiz derleme
   ```

## 📈 Performans Notları

- **Memory Management**: RAII kullanılarak otomatik hafıza yönetimi
- **Connection Handling**: poll() ile verimli I/O multiplexing
- **Error Handling**: Exception-safe kod tasarımı
- **C++98 Compliance**: Standart uyumluluk

## 🚀 Gelecek Geliştirmeler

- [ ] Channel (kanal) desteği
- [ ] Operator komutları
- [ ] File transfer desteği
- [ ] SSL/TLS şifreleme
- [ ] Logging sistemi
- [ ] Configuration dosyası

## 📝 Notlar

- Bu proje educational amaçlıdır
- Production kullanım için ek güvenlik önlemleri gereklidir
- IRC RFC 1459 standardına uygun geliştirilmiştir
