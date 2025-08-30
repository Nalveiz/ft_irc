#!/bin/bash

# IRC Server Test Script
echo "🚀 IRC Server Test Başlatılıyor..."
echo "📡 Sunucu: localhost:6667"
echo "🔑 Şifre: test123"
echo ""

# Test komutları
echo "📝 Test Komutları:"
echo "1. PASS test123"
echo "2. NICK TestUser"
echo "3. USER TestUser 0 * :Real Name"
echo "4. PING :test"
echo "5. QUIT :Goodbye"
echo ""
echo "🔧 Manuel Test için:"
echo "   nc localhost 6667"
echo "   Yukarıdaki komutları sırayla girin"
echo ""

# Netcat test
echo "🧪 Netcat ile otomatik test başlatılıyor..."
{
    echo "PASS test123"
    sleep 1
    echo "NICK TestUser"
    sleep 1
    echo "USER TestUser 0 * :Real Name"
    sleep 1
    echo "PING :test123"
    sleep 1
    echo "QUIT :Test completed"
    sleep 1
} | nc localhost 6667

echo "✅ Test tamamlandı!"
