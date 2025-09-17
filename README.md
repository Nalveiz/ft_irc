[![License: MIT](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![Language: C++](https://img.shields.io/badge/Language-C%2B%2B-blue.svg)](https://www.cplusplus.com/)
[![Platform: Linux](https://img.shields.io/badge/Platform-Linux-orange.svg)](https://www.linux.org/)

<h1 align="center">💬 Gerçek Zamanlı IRC Sunucusu<br>(FT_IRC – C++)</h1>

## 📌 Proje Hakkında


`ft_irc`, C++ kullanılarak geliştirilmiş, **RFC1459 standardına uyumlu** bir IRC (Internet Relay Chat) sunucusudur.  
IRC, modern mesajlaşma uygulamalarının (Slack, Discord vb.) temelini oluşturan klasik protokollerden biridir.  
Bu proje, düşük seviyeli ağ programlama ve event-driven mimariler üzerine pratik bir altyapı geliştirmeyi hedeflemektedir.  

Projeyi geliştirirken **socket programlama**, **poll() tabanlı event loop**, **çoklu istemci yönetimi** ve **protokol tasarımı** gibi ileri seviye konular üzerine tasarlandı.  
Sonuç olarak, gerçek dünyada kullanılabilecek sağlam bir mesajlaşma altyapısı ortaya konuldu. 🚀


---

## ⚙️ Özellikler
- 🔌 **Socket Programlama**: TCP/IP üzerinden bağlantı yönetimi  
- 🔄 **poll() tabanlı event loop**: Aynı anda yüzlerce kullanıcı bağlantısını yönetebilme  
- 👥 **Çoklu İstemci Desteği**: Birden fazla kullanıcıyı aynı anda bağlama ve mesajlaşma  
- 🛠️ **IRC Komutları**:
  - `NICK`, `USER`, `JOIN`, `PART`, `PRIVMSG`, `KICK`, `QUIT`  
- 🔑 **Kanal Yönetimi**:
  - Kanal oluşturma ve katılım  
  - Operator (OP) yetkileri  
  - Kick/ban desteği  
- 📡 **Gerçek Zamanlı Mesajlaşma**:
  - Kullanıcıdan kullanıcıya mesaj  
  - Kanal içi toplu mesaj  
- 🧩 **Genişletilebilir Mimari**: Yeni komut ve özellikler kolayca eklenebilir  

---

## 🔗 İstemci – Sunucu İletişim Akışı

Aşağıdaki diyagram, sunucu ile istemci arasındaki tipik iletişim sürecini özetler:

```text
+---------+                                   +---------+
| Client  |                                   | Server  |
+---------+                                   +---------+
    |                                              |
    | socket()                                     | socket()
    | connect()  ------------------------------->  | bind()
    |                                              | listen()
    |                                              | poll()
    |                                              |
    | send(request) ---------------------------->  | accept()
    |                                              | recv(request)
    |                                              |
    | recv(response) <---------------------------  | send(response)
    |                                              |
    | close()                                      | close()
    |                                              |
```
## 🖥️ Sunucu Tarafı (Server)

- **`socket()`** → Sunucu soketini oluşturur  
- **`bind()`** → Soketi belirli bir IP adresine ve porta bağlar  
- **`listen()`** → Gelen bağlantıları dinlemeye başlar  
- **`poll()`** → Tüm soketleri (yeni bağlantılar + mevcut client’lar) izler  
- **`accept()`** → Yeni bir istemci bağlantısını kabul eder  
- **`recv()`** → İstemciden gelen veriyi alır  
- **`send()`** → İstemciye cevap gönderir  
- **`close()`** → Bağlantıyı kapatır  

---

## 👤 İstemci Tarafı (Client)

- **`socket()`** → İstemci soketini oluşturur  
- **`connect()`** → Sunucuya bağlanma isteği gönderir  
- **`send()`** → Sunucuya veri gönderir (**request**)  
- **`recv()`** → Sunucudan gelen yanıtı alır (**response**)  
- **`close()`** → Bağlantıyı kapatır  
---

## ⚡ Non-Blocking Mod

Normalde **`recv()`** veya **`send()`** çağrıları **bloklayıcıdır**. Yani veri yoksa işlem bekler.  
**Non-blocking mod** sayesinde:

- Sunucu, hiçbir bağlantıda beklemeden diğer soketlere geçebilir  
- **`poll()`** fonksiyonu kullanılarak hangi soketlerde **okunacak/gönderilecek veri** olduğu öğrenilir  
- Böylece yüzlerce istemci aynı anda yönetilebilir  

```cpp
// Soketi non-blocking moda alma
fcntl(sockfd, F_SETFL, O_NONBLOCK);
```

---
## 📂 Proje Yapısı (Tree)

Kod organizasyonu aşağıdaki gibidir:

```bash
ft_irc/
├── includes/     # Header dosyaları
├── src/          # Kaynak kodlar
├── main.cpp      # Giriş noktası
├── Makefile      # Derleme talimatları
└── README.md     # Dokümantasyon
```

---
## 💬 Temel IRC Komutları

- **`PASS <password>`** → Sunucuya giriş için parola  
- **`NICK <nickname>`** → Takma ad belirleme  
- **`USER <username> 0 * :Real Name`** → Kullanıcı kaydı  
- **`JOIN #kanal`** → Kanala katılma  
- **`PART #kanal`** → Kanaldan ayrılma  
- **`PRIVMSG <hedef> :mesaj`** → Özel mesaj gönderme  
- **`QUIT`** → Sunucudan çıkış  

---

## ⚙️ IRC Modları (MODE Komutu)

IRC protokolünde `MODE` komutu, **kullanıcı** ve **kanal** üzerinde ayarlar yapmaya yarar.  

#### 👤 Kullanıcı Modları
- **`+o`** → Kullanıcıyı operatör yapar (OP)  
- **`-o`** → Kullanıcının operatör yetkisini alır  

#### #️⃣ Kanal Modları
- **`+i`** → Kanalı davetiyeye özel (invite-only) yapar  
- **`+t`** → Sadece operatörün konu (topic) değiştirmesine izin verir  
- **`+k <şifre>`** → Kanala giriş için şifre belirler  
- **`+l <limit>`** → Kanal için maksimum kullanıcı limiti belirler  
- **`+o <nick>`** → Belirtilen kullanıcıya kanal içinde OP yetkisi verir  


📌 Örnek Kullanımlar:
```text
/MODE #genel +i          → #genel kanalını davetiyeye özel yap  
/MODE #genel +k 1234     → #genel kanalına giriş için şifre "1234" koy  
/MODE #genel +l 10       → #genel kanalına en fazla 10 kişi girebilir  
/MODE #genel +o alice    → alice kullanıcısına OP yetkisi ver  
```
---

## 🛠️ Kurulum ve Derleme

### 📋 Ön Gereksinimler

* **C++98 Uyumluluğuna Sahip Bir Derleyici**: `g++` veya `clang++` önerilir.
* **Make**: Proje derleme sistemi için gereklidir.
* **Linux İşletim Sistemi**: Sunucu tarafı uygulamalar için idealdir.

### 🔧 Derleme Talimatları

Projeyi klonladıktan sonra, aşağıdaki komutları kullanarak sunucuyu derleyebilirsiniz:

1.  **Temizlik**: Önceki derleme kalıntılarını temizleyin.
    ```bash
    make clean
    ```
2.  **Derleme**: Sunucuyu derleyin.
    ```bash
    make
    ```

---
    
## ▶️ Çalıştırma

Derleme işlemi başarıyla tamamlandıktan sonra, sunucuyu belirli bir port ve isteğe bağlı bir şifre ile başlatabilirsiniz.

### 🚀 Sunucu Başlatma

```bash
./ft_irc <port> [password]
```

Sunucuyu aşağıdaki komutla çalıştırabilirsiniz:

```bash
./ft_irc 6667 pass42
```


### 🧪  İstemci Bağlantısı / Örnek Kullanım

Bağlantı için çeşitli IRC istemcilerini kullanabilirsiniz: **HexChat**, **KVIrc**, veya basit testler için `nc`.  
`nc` (**netcat**) ile sunucuya bağlanıp temel bir senaryoyu test edebilirsiniz:

```bash
nc localhost 6667

PASS pass42 → Sunucu parolasını girer

NICK alice → Kullanıcı adını belirler

USER alice 0 * :Alice Wonderland → Kullanıcıyı sisteme kaydeder

JOIN #genel → #genel kanalına katılır

PRIVMSG #genel :Merhaba! → Kanala mesaj gönderir
```


---

## 👥 Takım

Bu proje, **42 İstanbul** öğrencileri tarafından ortak bir çalışma olarak geliştirilmiştir.

| Developer     | GitHub Profile |
|---------------|----------------|
| nalveiz       | [![GitHub](https://img.shields.io/badge/GitHub-nalveiz-blue?logo=github)](https://github.com/nalveiz) |
| seyhmusoksak  | [![GitHub](https://img.shields.io/badge/GitHub-seyhmusoksak-blue?logo=github)](https://github.com/seyhmusoksak) |
