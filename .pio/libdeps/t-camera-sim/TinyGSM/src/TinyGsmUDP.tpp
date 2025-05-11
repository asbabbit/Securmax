/**
 * @file        TinyGsmUDP.tpp
 * @author      Alma Babbitt
 * @date        April 2025
 */

#ifndef SRC_TINYGSMUDP_H
#define SRC_TINYGSMUDP_H

#include "TinyGsmCommon.h"

#define TINY_GSM_MODEM_HAS_UDP

#include "TinyGsmFifo.h"

#if !defined (TINY_GSM_RX_BUFFER)
#define TINY_GSM_RX_BUFFER 64
#endif

#define TINY_GSM_CLIENT_CONNECT_OVERRIDES                                 \
    int connect(IPAddress ip, uint16_t port, int timeout_s)               \
    {                                                                     \
        return connect(TinyGsmStringFromIp(ip).c_str(), port, timeout_s); \
    }                                                                     \
    int connect(const char *host, uint16_t port) override                 \
    {                                                                     \
        return connect(host, port, 75);                                   \
    }                                                                     \
    int connect(IPAddress ip, uint16_t port) override                     \
    {                                                                     \
        return connect(ip, port, 75);                                     \
    }

template <class modemType, uint8_t muxCount>
class TinyGsmUDP
{
public:
    void maintain()
    {
        return thisModem().maintainImpl();
    }

protected:
    inline const modemType &thisModem() const
    {
        return static_cast<const modemType &>(*this);
    }

    inline modemType &thisModem()
    {
        return static_cast< modemType &>(*this);
    }

public:
    class GsmClient : public Client
    {
        // Make all classes created from the modem template friends
        friend class TinyGsmUDP<modemType, muxCount>;
        typedef TinyGsmFifo<uint8_t, TINY_GSM_RX_BUFFER> RxFifo;

    public:
        static inline String TinyGsmStringFromIp(IPAddress ip)
        {
            String host;
            host.reserve(16);
            host += ip[0];
            host += ".";
            host += ip[1];
            host += ".";
            host += ip[2];
            host += ".";
            host += ip[3];
            return host;
        }

        bool begin_packet(size_t len, const char* host, uint16_t port)
        {
            if(len == 0) { return false; }
            TINY_GSM_YIELD();
            at->maintain();
            return at->modemBeginPacket(len, mux, host, port);
        }

        int16_t end_packet(){
            return at->modemEndPacket();
        }

        // Write byte by byte to modem using stream and AT commands
        size_t write(const void *buf, size_t size)
        {
            if(!buf) { return 0; }
            return at->modemWrite(buf,size);
        }

        size_t write(uint8_t c) override
        {
            return write(&c, 1);
        }

        size_t write(const char *str)
        {
            if (str == NULL)
                return 0;
            return write((const uint8_t *)str, strlen(str));
        }

        int available() override
        {
            if (!rx.size())
            {
                if (millis() - prev_check > 500)
                {
                    // setting got_data to true will tell maintain to run
                    // modemGetAvailable(mux)
                    got_data = true;
                    prev_check = millis();
                }
                at->maintain();
            }
            return static_cast<uint16_t>(rx.size()) + sock_available;
        }
        
        int read(uint8_t *buf, size_t size) override
        {
            TINY_GSM_YIELD();
            size_t cnt = 0;

            at->maintain();
            while (cnt < size)
            {
                size_t chunk = TinyGsmMin(size - cnt, rx.size());
                if (chunk > 0)
                {
                    rx.get(buf, chunk);
                    buf += chunk;
                    cnt += chunk;
                    continue;
                }
                // Workaround: Some modules "forget" to notify about data arrival
                if (millis() - prev_check > 500)
                {
                    // setting got_data to true will tell maintain to run
                    // modemGetAvailable()
                    got_data = true;
                    prev_check = millis();
                }
                // TODO(vshymanskyy): Read directly into user buffer?
                at->maintain();
                if (sock_available > 0)
                {
                    int n = at->modemRead(TinyGsmMin((uint16_t)rx.free(), sock_available),
                                            mux);
                    if (n == 0)
                        break;
                }
                else
                {
                    break;
                }
            }
            return cnt;
        }
        int read() override
        {
            uint8_t c;
            if (read(&c, 1) == 1)
            {
                return c;
            }
            return -1;
        }
        int peek() override
        {
            return (uint8_t)rx.peek();
        }

        void flush() override
        {
            at->stream.flush();
        }

        uint8_t connected() override
        {
            if (available())
            {
                return true;
            }
            return sock_connected;
        }

        operator bool() override
        {
            return connected();
        }

        String remoteIP() TINY_GSM_ATTR_NOT_IMPLEMENTED;

    protected:
        // Read and dump anything remaining in the modem's internal buffer.
        // Using this in the client stop() function.
        // The socket will appear open in response to connected() even after it
        // closes until all data is read from the buffer.
        // Doing it this way allows the external mcu to find and get all of the
        // data that it wants from the socket even if it was closed externally.
        inline void dumpModemBuffer(uint32_t maxWaitMs)
        {
            TINY_GSM_YIELD();
            uint32_t startMillis = millis();
            while (sock_available > 0 && (millis() - startMillis < maxWaitMs))
            {
                rx.clear();
                at->modemRead(TinyGsmMin((uint16_t)rx.free(), sock_available), mux);
            }
            rx.clear();
            at->streamClear();
        }

        modemType *at;           // Interface object with modem via stream
        uint8_t mux;             // Identifier for which socket
        uint16_t sock_available; // Number of bytes available to read from the modem's receive buffer
        uint32_t prev_check;     // Timestamp of last check for new data availability
        bool sock_connected;     // If socket connected to server
        bool got_data;           // Flag to trigger checking for new available data in modem
        RxFifo rx;               // Software buffer for received data waiting to be read by application        
    };

protected:
    // Sets sock_available
    void maintainImpl()
    {
        for (int mux = 0; mux < muxCount; mux++)
        {
            GsmClient *sock = thisModem().sockets[mux];
            if (sock && sock->got_data)
            {
                sock->got_data = false;
                sock->sock_available = thisModem().modemGetAvailable(mux);
            }
        }
        while (thisModem().stream.available())
        {
            thisModem().waitResponse(15, NULL, NULL);
        }
    }

    // Move char from MCU receive buffer to software buffer
    inline void moveCharFromStreamToFifo(uint8_t mux)
    {
        if (!thisModem().sockets[mux])
            return;
        uint32_t startMillis = millis();
        while (!thisModem().stream.available() &&
                (millis() - startMillis < thisModem().sockets[mux]->_timeout))
        {
            TINY_GSM_YIELD();
        }
        char c = thisModem().stream.read();
        thisModem().sockets[mux]->rx.put(c);
    }
};

#endif // SRC_TINYGSMUDP_H