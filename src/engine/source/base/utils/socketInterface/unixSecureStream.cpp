#include "unixSecureStream.hpp"

#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include <logging/logging.hpp>

namespace base::utils::socketInterface
{

ssize_t unixSecureStream::recvWaitAll(void* buf, size_t size) const noexcept
{
    ssize_t offset {}; // offset in the buffer
    ssize_t recvb {};  // Recived bytes

    for (offset = 0; offset < size; offset += recvb)
    {
        recvb = recv(getFD(), (char*)buf + offset, size - offset, 0);

        if (0 >= recvb)
        {
            offset = recvb;
            break;
        }
    }

    return offset;
}

SendRetval unixSecureStream::sendMsg(const std::string& msg)
{
    auto result {SendRetval::SOCKET_ERROR};
    auto payloadSize {static_cast<uint32_t>(msg.size())};
    const auto HEADER_SIZE {sizeof(uint32_t)};

    // Validate
    if (!isConnected())
    {
        socketConnect();
    }

    if (0 >= payloadSize)
    {
        result = SendRetval::SIZE_ZERO;
    }
    else if (getMaxMsgSize() < payloadSize)
    {
        result = SendRetval::SIZE_TOO_LONG;
    }
    else
    {
        payloadSize++; // send the null terminator
        // MSG_NOSIGNAL prevent broken pipe signal
        auto success {send(getFD(), &payloadSize, HEADER_SIZE, MSG_NOSIGNAL)
                      == HEADER_SIZE};
        success =
            success
            && (send(getFD(), msg.c_str(), payloadSize, MSG_NOSIGNAL) == payloadSize);

        if (success)
        {
            result = SendRetval::SUCCESS;
        }
        else if (EAGAIN == errno || EWOULDBLOCK == errno)
        {
            WAZUH_LOG_WARN("wdb socket is full: {} ({})", strerror(errno), errno);
        }
        else if (EPIPE == errno)
        {
            // Recoverable case, socket is disconnected remotely.
            socketDisconnect(); // Force reconnect in next call.
            throw RecoverableError("sendMsg socket is disconnected.");
        }
    }

    return result;
}

std::vector<char> unixSecureStream::recvMsg()
{
    // Check recive msg
    const auto checkRcv = [this](const ssize_t rcvBytes)
    {
        if (0 > rcvBytes)
        {
            const auto msg = fmt::format("recvMsg: {} ({})", strerror(errno), errno);
            socketDisconnect();
            if (ECONNRESET == errno)
            {

                throw RecoverableError(msg);
            }
            throw std::runtime_error(msg);
        }
        else if (0 == rcvBytes)
        {
            // Remote disconect recoverable case
            socketDisconnect();
            throw RecoverableError("recvMsg: socket disconnected."); // errno is not set
        }
    };

    uint32_t msgSize; // Message size (Header readed)
    auto recvb {recvWaitAll(&msgSize, sizeof(msgSize))};
    checkRcv(recvb);

    if (getMaxMsgSize() < msgSize)
    {
        socketDisconnect();
        std::runtime_error("recvMsg: message size too long.");
    }

    std::vector<char> recvMsg;
    recvMsg.resize(msgSize + 1, '\0');

    recvb = recvWaitAll(&(recvMsg[0]), msgSize);
    checkRcv(recvb);

    return recvMsg;
}

std::string unixSecureStream::recvString()
{
    return std::string(recvMsg().data());
}

} // namespace base::utils::socketInterface